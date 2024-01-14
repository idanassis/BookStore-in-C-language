// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#pragma once

typedef struct book {
	int id;
	char bookName[30];
	char authorName[30];
	int category;
	double price;
	int quantity;
	time_t publishDate;
	int bestSeller; //need to change to another boolean value (isPopular)
}Book;

typedef struct bookNode {
	Book book;
	struct bookNode* next;
}BookNode;

//creates a book object from the arguments given to the function
Book createBook(int id, char* bookName, char* authorName, int category, double price, time_t publishDate, int quantity, int bestSeller);