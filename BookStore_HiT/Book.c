// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "Book.h"

//creates a book object from the arguments given to the function
Book createBook(int id, char* bookName, char* authorName, int category, double price, time_t publishDate, int quantity, int bestSeller) {
    Book book;
    book.id = id;
    strcpy(book.bookName, bookName);
    strcpy(book.authorName, authorName);
    book.category = category;
    book.price = price;
    book.publishDate = publishDate;
    book.quantity = quantity;
    book.bestSeller = bestSeller;
    return book;
};