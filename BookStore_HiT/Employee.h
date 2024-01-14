// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#pragma once
#include "Customer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct employee{
	int id;
	char fname[20];
	char lname[20];
	char username[10];
	char password[32];
	int level;
}Employee;

typedef struct employeeNode {
	Employee emp;
	struct employeeNode* next;
}EmployeeNode;

extern int next_emp_id;

//returns 1 if the employee passed the authentication successfuly and 0 if not
int authEmployee(Employee* employee, char* username, char* password);

// adding a new employee to the employees list
void addNewEmployee(EmployeeNode** head, Employee newEmp);

// get employee pointer by username, get null if not found
Employee* getEmployeeByUsername(EmployeeNode* head, char* username);

// writing customers data to bin file
void writeEmployeesToBinaryFile(EmployeeNode** head, char* filename);

// reading customers data to bin file
EmployeeNode* readEmployeesFromBinaryFile(char* filename);

// insert employee to the employees list
EmployeeNode* insertEmployeeAtHead(EmployeeNode* head, Employee employee);

// remove employee from list by id
Employee* removeEmployeeFromListById(EmployeeNode** employees, int employee_id);

// find employee in list by id
Employee* findEmployeeInList(EmployeeNode* employees, int employee_id);

// free employees list 
void freeEmployeesList(EmployeeNode* head);

//search employees by id, fname, lname, username
EmployeeNode* searchEmployees(EmployeeNode* employees, char* searchTerm, int searchField);

//printing a list of employees
void printEmployees(EmployeeNode* head);

//reversing employees list
EmployeeNode* reverseList(EmployeeNode* head);