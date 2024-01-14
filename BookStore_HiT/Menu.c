// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include "Employee.h"
#include "BooksHashTable.h"
#include "Crypto.h"
#include "Customer.h"
//#include "LogFunction.h"

#define EMPLOYEE_FILENAME "employee.bin"
#define BOOKS_FILENAME "books.bin"
#define CUSTOMER_FILENAME "customers.bin"
#define SEARCH_BY_ID 0
#define SEARCH_BY_NAME 1
#define SEARCH_BY_AUTHOR 2
#define SEARCH_BY_PRICE_SMALLER 3
#define SEARCH_BY_PRICE_GREATER 4
#define SEARCH_BY_BESTSELLER 5
#define SEARCH_BY_NEWER_THAN_PUBLISH 6
#define SEARCH_BY_OLDER_THAN_PUBLISH 7
#define SEARCH_BY_CATEGORY 8

#define SEARCH_BY_ID 0
#define SEARCH_BY_FNAME 1
#define SEARCH_BY_LNAME 2
#define SEARCH_BY_USERNAME 3

#define STR_LEN 30

void viewCustomerOrdersMenu(CustomerNode* customerList) {
	int customer_id;
	OrderListNode* orderList;

	system("cls");
	printf("\n-----View customer orders-----\n");

	printf("Enter customer ID: ");
	scanf("%d", &customer_id);

	orderList = getPurchasesByCustomer(customerList, customer_id);
	if (orderList == NULL) {
		printf("Error: Customer with ID %d not found or has no orders.\n", customer_id);
		logAction("Customer's not found.");
		Sleep(1500);
		return;
	}
	logAction("Customer found.");
	// Print the customer's order list
	printf("\n--- Orders for Customer ID %d ---\n", customer_id);
	printOrdersList(orderList, true);

	printf("\nPress any key to get back to main menu...");
	getch();
}

void deleteCustomerMenu(CustomerNode** customers) {
	int customer_id;
	bool main_menu_flag = false;
	while (!main_menu_flag) {
		printf("\n-----Delete Customer Menu-----\n");
		if (!(*customers)) {
			printf("There are no customers to remove. Please try again later..");
			logAction("Customer's not found.");
			Sleep(1500);
			main_menu_flag = true;
		}
		else {
			printf("\nCustomers data:\n");
			printCustomersList(*customers);

			printf("\nEnter the ID of the customer you want to delete or press 0 to go back to main menu: ");
			scanf("%d", &customer_id);

			if (customer_id == 0) {
				main_menu_flag = true;
			}
			else {
				CustomerNode* customerToRemove = findCustomerInList(*customers, customer_id);
				if (customerToRemove) {
					printf("\nSuccessfully deleted customer with ID %d and name %s.\n\n", customerToRemove->customer.customer_id, customerToRemove->customer.name);
					removeCustomerFromListById(customers, customer_id);
					Sleep(2000);
					logAction("Customer removed from the customers list.");
				}
				else {
					printf("Error: Failed to delete customer with ID %d.\n\n", customer_id);
					Sleep(2000);
					logAction("Customer failed to remove from the customers list.");
				}
			}

			system("cls");
		}
	}
}

void refundItemMenu(CustomerNode* head, BooksHashTable* ht) {
	int customer_id, order_id, item_id, item_count;
	double singlePrice;

	printf("\n-----Refund Item Menu-----\n");
	printf("Enter the following details to refund an item:\n");

	printf("Customer ID: ");
	scanf("%d", &customer_id);

	//check if customer exists
	CustomerNode* currentCustomer = findCustomerInList(head, customer_id);
	if (!currentCustomer) {
		printf("Customer doesn't exists in our systems.");
		logAction("Customer doesn't exists in the list.");
		Sleep(1000);
	}
	else {
		printf("Order ID: ");
		scanf("%d", &order_id);

		Order* currentOrder = findOrderInList(currentCustomer->customer.purchases, order_id);
		if (!currentOrder) {
			printf("Order doesn't exist for customer id: %d", customer_id);
			logAction("Order doesn't exists.");
			Sleep(1000);
		}
		else {
			printf("Item ID: ");
			scanf("%d", &item_id);

			OrderItem* itemToRemove = findItemInList(currentOrder->itemsList, item_id);
			if (!itemToRemove) {
				printf("There is no such item in this order.");
				logAction("Item doesn't exists.");
				Sleep(1000);
			}
			else {
				time_t currentTime = time(NULL);
				double timediff = difftime(currentTime, currentOrder->date);
				if (timediff <= 1209600) {
					// order is within 14 days, refund items
					printf("Item quantity to refund: ");
					scanf("%d", &item_count);

					if (item_count >= 0 && item_count <= itemToRemove->count) {
						//get single quantity price
						if (itemToRemove->count == 0) {
							singlePrice = itemToRemove->price;
						}
						else {
							singlePrice = itemToRemove->price / itemToRemove->count;
						}
						//update new quantity and price
						itemToRemove->count -= item_count;
						itemToRemove->price = singlePrice * itemToRemove->count;

						//update order price
						updateTotalPrice(currentOrder, currentCustomer->customer.discount);
						logAction("Item refunded from the Order.");
						printf("Item refunded successfuly.");
						Sleep(1000);

						//update book quantity 
						Book* book = getBookById(ht, item_id);
						book->quantity += item_count;
					}
					else {
						printf("Item quantity to refund is invalid.");
						logAction("Quantity invalid.");
						Sleep(1000);
					}
				}
				else {
					printf("Order was made more than 14 days ago. Refund is unavailable.");
					logAction("More than 14 days.");
					Sleep(1000);
				}
			}
		}
	}
}

void deleteEmployeeMenu(Employee** employees) {
	bool main_menu_flag = false;
	bool valid_input = false;
	bool adminSearch = false;
	char search_term[STR_LEN];
	EmployeeNode* search_results = NULL;

	int sub_choice = -1;

	while (sub_choice != 0 && !main_menu_flag) {
		//sub_choice = -1;
		while (!valid_input) {
			system("cls");
			printf("\n-----Delete Employee Menu-----\n");
			printf("1. ID\n");
			printf("2. First Name\n");
			printf("3. Last Name\n");
			printf("4. Username\n");
			printf("0. Go back to main menu\n");
			printf("Enter your choice: ");
			if (scanf("%d", &sub_choice) == 1) {
				if (sub_choice >= 0 && sub_choice <= 4) {
					valid_input = true;
				}
				else {
					printf("Invalid input, please enter a number between 0 and 4\n");
					Sleep(1000);
					valid_input = false;
				}
			}
			else {
				printf("Invalid input, please enter a number between 0 and 4\n");
				// Discard the remaining invalid input
				int c;
				while ((c = getchar()) != '\n' && c != EOF);

				Sleep(1000);
				valid_input = false;
			}
		}
		system("cls");
		switch (sub_choice) {
		case 1:
			printf("Enter employee's id to delete: ");
			scanf("%s", search_term);
			if (atoi(search_term) == 1) {
				printf("Error: you cannot delete the admin.");
				logAction("Trying to remove the admin.");
				adminSearch = true;
				Sleep(1000);
			}
			else {
				search_results = searchEmployees(*employees, search_term, SEARCH_BY_ID);
			}
			break;
		case 2:
			printf("Enter employee's first name to delete: ");
			scanf("%s", search_term);
			search_results = searchEmployees(*employees, search_term, SEARCH_BY_FNAME);
			break;
		case 3:
			printf("Enter employee's last name to delete: ");
			scanf("%s", search_term);
			search_results = searchEmployees(*employees, search_term, SEARCH_BY_LNAME);
			break;
		case 4:
			printf("Enter employee's username to delete: ");
			scanf("%s", search_term);
			if (strcmp(search_term, "admin") == 0) {
				printf("Error: you cannot delete the admin.");
				logAction("Trying to remove the admin.");
				adminSearch = true;
				Sleep(1000);
			}
			else {
				search_results = searchEmployees(*employees, search_term, SEARCH_BY_USERNAME);
			}
			break;
		case 0:
			main_menu_flag = true;
			continue;
			break;
		default:
			printf("\nInvalid choice. Try again.\n");
			Sleep(1000);
			continue;
			break;
		}

		if (!adminSearch) {
			if (search_results == NULL) {
				printf("Employee not found. please try again..");
				logAction("Employee not found.");
				valid_input = false;
				sub_choice = -1;
				Sleep(1000);
				continue;
			}
			else {
				system("cls");
				printEmployees(search_results);
				int id;
				printf("\n\nWhat is the ID of the employee you want to delete out of these matching results:");
				scanf("%d", &id);
				if (id == 1) { //trying to delete admin
					printf("Error: you cannot delete the admin.");
					logAction("Trying to remove the admin.");
					valid_input = false;
					sub_choice = -1;
					Sleep(1000);
				}
				else {
					if (!findEmployeeInList(search_results, id)) {
						printf("Invalid employee ID, please try again.. ");
						logAction("Employee not found.");
						valid_input = false;
						sub_choice = -1;
						Sleep(1000);
					}
					else {
						printf("\nAre you sure you want to delete?(y/n) \n\n");
						char confirm;
						scanf(" %c", &confirm);
						if (confirm == 'y') {
							removeEmployeeFromListById(employees, id);
							sub_choice = 0;

							logAction("Employee Deleted Successfuly.");
							printf("Employee deleted successfuly.\n");
							Sleep(1000);
						}
						else if (confirm == 'n') {
							printf("Employee delete cancelled.\n");
							logAction("Employee delete canceled.");
							valid_input = false;
							sub_choice = -1;
							Sleep(1000);
						}
						else {
							printf("Invalid input, please try again..\n");
							valid_input = false;
							sub_choice = -1;
							Sleep(1000);
						}
					}
				}
			}
		}
		else {
			valid_input = false;
			adminSearch = false;
			sub_choice = -1;
		}
	}
}

void deleteBookMenu(BooksHashTable* ht) {
	int sub_choice = -1;
	while (sub_choice != 0) {
		printf("\n-----Delete Book Menu-----\n");
		printf("Enter the ID of the book you want to delete: ");
		int id;
		scanf("%d", &id);
		printf("Enter the category of the book you want to delete:\n(1- Fiction, 2- Non-fiction, 3- Children, 4- Science, 5- Educational, 9-If you don't know the category)\n");
		int category;
		scanf("%d", &category);

		while (category != 9 && (category < 1 || category > 5)) {
			printf("Invalid category, please enter a number between 1 and 5.\n");
			printf("Enter the category of the book you want to delete (9 if not known): ");
			scanf("%d", &category);
		}

		Book* removedBook = NULL;
		if (category != 9) {
			removedBook = removeBookByIdCategory(ht, id, category - 1);
		}
		else {
			removedBook = removeBookById(ht, id);
		}

		if (removedBook != NULL) {
			char categoryStr[15];
			switch (removedBook->category) {
			case 0:
				strcpy(categoryStr, "Fiction");
				break;
			case 1:
				strcpy(categoryStr, "Non-fiction");
				break;
			case 2:
				strcpy(categoryStr, "Children");
				break;
			case 3:
				strcpy(categoryStr, "Science");
				break;
			case 4:
				strcpy(categoryStr, "Educational");
				break;
			}

			if (category != 9) {
				printf("\nBook with ID %d and category %s has been successfully removed.\n", id, categoryStr);
				logAction("Book removed successfuly.");
			}
			else {
				printf("\nBook with ID %d has been successfully removed.\n", id);
				logAction("Book removed successfuly.");
			}

			printf("\nDetails of removed book:\n\n");
			printf("\tID: %d\n", removedBook->id);
			printf("\tAuthor: %s\n", removedBook->authorName);
			printf("\tBook Name: %s\n", removedBook->bookName);
			printf("\tCategory: %s\n", categoryStr);
			printf("\tIs bestSeller: %d\n", removedBook->bestSeller);
			printf("\tPrice: %.2lf\n", removedBook->price);
			printf("\tQuantity: %d\n\n", removedBook->quantity);
		}
		else {
			if (category != 9) {
				printf("\nBook with ID %d and category %d not found.\n", id, category);
				logAction("Book not found.");
			}
			else {
				printf("\nBook with ID %d not found.\n", id);
				logAction("Book not found.");
			}
		}

		free(removedBook);
		printf("1. Delete another book\n");
		printf("0. Go back to main menu\n");
		printf("Enter your choice: ");
		scanf("%d", &sub_choice);
	}
}

void editBookMenu(BooksHashTable* ht) {
	int id;
	system("cls");
	printAllBooks(ht);
	printf("\n\nEnter the ID of the book you want to edit: ");
	scanf("%d", &id);

	Book* book = getBookById(ht, id);
	if (book == NULL) {
		printf("Book not found\n");
		logAction("Book not found.");
		return;
	}

	int choice = 0, newQuantity, newBestSeller;
	double newPrice;

	while (choice!=4) {
		printf("\nWhat property would you like to edit?\n");
		printf("1. Price\n");
		printf("2. Quantity\n");
		printf("3. Best Seller Status\n");
		printf("4. Exit\n");
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			printf("Enter the new price: ");
			scanf("%lf", &newPrice);
			book->price = newPrice;
			logAction("Book price changed successfuly.");
			break;
		case 2:
			printf("Enter the new quantity: ");
			scanf("%d", &newQuantity);
			book->quantity = newQuantity;
			logAction("Book quantity changed successfuly.");
			break;
		case 3:
			printf("Enter the new best seller status (0 for not a best seller, 1 for best seller): ");
			scanf("%d", &newBestSeller);
			book->bestSeller = newBestSeller;
			logAction("Book bestseller changed successfuly.");
			break;
		case 4:
			break;
		default:
			printf("Invalid choice\n");
			break;
		}
	}
}

void changeDiscountMenu(CustomerNode** customers) {
	printf("Enter the customer ID: ");
	int customerId;
	scanf("%d", &customerId);

	// Search for the customer in the customers list
	CustomerNode* current = findCustomerInList(*customers, customerId);
	if (current == NULL) {
		printf("Error: Customer not found.\n");
		logAction("Customer not found.");
		return;
	}

	printf("Current discount: %d%%\n", current->customer.discount);
	printf("Enter the new discount: ");
	int newDiscount;
	scanf("%d", &newDiscount);

	// Update the customer's discount
	current->customer.discount = newDiscount;
	printf("Discount updated successfully.\n");
	logAction("Customer discount updated.");
	Sleep(1500);
}

void createOrderMenu(CustomerNode** customers, BooksHashTable* ht, int employee_id) {
	system("cls");
	// Prompt the user for the customer's information
	int customer_id, count = 0;
	char customerName[30];
	bool customer_list_empty = false;
	bool success = false;
	printf("Enter the customer's ID: ");
	scanf("%d", &customer_id);

	if ((*customers) == NULL)
		customer_list_empty = true;

	CustomerNode* customer = NULL;
	if (!customer_list_empty) {
		// Check if the customer already exists in the linked list
		customer = findCustomerInList(*customers, customer_id);
	}
	
	if (!customer || customer_list_empty) {
		// If the customer does not exist, create a new customer
		printf("Customer with ID: %d doesn't exists. \nPlease follow the following prompts to create a new customer for ID: %d\n\n", customer_id, customer_id);
		//customer = (CustomerNode*)malloc(sizeof(CustomerNode));
		logAction("Customer doesn't exists.");
		printf("Enter the customer's name: ");
		scanf("%s", customerName);
		customer = createNewCustomer(customer_id, customerName, 0);
		logAction("Customer created.");
		// Add the new customer to the linked list in the right place related to the joinedDate
		//newer customer will be at head
		insertCustomerNode(customers, customer);
	}

	// Create a new order
	OrderListNode* new_order = (OrderListNode*)malloc(sizeof(OrderListNode));
	new_order->data.employee_id = employee_id; //add a functionality to auto get the employee_id
	new_order->data.customer_id = customer->customer.customer_id;
	new_order->data.order_id = customer->customer.countOfPurchases++;
	new_order->data.date = time(NULL);
	new_order->data.itemsList = NULL;
	new_order->data.totalPrice = 0.0;

	// Prompt the user to add items to the order
	char choice;
	do {
		system("cls");
		if (count >= 3) {
			printf("Maximum number of items for customer per order is reached.");
			logAction("Maximum items per order reached.");
			Sleep(2000);
			break;
		}
		printAllBooks(ht);
		printf("Enter the item ID: ");
		int itemId;
		scanf("%d", &itemId);
		printf("Enter the item quantity: ");
		int quantity;
		scanf("%d", &quantity);
		new_order->data.itemsList = insertOrderItem(ht, new_order->data.itemsList, itemId, quantity, &success, &count);
		if (!success) {
			//printf("\nItem doesn't exist or not in stock.\n");
			printf("\nDo you want to add different item or different quantity (y/n)?");
			scanf(" %c", &choice);
		}
		else {

			logAction("Item added succesfuly to the order.");
			
			printf("\nDo you want to add more items to the order (y/n)? ");
			scanf(" %c", &choice);
		}
	} while (choice == 'y');

	//update totalPrice of the order
	updateTotalPrice(&(new_order->data), customer->customer.discount);
	
	// Add the new order to the customer's purchases list
	new_order->next = customer->customer.purchases;
	customer->customer.purchases = new_order;
	system("cls");
	printf("\nOrder created successfuly!\n");
	logAction("Order created successfuly.");

	//print final order summary to customer
	printOrdersList(new_order, false);

	
	printf("\nPress any key to get back to main menu...");
	getch();
}

void addEmployeeMenu(EmployeeNode** employees) {
	system("cls");
	int i = 0;
	char c;
	char password[STR_LEN];
	Employee newEmployee;
	// Assign the nextID value to the new employee's id
	newEmployee.id = (next_emp_id)++;
	
	printf("Enter the employee's first name: ");
	scanf("%s", newEmployee.fname);
	
	printf("Enter the employee's last name: ");
	scanf("%s", newEmployee.lname);
	
	printf("Enter the employee's level (1-3): ");
	scanf("%d", &newEmployee.level);
	while (newEmployee.level < 1 || newEmployee.level > 3) {
		printf("Invalid level. Level must be between 1 and 3.\n");
		printf("Enter the employee's level: ");
		scanf("%d", &newEmployee.level);
	}
	
	printf("Enter the employee's username: ");
	scanf("%s", newEmployee.username);
	
	printf("Enter the employee's password: ");
	while ((c = getch()) != '\r') {
		if (i < STR_LEN - 1) {
			password[i++] = c;
			printf("*");
		}
	}
	password[i] = '\0';
	strcpy(newEmployee.password, password);

	addNewEmployee(employees, newEmployee);
	printf("\nEmployee added successfully!\n");
	logAction("Employee added successfuly.");
}

void searchBookMenu(BooksHashTable* ht, int* sub_choice) {
	bool main_menu_flag = false;
	char search_term[STR_LEN];
	BookNode* search_results = NULL;

	if (!ht || isBooksEmpty(ht)) {
		main_menu_flag = true;
		printf("Error: Books Store storage is empty, Please add books first.\n");
		printf("Redirecting to main menu..");
		logAction("No Books - back to main menu");
		Sleep(2000);
	}
	else {
		while (!main_menu_flag) {
			printf("\n-----Search Book Menu-----\n");
			printf("1. Search by ID\n");
			printf("2. Search by Title\n");
			printf("3. Search by Author\n");
			printf("4. Search by greater than Price\n");
			printf("5. Search by less than Price\n");
			printf("6. Search by Bestseller\n");
			printf("7. Search for newer books than Publish Date\n");
			printf("8. Search for older books than Publish Date\n");
			printf("9. Search for Books in certain category:\n");
			printf("10. Multiple search by Author and Title\n");
			printf("0. Go back to main menu\n");
			printf("Enter your choice: ");
			scanf("%d", sub_choice);

			switch (*sub_choice) {
			case 1: //search by id
				printf("Enter book's id to search: ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_ID);
				logAction("Searched book by ID");
				break;
			case 2: //search by name
				printf("Enter book's name to search: ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_NAME);
				logAction("Searched book by title");
				break;
			case 3: //search by author
				printf("Enter book's author to search: ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_AUTHOR);
				logAction("Searched book by author");
				break;
			case 4: // greater price
				printf("Enter book's price to search books with greater price: ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_PRICE_GREATER);
				logAction("Searched book by price greater than");
				break;
			case 5: // smaller price
				printf("Enter book's price to search books with smaller price: ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_PRICE_SMALLER);
				logAction("Searched book by price smaller than");
				break;
			case 6: //search by bestseller
				printf("Enter book's bestseller status to search (1-bestseller, 0-not): ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_BESTSELLER);
				logAction("Searched book by bestseller");
				break;
			case 7: //search by books newer than publish date
				printf("Enter book's publish year to search for newer books: ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_NEWER_THAN_PUBLISH);
				logAction("Searched book by publish year greater than");
				break;
			case 8: //search by books older than publish date
				printf("Enter book's publish year to search for older books: ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_OLDER_THAN_PUBLISH);
				logAction("Searched book by publish year smaller than");
				break;
			case 9: //search by category
				printf("Enter category to look for books from that category:\n ");
				printf("(1. Fiction 2. Non-Fiction 3. Children 4. Science 5. Educational): ");
				scanf("%s", search_term);
				search_results = searchBooks(ht, search_term, SEARCH_BY_CATEGORY);
				logAction("Searched book by category");
				break;
			case 10: //search by author and name combined
				printf("Enter book's author to search: ");
				scanf("%s", search_term);
				char title[STR_LEN];
				printf("Enter book's title to search: ");
				scanf("%s", title);
				search_results = searchBooksByAuthorAndTitle(ht, search_term, title);
				logAction("Searched book by author name and book's title");
				break;
			case 0:
				main_menu_flag = true;
				break;
			default:
				printf("\nInvalid choice. Try again.\n");
				searchBookMenu(ht, sub_choice);
			}
			printBooksList(search_results);
		}
	}
}

void addBookMenu(BooksHashTable* ht) {
	bool id_valid = true;
	int sub_choice = -1;
	int id;
	int c;
	while (sub_choice != 0) {
		printf("\n-----Add Book Menu-----\n");
		printf("Enter the following details to add a new book:\n");
		
		do {
			id_valid = true;
			printf("ID: ");
			
			scanf("%d", &id);
			if (getBookById(ht, id)) { //if not null there is already a book with that id
				printf("Id is invalid - there is existing book with that id. Please try again.. \n");
				logAction("Book id already exists.");
				id_valid = false;
			}
		} while (!id_valid);
		while ((c = getchar()) != '\n' && c != EOF) {} // consume the remaining newline 
		printf("Title: ");
		char title[STR_LEN];
		fgets(title, STR_LEN, stdin);
		// remove the newline character at the end of the string
		title[strcspn(title, "\n")] = '\0';
		printf("Category: ");
		int category;

		while (1) {
			printf("Category (1- Fiction, 2- Non-fiction, 3- Children, 4- Science, 5- Educational): ");
			scanf("%d", &category);
			while ((c = getchar()) != '\n' && c != EOF) {} // consume the remaining newline 
			if (category >= 1 && category <= 5) {
				break;
			}
			printf("Invalid category, please enter a number between 1 and 5.\n");
		}

		printf("Author: ");
		char author[STR_LEN];
		fgets(author, STR_LEN, stdin);
		// remove the newline character at the end of the string
		author[strcspn(author, "\n")] = '\0';
		printf("Price: ");
		double price;
		scanf("%lf", &price);
		printf("Quantity: ");
		int quantity;
		scanf("%d", &quantity);
		printf("Is this bestSeller: ");
		int bestSeller;
		scanf("%d", &bestSeller);
		char publishDateString[5];

		printf("Enter publish date of the book: ");
		scanf("%s", publishDateString);
		time_t publishDate = dateFromString(publishDateString);

		// Create a new book with the inputted details
		Book book = createBook(id, title, author, category-1, price, publishDate, quantity, bestSeller);

		// Add the new book to the books hash table
		addBook(&ht, book);
		logAction("Added new book successfuly.");
		
		
		//adding mock data of books to the books hashtable
		//addMockData(ht);

		printf("\nBook added successfully.\n");
		printf("1. Add another book\n");
		printf("0. Go back to main menu\n");
		printf("Enter your choice: ");
		scanf("%d", &sub_choice);
	}
}

void printMenuByLevel(BooksHashTable* ht, EmployeeNode** employees, CustomerNode** customers, int level, int* choice, int* employee_id) {
	int sub_choice = 0;
	system("cls");
	printf("\n-----Main Menu:-----\n");
	printf("1. Search for book\n");
	printf("2. Add new book\n");
	/*printf("3. Add new customer\n");*/
	printf("3. Create order for customer\n");
	if (level >= 2) {
		printf("4. Refund item from order\n");
		printf("5. Edit book\n");
		printf("6. Change discount level for customer\n");
	}
	if (level >= 3) {
		printf("7. Add new employee\n");
		printf("8. Delete employee\n");
		printf("9. Delete book\n");
	}
	printf("10. Delete customer from the book store\n");
	printf("11. View Customer's orders list\n");
	printf("12. Log out\n");
	printf("0. Log out and Exit the Application\n");
	printf("Enter your choice: ");
	scanf("%d", choice);

	switch (*choice) {
	case 1:
		system("cls");
		logAction("Searching for a book");
		searchBookMenu(ht, &sub_choice);
		break;
	case 2:
		logAction("Adding a new book");
		addBookMenu(ht);
		writeToBinaryFile(BOOKS_FILENAME, ht);
		logAction("Wrote the changes to the books file");
		break;
	case 3:
		logAction("Creating a new order");
		createOrderMenu(customers, ht, *employee_id);
		writeCustomersToBinaryFile(*customers, CUSTOMER_FILENAME);
		logAction("Wrote the changes to the customers file");
		writeToBinaryFile(BOOKS_FILENAME, ht);
		logAction("Wrote the changes to the books file");
		break;
	case 4:
		if (level >= 2) {
			logAction("Refunding an item");
			refundItemMenu(*customers, ht);
			writeToBinaryFile(BOOKS_FILENAME, ht);
			logAction("Wrote the changes to the books file");
			writeCustomersToBinaryFile(*customers, CUSTOMER_FILENAME);
			logAction("Wrote the changes to the customers file");
		}
		else {
			printf("\nAccess denied. You must be at least level 2 to access this option.\n");
		}
		break;
	case 5:
		if (level >= 2) {
			logAction("Editing a book details");
			editBookMenu(ht);
			writeToBinaryFile(BOOKS_FILENAME, ht);
			logAction("Wrote the changes to the books file");
		}
		else {
			printf("\nAccess denied. You must be at least level 2 to access this option.\n");
		}
		break;
	case 6:
		if (level >= 2) {
			logAction("Changing a discount level for a customer");
			changeDiscountMenu(customers);
			writeCustomersToBinaryFile(*customers, CUSTOMER_FILENAME);
			logAction("Wrote the changes to the customers file");
		}
		else {
			printf("\nAccess denied. You must be at least level 2 to access this option.\n");
		}
		break;
	case 7:
		if (level >= 3) {
			logAction("Adding a new employee.");
			addEmployeeMenu(employees);
			writeEmployeesToBinaryFile(employees, EMPLOYEE_FILENAME);
			logAction("Wrote the changes to the employees file");
		}
		else {
			printf("\nAccess denied. You must be an admin to access this option.\n");
		}
		break;
	case 8:
		if (level >= 3) {
			logAction("Removing an employee.");
			deleteEmployeeMenu(employees);
			writeEmployeesToBinaryFile(employees, EMPLOYEE_FILENAME);
			logAction("Wrote the changes to the employees file");
		}
		else {
			printf("\nAccess denied. You must be an admin to access this option.\n");
		}
		break;
	case 9:
		if (level >= 3) {
			logAction("Deleting a book.");
			deleteBookMenu(ht);
			writeToBinaryFile(BOOKS_FILENAME, ht);
			logAction("Wrote the changes to the employees file");
		}
		else {
			printf("\nAccess denied. You must be an admin to access this option.\n");
		}
		break;
	case 10:
		logAction("Deleting a Customer");
		deleteCustomerMenu(customers);
		writeCustomersToBinaryFile(*customers, CUSTOMER_FILENAME);
		logAction("Wrote the changes to the customers file");
		break;
	case 11:
		viewCustomerOrdersMenu(*customers);
		logAction("Printing customers orders");
		break;
	case 12:
		printf("\nLogging out...\n");
		break;
	case 0:
		printf("\nLogging out and exits...\n");
		break;
	default:
		printf("\nInvalid choice. Try again.\n");
	}
}

