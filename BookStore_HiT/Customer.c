// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
//#include "LogFunction.h"
#include "Customer.h"
#include "OrderItemNode.h"

#define SEARCH_BY_ID 0
#define SEARCH_BY_NAME 1
#define SEARCH_BY_AUTHOR 2
#define SEARCH_BY_PRICE 3

//freeing customers orders and items 
void freeCustomerData(Customer* customer) {
	OrderListNode* current = customer->purchases;
	while (current != NULL) {
		OrderListNode* next = current->next;
		OrderItemNode* item_current = current->data.itemsList;
		while (item_current != NULL) {
			OrderItemNode* item_next = item_current->next;
			free(item_current);
			item_current = item_next;
		}
		free(current);
		current = next;
	}
}

// create a new node with customer data
CustomerNode* createNewCustomer(int customer_id, char* name, int discount) {
	CustomerNode* newNode = (CustomerNode*)malloc(sizeof(CustomerNode));
	time_t joinDate = time(NULL);
	newNode->customer.customer_id = customer_id;
	strcpy(newNode->customer.name, name);
	newNode->customer.discount = discount;
	newNode->customer.joinedDate = joinDate;
	newNode->customer.countOfPurchases = 0;
	newNode->customer.purchases = NULL;
	newNode->next = NULL;
	return newNode;
}

// for refund the order must have been taking place at max 14 days ago
// updating purchases and quantity of item
void refundItems(int customer_id, int order_id, OrderItemNode* itemsToRemove, CustomerNode* head, BooksHashTable* ht) {
	CustomerNode* current = head;
	while (current != NULL) {
		if (current->customer.customer_id == customer_id) {
			// customer found, check purchases
			OrderListNode* orderCurrent = current->customer.purchases;
			while (orderCurrent != NULL) {
				if (orderCurrent->data.order_id == order_id) {
					// order found, check if order is within 14 days
					time_t currentTime = time(NULL);
					double timediff = difftime(currentTime, orderCurrent->data.date);
					if (timediff <= 1209600) {
						// order is within 14 days, refund items
						OrderItemNode* itemCurrent = orderCurrent->data.itemsList;
						while (itemCurrent != NULL) {
							if (itemsToRemove->item.id == itemCurrent->item.id) {
								// item found, refund it
								itemCurrent->item.count -= itemsToRemove->item.count;
								BookNode* book = searchBooks(ht, itemCurrent->item.id, SEARCH_BY_ID);
								book->book.quantity += itemsToRemove->item.count;
								addBook(ht, book->book);
								updateTotalPrice(orderCurrent, current->customer.discount);
								return;
							}
							itemCurrent = itemCurrent->next;
						}
						// item not found in order
						printf("error: item with id %d not found in order.\n", itemsToRemove->item.id);
						return;
					}
					else {
						// order is more than 14 days old
						printf("error: order is more than 14 days old.\n");
						return;
					}
				}
				orderCurrent = orderCurrent->next;
			}
			// order not found
			printf("error: order with id %d not found.\n", order_id);
			return;
		}
		current = current->next;
	}
	// customer not found
	printf("error: customer with id %d not found.\n", customer_id);
}

// get list of customers and get the specific customer purchases list
OrderListNode* getPurchasesByCustomer(CustomerNode* customerList, int customer_id) {
	CustomerNode* current = customerList;
	while (current != NULL) {
		if (current->customer.customer_id == customer_id) {
			// Customer found, return purchases
			return current->customer.purchases;
		}
		current = current->next;
	}
	// Customer not found
	printf("Error: Customer with ID %d not found.\n", customer_id);
	return NULL;
}

// writing customers data including orders and items to bin file
void writeCustomersToBinaryFile(CustomerNode* customer, char* filename) {
	FILE* fp = fopen(filename, "wb");
	if (fp == NULL) {
		// handle file open error
		logAction("Error: Customers file is corrupted.");
	}
	while (customer != NULL) {
		// write customer data:
		
		//write customer id
		fwrite(&customer->customer.customer_id, sizeof(int), 1, fp);
		//write customer name
		fwrite(&customer->customer.name, sizeof(char), 30, fp);
		//write customer discount
		fwrite(&customer->customer.discount, sizeof(int), 1, fp);
		//write customer join date
		fwrite(&customer->customer.joinedDate, sizeof(time_t), 1, fp);
		// write number of orders for this customer
		int numOrders = customer->customer.countOfPurchases;
		fwrite(&numOrders, sizeof(int), 1, fp);
		
		// write order data
		OrderListNode* orderNode = customer->customer.purchases;
		while (orderNode != NULL) {
			Order order = orderNode->data;

			//write order's employee id
			fwrite(&order.employee_id, sizeof(int), 1, fp);
			//write order's customer id
			fwrite(&order.customer_id, sizeof(int), 1, fp);
			//write order id
			fwrite(&order.order_id, sizeof(int), 1, fp);
			//write date of order 
			fwrite(&order.date, sizeof(time_t), 1, fp);
			//write total price of the order
			fwrite(&order.totalPrice, sizeof(double), 1, fp);

			// write number of items for this order
			int numItems = countItems(order.itemsList);
			fwrite(&numItems, sizeof(int), 1, fp);

			// write order item data
			OrderItemNode* itemNode = order.itemsList;
			while (itemNode != NULL) {
				OrderItem item = itemNode->item;
				fwrite(&item, sizeof(OrderItem), 1, fp);
				itemNode = itemNode->next;
			}
			orderNode = orderNode->next;
		}
		customer = customer->next;
	}
	fclose(fp);
}

//read customers data including orders and items from the bin file
CustomerNode* readCustomersFromBinaryFile(char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {
		// handle file open error
		logAction("Error: Customers file is corrupted.");
		CustomerNode* customerList = (CustomerNode*)malloc(sizeof(CustomerNode));
		customerList->customer.purchases = (OrderListNode*)malloc(sizeof(OrderListNode));
		customerList->customer.purchases = NULL;
		customerList = NULL;
		return customerList;
	}

	CustomerNode* customerHead = NULL;
	CustomerNode* current = customerHead;

	while (!feof(fp)) {
		Customer customer;
		time_t joinDate;
		int customerId, discount, numOrders;
		char name[30];

		// read customer id
		if (fread(&customerId, sizeof(int), 1, fp) == 0) {
			break;
		}
		customer.customer_id = customerId;
		
		// read customer name
		if (fread(&name, sizeof(char), 30, fp) == 0) {
			break;
		}
		strcpy(customer.name, name);
		
		// read customer discount
		if (fread(&discount, sizeof(int), 1, fp) == 0) {
			break;
		}
		customer.discount = discount;
		
		// read customer join date
		if (fread(&joinDate, sizeof(time_t), 1, fp) == 0) {
			break;
		}
		customer.joinedDate = joinDate;
		
		// read number of orders for this customer
		if (fread(&numOrders, sizeof(int), 1, fp) == 0) {
			break;
		}
		customer.countOfPurchases = numOrders;
		
		// read order data
		customer.purchases = NULL;
		OrderListNode* orderTail = NULL;
		for (int i = 0; i < numOrders; i++) {
			Order order;
			int employeeId, customerId, orderId, numItems;
			double totalPrice;
			time_t date;

			// read order's employee id
			if (fread(&employeeId, sizeof(int), 1, fp) == 0) {
				break;
			}
			order.employee_id = employeeId;
			// read order's customer id
			if (fread(&customerId, sizeof(int), 1, fp) == 0) {
				break;
			}
			order.customer_id = customerId;
			// read order id
			if (fread(&orderId, sizeof(int), 1, fp) == 0) {
				break;
			}
			order.order_id = orderId;
			// read date of order
			if (fread(&date, sizeof(time_t), 1, fp) == 0) {
				break;
			}
			order.date = date;
			// read totalPrice of order
			if (fread(&totalPrice, sizeof(double), 1, fp) == 0) {
				break;
			}
			order.totalPrice = totalPrice;

			// read number of items for this order
			if (fread(&numItems, sizeof(int), 1, fp) == 0) {
				break;
			}
			// read order item data
			order.itemsList = NULL;
			OrderItemNode* itemTail = NULL;

			for (int j = 0; j < numItems; j++) {
				OrderItem item;
				// read item data
				if (fread(&item, sizeof(OrderItem), 1, fp) == 0) {
					break;
				}
				// add item to the order's item list
				OrderItemNode* itemNode = (OrderItemNode*)malloc(sizeof(OrderItemNode));
				itemNode->item = item;
				itemNode->next = NULL;
				if (order.itemsList == NULL) {
					order.itemsList = itemNode;
				}
				else {
					itemTail->next = itemNode;
				}
				itemTail = itemNode;
			}
			// add order to the customer's purchase list
			OrderListNode* orderNode = (OrderListNode*)malloc(sizeof(OrderListNode));
			orderNode->data = order;
			orderNode->next = NULL;
			if (customer.purchases == NULL) {
				customer.purchases = orderNode;
			}
			else {
				orderTail->next = orderNode;
			}
			orderTail = orderNode;
		}
		// add customer to the customer list
		CustomerNode* customerNode = (CustomerNode*)malloc(sizeof(CustomerNode));
		customerNode->customer = customer;
		customerNode->next = NULL;
		if (customerHead == NULL) {
			customerHead = customerNode;
		}
		else {
			current->next = customerNode;
		}
		current = customerNode;
	}
	fclose(fp);
	return customerHead;
}

//remove customer from list by id
Customer* removeCustomerFromListById(CustomerNode** customers, int customer_id) {
	Customer* removedCustomer;
	CustomerNode* current = *customers;
	CustomerNode* prev = NULL;
	while (current != NULL) {
		if (current->customer.customer_id == customer_id) {
			// Customer found, remove it from the list
			if (prev == NULL) {
				// Customer is the head of the list
				removedCustomer = &(current->customer);
				*customers = current->next;
			}
			else {
				// Customer is not the head of the list
				removedCustomer = &(current->customer);
				prev->next = current->next;
			}
			freeCustomerData(removedCustomer);
			free(current);
			return removedCustomer;
		}
		prev = current;
		current = current->next;
	}
	// Customer not found
	printf("Error: Customer with ID %d not found.\n", customer_id);
	return NULL;
}

//find customer in list by id
CustomerNode* findCustomerInList(CustomerNode* head, int customer_id) {
	CustomerNode* current = head;
	while (current != NULL) {
		if (current->customer.customer_id == customer_id) {
			// Customer found, return pointer to the node
			return current;
		}
		current = current->next;
	}
	// Customer not found
	return NULL;
}

//print customers list
void printCustomersList(CustomerNode* head) {
	time_t joinDate;
	struct tm* time_info;
	char timeString[11];
	
	CustomerNode* current = head;
	while (current != NULL) {
		joinDate = current->customer.joinedDate;
		time_info = localtime(&joinDate);
		strftime(timeString, sizeof(timeString), "%Y-%m-%d", time_info);

		printf("Customer ID: %d, Name: %s, Discount: %d%%, Count of Purchases: %d, Joined Date: %s\n",
			current->customer.customer_id,
			current->customer.name, 
			current->customer.discount, 
			current->customer.countOfPurchases,
			timeString);
		current = current->next;
	}
}

//creates customer's mock data
CustomerNode* createMockData() {
	// Create mock customer data
	CustomerNode* customerHead = NULL;
	CustomerNode* customerTail = NULL;
	for (int i = 0; i < 3; i++) {
		Customer customer = { i, "Customer name", i * 10, i + 1 };
		customer.purchases = NULL;

		// create mock order data
		OrderListNode* orderHead = NULL;
		OrderListNode* orderTail = NULL;
		for (int j = 0; j < customer.countOfPurchases; j++) {
			Order order = { i, customer.customer_id, j, time(NULL) };
			order.itemsList = NULL;

			// create mock order item data
			OrderItemNode* itemHead = NULL;
			OrderItemNode* itemTail = NULL;
			for (int k = 0; k < j + 1; k++) {
				OrderItem item = { k, k + 1, k * 10.0 };
				OrderItemNode* itemNode = (OrderItemNode*)malloc(sizeof(OrderItemNode));
				itemNode->item = item;
				itemNode->next = NULL;
				if (itemHead == NULL) {
					itemHead = itemNode;
					itemTail = itemNode;
				}
				else {
					itemTail->next = itemNode;
					itemTail = itemNode;
				}
			}
			order.itemsList = itemHead;
			OrderListNode* orderNode = (OrderListNode*)malloc(sizeof(OrderListNode));
			orderNode->data = order;
			orderNode->next = NULL;
			if (orderHead == NULL) {
				orderHead = orderNode;
				orderTail = orderNode;
			}
			else {
				orderTail->next = orderNode;
				orderTail = orderNode;
			}
		}

		customer.purchases = orderHead;
		CustomerNode* customerNode = (CustomerNode*)malloc(sizeof(CustomerNode));
		customerNode->customer = customer;
		customerNode->next = NULL;
		if (customerHead == NULL) {
			customerHead = customerNode;
			customerTail = customerNode;
		}
		else {
			customerTail->next = customerNode;
			customerTail = customerNode;
		}
	}

	// return customer list 
	return customerHead;
}

//insert customer to list
void insertCustomerNode(CustomerNode** head, CustomerNode* newNode) {
	CustomerNode* current = *head;
	CustomerNode* previous = NULL;
	while (current != NULL && current->customer.joinedDate > newNode->customer.joinedDate) {
		previous = current;
		current = current->next;
	}
	if (previous == NULL) {
		// newNode is the new head
		newNode->next = *head;
		*head = newNode;
	}
	else {
		// insert newNode between previous and current
		newNode->next = current;
		previous->next = newNode;
	}
}
