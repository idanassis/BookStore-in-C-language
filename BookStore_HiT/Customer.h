// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#pragma once
#include <stdio.h>
#include "OrderListNode.h"
#include "OrderItemNode.h"

typedef struct customer {
	int customer_id;
	char name[30];
	time_t joinedDate;
	int discount;
	int countOfPurchases;
	OrderListNode* purchases;
}Customer;

typedef struct customerNode {
	Customer customer;
	struct customerNode* next;
}CustomerNode;

//freeing customers orders and items 
void freeCustomerData(Customer* customer);

// create a new node with customer data
CustomerNode* createNewCustomer(int customer_id, char* name, int discount);

// for refund the order must have been taking place at max 14 days ago
// updating purchases and quantity of item
void refundItems(int customer_id, int order_id, OrderItemNode* itemsToRemove, CustomerNode* head, BooksHashTable* ht);

// get list of customers and get the specific customer purchases list
OrderListNode* getPurchasesByCustomer(CustomerNode* customerList, int customer_id);

// writing customers data to bin file
void writeCustomersToBinaryFile(CustomerNode* customer, char* filename);

// reading customers data to bin file
CustomerNode* readCustomersFromBinaryFile(char* filename);

//remove customer from list by id
Customer* removeCustomerFromListById(CustomerNode** customers, int customer_id);

//find customer in list by id
CustomerNode* findCustomerInList(CustomerNode*, int customer_id);

//print customers list
void printCustomersList(CustomerNode*);
 
//creates customer's mock data
CustomerNode* createMockData();

//insert customer to list
void insertCustomerNode(CustomerNode** head, CustomerNode* newNode);


