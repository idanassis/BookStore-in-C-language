// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include "Employee.h"
#include "Crypto.h"
#include "LogFunction.h"

#define SEARCH_BY_ID 0
#define SEARCH_BY_FNAME 1
#define SEARCH_BY_LNAME 2
#define SEARCH_BY_USERNAME 3

int next_emp_id;

//writing to binary file
void writeEmployeesToBinaryFile(EmployeeNode** head, char* filename) {
	if (!(*head)) {

		// No employees, create default admin employee
		Employee defaultEmp;
		defaultEmp.id = 1;
		strcpy(defaultEmp.username, "admin");
		strcpy(defaultEmp.password, "admin");
		strcpy(defaultEmp.fname, "");
		strcpy(defaultEmp.lname, "");
		defaultEmp.level = 3;

		addNewEmployee(&head, defaultEmp);

		FILE* fp = fopen(filename, "wb");

		// Write the next employee id variable to the file
		next_emp_id = 2;
		fwrite(&next_emp_id, sizeof(int), 1, fp);

		//write the admin employee details to the bin file
		fwrite(&defaultEmp, sizeof(Employee), 1, fp);
		logAction("Created a new Employee binary file.");

		*head = (EmployeeNode*)malloc(sizeof(EmployeeNode));

		(*head)->emp = defaultEmp;
		(*head)->next = NULL;

		fclose(fp);
		return;
	}
	FILE* fp = fopen(filename, "wb");

	// Write the next employee id variable to the file
	fwrite(&next_emp_id, sizeof(int), 1, fp);

	EmployeeNode* current = *head;
	while (current) {
		fwrite(&current->emp, sizeof(Employee), 1, fp);
		current = current->next;
	}
	fclose(fp);
}

//read employees from bin file
EmployeeNode* readEmployeesFromBinaryFile(const char* filename) {
	EmployeeNode* head = NULL;
	EmployeeNode* tail = NULL;
	FILE* file = fopen(filename, "rb");
	if (!file) {
		
		// File doesn't exist, create it with default employee
		Employee defaultEmp;
		defaultEmp.id = 1;
		strcpy(defaultEmp.username, "admin");
		strcpy(defaultEmp.password, "admin");
		strcpy(defaultEmp.fname, "System_");
		strcpy(defaultEmp.lname, "Manager");
		defaultEmp.level = 3;

		addNewEmployee(&head, defaultEmp);

		writeEmployeesToBinaryFile(&head, filename);
		logAction("Created a new Employee binary file.");
		return head;
	}

	// Read the nextID variable from the file
	fread(&next_emp_id, sizeof(int), 1, file);

	Employee emp;
	while (fread(&emp, sizeof(Employee), 1, file) == 1) {
		EmployeeNode* newNode = malloc(sizeof(EmployeeNode));
		newNode->emp = emp;
		newNode->next = NULL;
		if (!head) {
			head = newNode;
			tail = newNode;
		}
		else {
			tail->next = newNode;
			tail = newNode;
		}
	}
	fclose(file);
	return head;
}

//get employee by username
Employee* getEmployeeByUsername(EmployeeNode* head, char* username) {
	EmployeeNode* current = head;
	while (current != NULL) {
		decryptEmployee(&(current->emp));
		if (strcmp(current->emp.username, username) == 0) {
			encryptEmployee(&(current->emp));
			return &(current->emp);
		}
		encryptEmployee(&(current->emp));
		current = current->next;
	}
	return NULL;
}

//returns 1 if the employee passed the authentication successfuly and 0 if not
int authEmployee(Employee* employee, char* username, char* password) {
	decryptEmployee(employee);
	char* storedUsername = employee->username;
	char* storedPass = employee->password;
	int result = 0;
	if (strcmp(storedUsername, username) != 0)
		result = 0;
	else {
		if (strcmp(storedPass, password) != 0)
			result = 0;
		else {
			result = 1; // username and password match
		}
	}
	encryptEmployee(employee);
	return result;
}

// adding a new employee to the employees list
void addNewEmployee(EmployeeNode** head, Employee newEmp) {

	encryptEmployee(&(newEmp));
	//create an empty node if the head is null
	if (*head == NULL) {
		*head = (EmployeeNode*)malloc(sizeof(EmployeeNode));
		(*head)->emp = newEmp;
		(*head)->next = NULL;
		return;
	}

	// Create a new employee node
	EmployeeNode* newNode = (EmployeeNode*)malloc(sizeof(EmployeeNode));
	newNode->emp = newEmp;
	newNode->next = NULL;

	// Add the new node to the linked list of employees in the front of the list
	newNode->next = *head;
	*head = newNode;
}

// insert employee to the employees list
EmployeeNode* insertEmployeeAtHead(EmployeeNode* head, Employee employee) {
	// Encrypt the employee data before inserting it into the list
	encryptEmployee(&employee);

	// Create a new employee node
	EmployeeNode* newNode = malloc(sizeof(EmployeeNode));
	newNode->emp = employee;
	newNode->next = head;

	return newNode;
}

// remove employee from list by id
Employee* removeEmployeeFromListById(EmployeeNode** employees, int employee_id) {
	
	Employee* temp = malloc(sizeof(Employee));
	EmployeeNode* cur = *employees;
	EmployeeNode* previous = NULL;

	while (cur != NULL) {
		decryptEmployee(&(cur->emp));
		if (cur->emp.id == employee_id) {
			*temp = cur->emp;
			if (!previous) {
				*employees = cur->next;
			}
			else {
				previous->next = cur->next;
				cur = previous;
			}
			encryptEmployee(temp);
			free(cur);
			return temp;
		}
		encryptEmployee(&(cur->emp));
		previous = cur;
		cur = cur->next;
	}
	free(temp);
	return NULL;
}

// find employee in list by id
Employee* findEmployeeInList(EmployeeNode* employees, int employee_id) {
	EmployeeNode* cur = employees;
	Employee* emp = malloc(sizeof(Employee));

	while (cur != NULL) {
		decryptEmployee(&(cur->emp));
		if (cur->emp.id == employee_id) {
			*emp = cur->emp;
			encryptEmployee(&(cur->emp));
			return emp;
		}
		encryptEmployee(&(cur->emp));
		cur = cur->next;
	}
	free(emp);
	return NULL;
}

// free employees list 
void freeEmployeesList(EmployeeNode* head) {
	EmployeeNode* cur = head;
	EmployeeNode* next;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}
}

//reverse list of employees
EmployeeNode* reverseList(EmployeeNode* head) {
	EmployeeNode* prev = NULL;
	EmployeeNode* current = head;
	EmployeeNode* next;
	while (current != NULL) {
		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}
	return prev;
}

//search employees
EmployeeNode* searchEmployees(EmployeeNode* employees, char* searchTerm, int searchField) {
	int searchPrice;

	// list of employees to return
	EmployeeNode* head = (EmployeeNode*)malloc(sizeof(EmployeeNode));
	head = NULL;

	// pointer that goes through the employees and find matching to the search term
	EmployeeNode* current = employees;

	while (current != NULL) {
		decryptEmployee(&(current->emp));
		switch (searchField) {
		case SEARCH_BY_ID:
			if (current->emp.id == atoi(searchTerm)) {
				head = insertEmployeeAtHead(head, current->emp);
			}
			break;
		case SEARCH_BY_FNAME:
			if (strstr(current->emp.fname, searchTerm) != NULL) {
				head = insertEmployeeAtHead(head, current->emp);
			}
			break;
		case SEARCH_BY_LNAME:
			if (strstr(current->emp.lname, searchTerm) != NULL) {
				head = insertEmployeeAtHead(head, current->emp);
			}
			break;
		case SEARCH_BY_USERNAME:
			if (strstr(current->emp.username, searchTerm) != NULL) {
				head = insertEmployeeAtHead(head, current->emp);
			}
			break;
		}
		encryptEmployee(&(current->emp));
		current = current->next;
	}
	return reverseList(head);
	//return head;
}

//print employees list
void printEmployees(EmployeeNode* head) {
	if (head == NULL) {
		printf("No Employees found.\n");
		return;
	}
	printf("ID\t\tName\t\t\t\t\tUsername\t\t\tLevel\n");
	printf("------------------------------------------------------------\n");
	while (head != NULL) {
		decryptEmployee(&(head->emp));
		printf("%d\t\t%s %s\t\t\t%s\t\t\t%d\n", 
			head->emp.id, 
			head->emp.fname, 
			head->emp.lname, 
			head->emp.username, 
			head->emp.level);
		encryptEmployee(&(head->emp));
		
		head = head->next;
	}
}
