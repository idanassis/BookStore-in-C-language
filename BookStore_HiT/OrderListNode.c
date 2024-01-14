// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "OrderListNode.h"

//find order in list
Order* findOrderInList(OrderListNode* head, int Order_id) {
    OrderListNode* current = head;
    while (current != NULL) {
        if (current->data.order_id == Order_id) {
            logAction("Order was found in the list");
            return &(current->data);
        }
        current = current->next;
    }
    logAction("Order wasn't found in the list");
    return NULL;
}

// print orders list - printList=true means to print all the orders in list, false means print only the head
void printOrdersList(OrderListNode* orderList, bool printList) {
    OrderListNode* cur = orderList;
    do {
        struct tm* timeInfo = localtime(&(cur->data.date));
        char timeString[20];
        printf("\n--------------------------------------------");
        strftime(timeString, 20, "%Y-%m-%d %H:%M:%S", timeInfo);
        printf("\n\nDate: %s\t\tOrder ID: %d\n\n\tCustomer ID: %d\n\tEmployee ID: %d\n",
            timeString,
            cur->data.order_id,
            cur->data.customer_id, 
            cur->data.employee_id);
        printf("\nItems in Order:\n");
        printOrderItemsList(cur->data.itemsList);
        printf("\nTotal Price: %.2lf\n", cur->data.totalPrice);
        cur = cur->next;
    } while (cur != NULL && printList);
}