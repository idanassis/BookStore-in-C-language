// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Book.h"
#include "BooksHashTable.h"
#include <stdbool.h>
#include <time.h>

#define SEARCH_BY_ID 0
#define SEARCH_BY_NAME 1
#define SEARCH_BY_AUTHOR 2
#define SEARCH_BY_PRICE_SMALLER 3
#define SEARCH_BY_PRICE_GREATER 4
#define SEARCH_BY_BESTSELLER 5
#define SEARCH_BY_NEWER_THAN_PUBLISH 6
#define SEARCH_BY_OLDER_THAN_PUBLISH 7
#define SEARCH_BY_CATEGORY 8
#define STRING_SIZE_MAX 30

int last_id_used;

// get date variable from string format
time_t dateFromString(const char* dateString) {
    int year;
    sscanf(dateString, "%d", &year);
    struct tm date = { 0 };
    date.tm_year = year - 1900;
    return mktime(&date);
}

// get string format from date variable
char* stringFromDate(time_t date) {
    struct tm* tm = gmtime(&date);
    int year = (tm->tm_year < 0) ? (1900 - abs(tm->tm_year)) : (tm->tm_year + 1900);
    char* str = (char*)malloc(sizeof(char) * 5);
    sprintf(str, "%d", year);
    return str;
}

//creates and returns an empty hashtable
BooksHashTable* createBooksHashTable() {
    BooksHashTable* ht = malloc(sizeof(BooksHashTable));
    for (int i = 0; i < BOOKS_CATEGORIES; i++) {
        ht->categories[i] = (BookNode*)malloc(sizeof(BookNode*));
        ht->categories[i] = NULL;
    }
    return ht;
}

//adds a book to the right book category list, adds the book to the right place - the list is sorted by books id
void addBook(BooksHashTable** ht, Book book) {
    
    //create an empty hashtable if it's null
    if (*ht == NULL) {
        *ht = createBooksHashTable();
    }
    
    int category = book.category;

    // Create a new book node
    BookNode* newNode = malloc(sizeof(BookNode));
    newNode->book = book;
    newNode->next = NULL;

    // Add the new node to the linked list for the book's category in the right place
    BookNode* cur = (*ht)->categories[category];
    if (cur == NULL || book.id < cur->book.id) {
        // Linked list is empty or new book has a smaller id, make the new node the head
        newNode->next = (*ht)->categories[category];
        (*ht)->categories[category] = newNode;
    }
    else {
        // Find the correct position in the linked list and add the new node there
        while (cur->next != NULL && book.id >= cur->next->book.id) {
            cur = cur->next;
        }
        newNode->next = cur->next;
        cur->next = newNode;
    }
}

//remove book from the books list - the book will not be available anymore
Book* removeBookById(BooksHashTable* ht, int id) {
    for (int i = 0; i < BOOKS_CATEGORIES; i++) {
        BookNode* cur = ht->categories[i];
        BookNode* prev = NULL;
        while (cur != NULL) {
            if (cur->book.id == id) {
                Book* removedBook = malloc(sizeof(Book));
                *removedBook = cur->book;
                if (prev == NULL) {
                    // Removing the head of the list
                    ht->categories[i] = cur->next;
                }
                else {
                    // Removing a node from the middle or end of the list
                    prev->next = cur->next;
                }
                free(cur);
                return removedBook;
            }
            prev = cur;
            cur = cur->next;
        }
    }
    return NULL;
}

//remove book from the books list - the book will not be available anymore - more efficient
Book* removeBookByIdCategory(BooksHashTable* ht, int id, int category) {
    BookNode* cur = ht->categories[category];
    BookNode* prev = NULL;
    while (cur != NULL) {
        if (cur->book.id == id) {
            Book* removedBook = malloc(sizeof(Book));
            *removedBook = cur->book;
            if (prev == NULL) {
                // Removing the head of the list
                ht->categories[category] = cur->next;
            }
            else {
                // Removing a node from the middle or end of the list
                prev->next = cur->next;
            }
            free(cur);
            return removedBook;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

//get book data by id
Book* getBookById(BooksHashTable* ht, int id) {
    for (int i = 0; i < BOOKS_CATEGORIES; i++) {
        BookNode* cur = ht->categories[i];
        while (cur != NULL) {
            if (cur->book.id == id) {
                Book* foundBook = &(cur->book);
                return foundBook;
            }
            cur = cur->next;
        }
    }
    return NULL;
}

//get book data by id and category - more efficient
Book* getBookByIdCategory(BooksHashTable* ht, int id, int category) {
    BookNode* cur = ht->categories[category];
    while (cur != NULL) {
        if (cur->book.id == id) {
            Book* foundBook = malloc(sizeof(Book));
            *foundBook = cur->book;
            return foundBook;
        }
        cur = cur->next;
    }
    return NULL;
}

//prints all the books data from specific category
void printCategory(BooksHashTable* ht, int category) {
    char categoryName[STRING_SIZE_MAX];
    printf("ID\tName\t\t\t\t\t\tAuthor\t\t\tCategory\tBestseller\tPrice  Q   Publish\n");
    BookNode* cur = ht->categories[category];
    while (cur != NULL) {
        switch (cur->book.category) {
        case 0:
            strcpy(categoryName, "Fiction");
            break;
        case 1:
            strcpy(categoryName, "Non - fiction");
            break;
        case 2:
            strcpy(categoryName, "Children");
            break;
        case 3:
            strcpy(categoryName, "Science");
            break;
        case 4:
            strcpy(categoryName, "Educational");
            break;
        }
        char* date = stringFromDate(cur->book.publishDate);
        printf("%d\t%-40s\t%-20s\t%-10s\t%-8d\t%.2lf\t%d\t%s\n"
            , cur->book.id
            , cur->book.bookName
            , cur->book.authorName
            , categoryName
            , cur->book.bestSeller
            , cur->book.price
            , cur->book.quantity
            , date);
        free(date);
        cur = cur->next;
    }
}

void decreaseBookQuantity(BooksHashTable* ht, int id, int category, int quantity) {
    BookNode* cur = ht->categories[category];
    while (cur != NULL) {
        if (cur->book.id == id) {
            if (cur->book.quantity == 0) {
                printf("Error: Cannot complete purchase. Book is out of stock.\n");
            }
            else if (quantity > cur->book.quantity) {
                printf("Error: Cannot complete purchase. Requested quantity is greater than available quantity.\n");
            }
            else {
                cur->book.quantity -= quantity;
            }
            break;
        }
        cur = cur->next;
    }
}

void increaseBookQuantity(BooksHashTable* ht, int id, int category, int quantity) {
    BookNode* cur = ht->categories[category];
    while (cur != NULL) {
        if (cur->book.id == id) {
            cur->book.quantity += quantity;
            break;
        }
        cur = cur->next;
    }
}

BooksHashTable* readFromBinaryFile(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        BooksHashTable* booksList = createBooksHashTable();
        return booksList;
    }

    //read last_index global variable 
    fread(&last_id_used, sizeof(int), 1, fp);

    //creates a new hashtable
    BooksHashTable* ht = createBooksHashTable();

    Book book;
    while (fread(&book, sizeof(Book), 1, fp) == 1) {
        addBook(&ht, book);
    }

    fclose(fp);
    return ht;
}

bool isBooksEmpty(BooksHashTable* ht)
{
    for (int i = 0; i < BOOKS_CATEGORIES; i++) {
        if (ht->categories[i])
            return false;
    }
    return true;
}

void writeToBinaryFile(const char* filename, BooksHashTable* ht) {
    FILE* fp = fopen(filename, "wb"); // Opens the file in write binary mode, creating it if it does not exist
    if (fp == NULL) {
        return;
    }

    // Write the last_id_used variable to the start of the file
    fwrite(&last_id_used, sizeof(int), 1, fp);

    //write the books
    for (int i = 0; i < BOOKS_CATEGORIES; i++) {
        BookNode* cur = ht->categories[i];
        while (cur != NULL) {
            fwrite(&(cur->book), sizeof(Book), 1, fp);
            cur = cur->next;
        }
    }

    fclose(fp);
}

void addBookToList(BookNode** head, Book book) {
    BookNode* newNode = (BookNode*)malloc(sizeof(BookNode));
    newNode->book = book;
    newNode->next = *head;
    *head = newNode;
}

BookNode* searchBooks(BooksHashTable* ht, char* searchTerm, int searchField) {
    int searchPrice, category;
    time_t current_time, compare_time;
    struct tm* timeInfo;
    BookNode* head = NULL;
    if (searchField == SEARCH_BY_CATEGORY) {
        category = atoi(searchTerm) - 1;
        if (category >= 0 && category <= 4) {
            return ht->categories[category];
        }
        else {
            return NULL;
        }
    }
    else {
        for (int i = 0; i < BOOKS_CATEGORIES; i++) {
            BookNode* current = ht->categories[i];
            while (current != NULL) {
                switch (searchField) {
                case SEARCH_BY_ID:
                    if (current->book.id == atoi(searchTerm)) {
                        addBookToList(&head, current->book);
                    }
                    break;
                case SEARCH_BY_NAME:
                    if (strstr(current->book.bookName, searchTerm) != NULL) {
                        addBookToList(&head, current->book);
                    }
                    break;
                case SEARCH_BY_AUTHOR:
                    if (strstr(current->book.authorName, searchTerm) != NULL) {
                        addBookToList(&head, current->book);
                    }
                    break;
                case SEARCH_BY_PRICE_SMALLER:
                    if ((int)(current->book.price) <= (atoi(searchTerm))) {
                        addBookToList(&head, current->book);
                    }
                    break;
                case SEARCH_BY_PRICE_GREATER:
                    if ((int)(current->book.price) >= (atoi(searchTerm))) {
                        addBookToList(&head, current->book);
                    }
                    break;
                case SEARCH_BY_BESTSELLER:
                    if (current->book.bestSeller == atoi(searchTerm)) {
                        addBookToList(&head, current->book);
                    }
                    break;
                case SEARCH_BY_NEWER_THAN_PUBLISH:
                    current_time = time(NULL);
                    timeInfo = localtime(&current_time);
                    timeInfo->tm_year = atoi(searchTerm) - 1900;
                    timeInfo->tm_mon = 0;
                    timeInfo->tm_mday = 1;
                    compare_time = mktime(timeInfo);
                    if (current->book.publishDate >= compare_time) {
                        addBookToList(&head, current->book);
                    }
                    break;
                case SEARCH_BY_OLDER_THAN_PUBLISH:
                    current_time = time(NULL);
                    timeInfo = localtime(&current_time);
                    timeInfo->tm_year = atoi(searchTerm) - 1900;
                    timeInfo->tm_mon = 0;
                    timeInfo->tm_mday = 1;
                    compare_time = mktime(timeInfo);
                    if (current->book.publishDate <= compare_time) {
                        addBookToList(&head, current->book);
                    }
                    break;
                }
                current = current->next;
            }
        }
    }
    return head;
}

BookNode* getBooksInBudget(BooksHashTable* ht, int category, double budget) {
    BookNode* head = NULL;
    BookNode* current = ht->categories[category];
    while (current != NULL) {
        if (current->book.price <= budget) {
            addBookToList(&head, current->book);
        }
        current = current->next;
    }
    return head;
}

//edit the book's details, if integers are less than 0 it won't change them, and strings are null it won't change them either.
void editBook(BooksHashTable* ht, int id, const char* bookName, const char* authorName, int category, double price, int bestSeller, int quantity) {
    Book* book = getBookById(ht, id);
    if (book == NULL) {
        printf("Book not found\n");
        return;
    }

    if (bookName != NULL) {
        strcpy(book->bookName, bookName);
    }
    if (authorName != NULL) {
        strcpy(book->authorName, authorName);
    }
    if (category >= 0) {
        book->category = category;
    }
    if (price >= 0) {
        book->price = price;
    }
    if (bestSeller >= 0) {
        book->bestSeller = bestSeller;
    }
    if (quantity >= 0) {
        book->quantity = quantity;
    }
}

void printBooksList(BookNode* booksList) {
    if (!booksList) {
        printf("\nThere is no matching books to your search term.");
    }
    else {
        BookNode* cur = booksList;
        char categoryName[15];
        printf("ID\tName\t\t\t\t\t\tAuthor\t\t\tCategory\tBestseller\tPrice  Q   Publish\n");

        while (cur != NULL) {
            switch (cur->book.category) {
            case 0:
                strcpy(categoryName, "Fiction");
                break;
            case 1:
                strcpy(categoryName, "Non - fiction");
                break;
            case 2:
                strcpy(categoryName, "Children");
                break;
            case 3:
                strcpy(categoryName, "Science");
                break;
            case 4:
                strcpy(categoryName, "Educational");
                break;
            }
            char* date = stringFromDate(cur->book.publishDate);
            printf("%d\t%-40s\t%-20s\t%-10s\t%-8d\t%.2lf\t%d\t%s\n"
                , cur->book.id
                , cur->book.bookName
                , cur->book.authorName
                , categoryName
                , cur->book.bestSeller
                , cur->book.price
                , cur->book.quantity
                , date);
            free(date);
            cur = cur->next;
        }
    }
}

void addMockData(BooksHashTable* ht) {
    addBook(&ht, createBook(1, "The Da Vinci Code", "Dan Brown", 0, 15.99, dateFromString("2003"), 5, 1));
    addBook(&ht, createBook(2, "The Alchemist", "Paulo Coelho", 0, 12.99, dateFromString("1988"), 5, 0));
    addBook(&ht, createBook(3, "The Hunger Games", "Suzanne Collins", 0, 14.99, dateFromString("2008"), 5, 1));
    addBook(&ht, createBook(4, "The Girl on Train", "Paula Hawkins", 0, 13.99, dateFromString("2015"), 5, 0));
    addBook(&ht, createBook(5, "The Lost Symbol", "Dan Brown", 0, 16.99, dateFromString("2009"), 5, 1));
    addBook(&ht, createBook(6, "Outliers: Success", "Malcolm Gladwell", 1, 20.99, dateFromString("2008"), 5, 0));
    addBook(&ht, createBook(7, "Quiet: Introverts", "Susan Cain", 1, 22.99, dateFromString("2012"), 5, 0));
    addBook(&ht, createBook(8, "The Power of Now", "Eckhart Tolle", 1, 24.99, dateFromString("1997"), 5, 0));
    addBook(&ht, createBook(9, "The Origin of Species", "Charles Darwin", 3, 24.99, dateFromString("1859"), 5, 0));
    addBook(&ht, createBook(10, "The Structure Sci Rev", "Thomas S. Kuhn", 3, 22.99, dateFromString("1962"), 5, 0));
    addBook(&ht, createBook(11, "A Brief Hist Time", "Stephen Hawking", 3, 20.99, dateFromString("1988"), 5, 0));
    addBook(&ht, createBook(12, "The Double Helix", "James D. Watson", 3, 19.99, dateFromString("1968"), 5, 0));
    addBook(&ht, createBook(13, "The Quantum World", "Jim Al-Khalili", 3, 18.99, dateFromString("2011"), 5, 0));
    addBook(&ht, createBook(14, "The Demon-Haunted World", "Carl Sagan", 3, 17.99, dateFromString("1995"), 5, 0));
    addBook(&ht, createBook(15, "The Selfish Gene", "Richard Dawkins", 3, 24.99, dateFromString("1976"), 5, 0));
    addBook(&ht, createBook(16, "The Structure of Scientific Revolutions", "Thomas S. Kuhn", 3, 22.99, dateFromString("1962"), 5, 0));
    addBook(&ht, createBook(17, "The Elegant Universe: Superstrings", "Brian Greene", 3, 20.99, dateFromString("1999"), 5, 0));
    addBook(&ht, createBook(18, "The Demon-Haunted World: Science as a Candle in the Dark", "Carl Sagan", 3, 19.99, dateFromString("1995"), 5, 0));
    addBook(&ht, createBook(19, "The Art of War", "Sun Tzu", 4, 16.99, dateFromString("1900"), 5, 0));
    addBook(&ht, createBook(20, "Meditations", "Marcus Aurelius", 4, 15.99, dateFromString("1900"), 5, 0));
    addBook(&ht, createBook(21, "7 Habits of Eff People", "Stephen Covey", 4, 14.99, dateFromString("1989"), 5, 0));
    addBook(&ht, createBook(22, "Power of Positive Think", "N.Vincent Peale", 4, 13.99, dateFromString("1952"), 5, 0));
    addBook(&ht, createBook(23, "Think and Grow Rich", "Napoleon Hill", 4, 12.99, dateFromString("1937"), 5, 0));
    addBook(&ht, createBook(24, "The 4-Hour Work Week", "Tim Ferriss", 4, 11.99, dateFromString("2007"), 5, 0));
    addBook(&ht, createBook(25, "5 Elements of Eff Think", "Edward B. Burger", 4, 10.99, dateFromString("2009"), 5, 0));
}

void printAllBooks(BooksHashTable* ht) {
    int i;
    for (i = 0; i < BOOKS_CATEGORIES; i++) {
        printCategory(ht, i);
        printf("\n\n");
    }
}

BookNode* searchBooksByAuthorAndTitle(BooksHashTable* ht, char* author, char* title) {
    BookNode* head = NULL;
    for (int i = 0; i < BOOKS_CATEGORIES; i++) {
        BookNode* current = ht->categories[i];
        while (current != NULL) {
            if (strstr(current->book.authorName, author) != NULL && strstr(current->book.bookName, title) != NULL) {
                addBookToList(&head, current->book);
            }
            current = current->next;
        }
    }
    return head;
}