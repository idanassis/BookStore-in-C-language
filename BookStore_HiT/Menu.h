// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#pragma once
#include "BooksHashTable.h"
#include "Employee.h"

void viewCustomerOrdersMenu(CustomerNode* customerList);
void deleteCustomerMenu(CustomerNode** customers);
void refundItemMenu(CustomerNode* head, BooksHashTable* ht);
void deleteEmployeeMenu(Employee** employees);
void deleteBookMenu(BooksHashTable* ht);
void editBookMenu(BooksHashTable* ht);
void changeDiscountMenu(CustomerNode** customers);
void createOrderMenu(CustomerNode** customers, BooksHashTable* ht, int employee_id);
void addEmployeeMenu(EmployeeNode** employees);
void searchBookMenu(BooksHashTable* ht, int* sub_choice);
void addBookMenu(BooksHashTable* ht);
void printMenuByLevel(BooksHashTable* ht, EmployeeNode** employees, CustomerNode** customers, int level, int* choice, int* employee_id);