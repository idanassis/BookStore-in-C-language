// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#pragma once
#include <stdio.h>
#include "OrderItemNode.h"

// A Node that holds data of type Order (OrdersList)
typedef struct order_list {
	Order data;
	struct order_list* next;
}OrderListNode;

//find order in list
Order* findOrderInList(OrderListNode* head, int Order_id);

// print orders list - printList=true means to print all the orders in list, false means print only the head
void printOrdersList(OrderListNode*, bool);
