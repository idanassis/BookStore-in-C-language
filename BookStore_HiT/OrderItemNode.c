// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "OrderItemNode.h"
#include "BooksHashTable.h"
#include "LogFunction.h"

// making a complete item from a given id , take in consider if the book is not found and if the quantity is 0 and no books left to purchase
OrderItem* createOrderItemById(BooksHashTable* ht, int itemId, int quantity) {
    OrderItem* item = malloc(sizeof(OrderItem));

    // Look up the price of the item by ID
    Book* book = getBookById(ht, itemId);
    if (book == NULL || book->quantity < quantity) {
        // Book not found, use a default price
        logAction("Item is not available");
        free(item);
        return NULL;
    }
    if (book->quantity >= quantity) {
        (*item).id = itemId;
        (*item).count = quantity;
        (*item).price = (book->price) * quantity;

        book->quantity -= quantity;
        logAction("Order Item created successfully");
        return item;
    }
    free(item);
    return NULL;
}

// find item in list
OrderItem* findItemInList(OrderItemNode* head, int itemId) {

    OrderItemNode* current = head;
    while (current != NULL) {
        if (current->item.id == itemId) {
            //Found the item
            logAction("Item was found in the list");
            return &(current->item);
        }
        current = current->next;
    }
    logAction("Item wasn't found in the list");
    return NULL;
}

//print list of items
void printOrderItemsList(OrderItemNode* head) {
    OrderItemNode* current = head;
    while (current != NULL) {
        printf("\tItem ID: %d\tQuantity: %d\tPrice: %.2f\n", current->item.id, current->item.count, current->item.price);
        current = current->next;
    }
    logAction("Printed items list.");
}

//insert order item - checks if the item exists in the items list, if yes only update quantity, if not, adding it to the list.
OrderItemNode* insertOrderItem(BooksHashTable* ht, OrderItemNode* head, int itemId, int quantity, bool* success, int* count) {
    OrderItem* item = createOrderItemById(ht, itemId, quantity);

    if (!item) {
        logAction("Item not exists.");
        printf("Item not exists in our store.");
        *success = false;
        return head;
    }

    *success = true;

    OrderItemNode* newNode = (OrderItemNode*)malloc(sizeof(OrderItemNode));
    newNode->item = *item;
    newNode->next = NULL;

    //// Check if the head node has the same ID as the new node's item
    //if (head->item.id == newNode->item.id) {
    //    head->item.count += quantity;
    //    free(item);
    //    free(newNode);
    //    logAction("Item quantity updated.");
    //    return head;
    //}

    if (head == NULL) {
        // The list is empty, so the new node is the first and only node in the list
        head = newNode;
        (*count)++;
    }
    else {
        // Find the correct position in the list to insert the new node
        OrderItemNode* current = head;
        OrderItemNode* prev = NULL;
        while (current != NULL) {
            if (current->item.id == newNode->item.id) {
                current->item.count += quantity;
                free(item);
                free(newNode);
                logAction("Item quantity updated.");
                return head;
            }
            prev = current;
            current = current->next;
        }
        prev->next = newNode;
        (*count)++;
    }
    free(item);
    logAction("New item added to the items list");
    return head;
}

//we have changed some functions so I am not sure what type to return ( it depends what functions are calling this one)
double updateTotalPrice(Order* order, int discount) {
    double totalPrice = 0;
    OrderItemNode* current = order->itemsList;
    while (current != NULL) {
        totalPrice += current->item.price;
        current = current->next;
    }

    totalPrice = totalPrice * (1 - (discount / 100));
    order->totalPrice = totalPrice;

    logAction("The total price of the order was changed.");
    return totalPrice;
}

//count how many items in itemslist
int countItems(OrderItemNode* itemsList) {
    OrderItemNode* cur = NULL;
    int count = 0;
    if (!itemsList) {
        return 0;
    }
    else {
        cur = itemsList;
        while (cur != NULL) {
            count++;
            cur = cur->next;
        }
        return count;
    }
}