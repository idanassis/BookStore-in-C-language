// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include "Employee.h"
#include "Crypto.h"
#include "Menu.h"
#include "BooksHashTable.h"

#define STRING_LENGTH 30
#define EMPLOYEE_FILENAME "employee.bin"
#define BOOKS_FILENAME "books.bin"
#define CUSTOMER_FILENAME "customers.bin"

extern bool is_logged_in = false;
extern int employeeLevel = 0;

//get inputs from user
void readUserPass(char* username, char* password) {
    int i = 0;
    char c;

    printf("Please enter username and pass\n");

    printf("username: ");
    scanf("%s", username);

    printf("Enter password: ");

    while ((c = getch()) != '\r') {
        if (c == 8 && i > 0) {  // check for backspace key
            i--;  // decrement index variable
            printf("\b \b");  // move cursor back and overwrite the * with a * again
        }
        else if (i < STRING_LENGTH - 1) {
            password[i++] = c;
            printf("*");
        }
    }
    password[i] = '\0';
}

//authenticate the users inputs
int authenticateUser(EmployeeNode* employeeList, char* username, char* password, int* newLevel, int* employee_id) {
    bool isAuth = false;
    Employee* emp = getEmployeeByUsername(employeeList, username);
    if (emp != NULL) {
        /*decryptEmployee(emp);
        printf("EMPLOYEE:\n");
        printf("  ID=%d\n  USER=%s\n  PASS=%s\n", emp->id, emp->username, emp->password);
        encryptEmployee(emp);*/
        isAuth = authEmployee(emp, username, password);
        
        if (isAuth) {
            decryptEmployee(emp);
            *newLevel = emp->level;
            *employee_id = emp->id;
            encryptEmployee(emp);
            logAction("User logged in");
            return 1;
        }
        else {
            printf("\nPassword is incorrect.\n"); //need to add an option to try 3 times, until reload the function.
            logAction("Password invalid.");
            //Sleep(1000);
            return 0;
        }
    }
    else {
        printf("\nThere is no such user in our database.\n\n");
        char msg[50];
        sprintf(msg, "Username {%s} not found.", username);
        logAction(msg);
        //Sleep(1000);
        return 0;
    }
}

void main() {
    int shouldExit = 0;
    int choice = 0;
    int authFailedCount = 0;
    int employee_id = 0;
    char* log="";

    last_id_used = 0;
    next_emp_id = 2;
    BooksHashTable* booksList = readFromBinaryFile(BOOKS_FILENAME);
    logAction("Read books.bin");
    EmployeeNode* employeeList = readEmployeesFromBinaryFile(EMPLOYEE_FILENAME);
    logAction("Read employee.bin");
    CustomerNode* customerList = readCustomersFromBinaryFile(CUSTOMER_FILENAME);
    logAction("Read customers.bin");
    char username[STRING_LENGTH];
    char password[STRING_LENGTH];

    do {
        if (authFailedCount >= 3) {
            printf("\nMaximum authentication tries reached. Please try again later..\n");
            sprintf(log, "Failed to authenticate user. count: %d of 3", authFailedCount);
            logAction(log);
            Sleep(1500);
            return;
        }
        else {
            if (!is_logged_in) {

                system("cls");
                printf("\n--- Hello Guest! ---\n\n");
                printf("1. Login\n");
                printf("2. Exit..\n\n");

                printf("Enter your choice: ");
                scanf("%d", &shouldExit);

                if (shouldExit == 1) {
                    system("cls");
                    readUserPass(username, password);
                    logAction("Read username and password");
                    is_logged_in = authenticateUser(employeeList, username, password, &employeeLevel, &employee_id);
                    if (is_logged_in) {
                        logAction("Login successful");
                    }
                    if (!is_logged_in) {
                        authFailedCount++;
                        strcpy(password, "");
                        printf("Wrong username or password - count: %d out of 3", authFailedCount);
                        Sleep(3000);
                    }

                }
                else {
                    return;
                }
            }
            else {
                choice = 0;
                printMenuByLevel(booksList, &employeeList, &customerList, employeeLevel, &choice, &employee_id);
                if (choice == 12) {
                    is_logged_in = false;
                    employeeLevel = 0;
                    logAction("Logging out ...");
                }
                if (choice == 0) {
                    is_logged_in = false;
                    employeeLevel = 0;
                    logAction("Logging out and exit ..");
                    break;
                }
            }
        }
    } while (1);
}