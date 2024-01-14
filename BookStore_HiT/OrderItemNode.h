// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#pragma once
#include <stdio.h>
#include <time.h>
#include "BooksHashTable.h"

// A structure of each item in the order
typedef struct order_item {
	int id;
	int count;
	double price;
}OrderItem;

// A Node that holds data of type OrderItem (OrderItemsList)
typedef struct order_items_node {
	OrderItem item;
	struct order_items_node* next;
}OrderItemNode;

// A structure of an Order
typedef struct order {
	int employee_id;
	int customer_id;
	int order_id;
	time_t date;
	OrderItemNode* itemsList;
	double totalPrice;
}Order;

// making a complete item from a given id , take in consider if the book is not found and if the quantity is 0 and no books left to purchase
OrderItem* createOrderItemById(BooksHashTable* ht, int itemId, int quantity);

// adding more items to the items list
OrderItemNode* insertOrderItem(BooksHashTable* ht, OrderItemNode* head, int itemId, int quantity, bool* success, int* count);

// find item in list
OrderItem* findItemInList(OrderItemNode* head, int itemId);

//print items list
void printOrderItemsList(OrderItemNode* head);

//update the total price of the order
double updateTotalPrice(Order* order, int discount);

//count how many items in items list
int countItems(OrderItemNode* itemsList);