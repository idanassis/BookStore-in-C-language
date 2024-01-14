// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#pragma once
#include "Book.h"
#include <stdbool.h>

#define BOOKS_CATEGORIES 5

extern int last_id_used;

typedef struct booksHashTable {
    struct bookNode* categories[BOOKS_CATEGORIES];
} BooksHashTable;

BooksHashTable* readFromBinaryFile(const char* filename);

bool isBooksEmpty(BooksHashTable* ht);

void writeToBinaryFile(const char* filename, BooksHashTable* ht);

void addBook(BooksHashTable** ht, Book book);

Book* removeBookById(BooksHashTable* ht, int id);
Book* removeBookByIdCategory(BooksHashTable* ht, int id, int category);

Book* getBookById(BooksHashTable* ht, int id);
Book* getBookByIdCategory(BooksHashTable* ht, int id, int category);

void decreaseBookQuantity(BooksHashTable* ht, int id, int category, int quantity);
void increaseBookQuantity(BooksHashTable* ht, int id, int category, int quantity);

void printCategory(BooksHashTable* ht, int category);

// search books by book id/book name/author name/price etc.

void addBookToList(BookNode** head, Book book);

void printBooksList(BookNode* booksList);

BookNode* searchBooks(BooksHashTable * ht, char* searchTerm, int searchField);

BookNode* getBooksInBudget(BooksHashTable* ht, int category, double budget);

// edit book's details
void editBook(BooksHashTable* ht, int id, char* bookName, char* authorName, int category, double price, int isUsed, int quantity);

void addMockData(BooksHashTable* ht);

void printAllBooks(BooksHashTable* ht);

time_t dateFromString(char* dateString);

// get string format from date variable
char* stringFromDate(time_t date);

BookNode* searchBooksByAuthorAndTitle(BooksHashTable* ht, char* author, char* title);