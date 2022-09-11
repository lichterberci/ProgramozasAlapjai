#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Book {

    char author[30 + 1];
    char title[50 + 1];
    int yearOfPublication;
    int id;
    int count;

};

// return 1 if b comes first, 0 if equal, -1 if a comes first
int Compare (const char* a, const char* b) {

    for (int i = 0; a[i] != 0 && b[i] != 0; i++)
    {
        if (a[i] == b[i])
            continue;

        return a[i] > b[i] ? 1 : -1;
    }

    return 0;

};

void SortByAuthor (struct Book* arr, int length) {

    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length; j++)
        {
            if (i == j)
                continue;

            if (Compare (arr[i].author, arr[j].author) == -1) {

                // j comes first

                struct Book temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }

        }
        
    }
};

void PrintBook (struct Book book) {
    printf ("%s - %s (%d) - #%d - %d\n", book.author, book.title, book.yearOfPublication, book.id, book.count);
};

struct Author {

    char name [30 + 1];
    int count;

};

int GetNextFreeSlotIndex (struct Author** map, int mapLength, struct Author* author) {

    int hash = 0;

    for (int i = 0; i < strlen(author->name); i++)
    {
        hash += author->name[i];
    }

    hash %= mapLength;

    for (int offset = 0; offset < mapLength; offset++)
    {
        int position = (hash + offset) % mapLength;

        if (map[position] == NULL)
            return position;
    }

    return -1;

}

struct Author* GetFromMap (struct Author** map, int mapLength, const char* name) {

    // printf ("Getting %s from map...\n", name);

    int hash = 0;

    for (int i = 0; i < strlen(name); i++)
    {
        hash += name[i];
    }

    hash %= mapLength;

    // printf ("Hash %d\n", hash);

    for (int offset = 0; offset < mapLength; offset++)
    {
        int position = (hash + offset) % mapLength;

        if (map[position] == NULL)
            continue;

        if (strcmp(name, map[position]->name) == 0)
            return map[position];
    }

    return NULL;

}

void PutIntoMap (struct Author** map, int mapLength, struct Author* author) {

    // // printf ("Putting %s into map...\n", author->name);

    int hash = 0;

    for (int i = 0; i < strlen(author->name); i++)
    {
        hash += author->name[i];
    }

    hash %= mapLength;

    // printf ("Hash = %d\n", hash);
    
    int index = GetNextFreeSlotIndex (map, mapLength, author);

    // printf ("Index = %d\n", index);

    if (index == -1)
        return;

    map[index] = author;

}

void CountByAuthorAndPrint (struct Book* books, int length) {

    // use hashmap
    //      it has {length} slots
    //      it uses sums to calculate hash
    //      on collision, it just moves to the next position

    struct Author** map = malloc(sizeof (struct Author) * length);

    for (int i = 0; i < length; i++)
    {
        map[i] = NULL;
    }
    

    for (int i = 0; i < length; i++)
    {
        // printf ("Processing book... (%s - %s)\n", books[i].author, books[i].title);

        struct Author* authorInHash = GetFromMap (map, length, books[i].author);

        if (authorInHash == NULL) {
            // create new and add to the map
            struct Author* author = malloc(sizeof (struct Author));

            strcpy(author->name, books[i].author);
            author->count = books[i].count;

            PutIntoMap (map, length, author);
        }
        else {
            authorInHash->count += books[i].count;
        }
    }
    
    // print

    for (int i = 0; i < length; i++)
    {
        if (map[i] == NULL)
            continue;

        printf ("%s - %d\n", map[i]->name, map[i]->count);

        free (map[i]);
    }
    
    free (map);

};

int main (void) {

    struct Book b1 = {
        "J.R.R Tolkien",
        "Lord of the Rings",
        1951,
        0,
        500
    };

    struct Book b2 = {
        "J.K.Rowling",
        "Harry Potter",
        1978,
        1,
        1500
    };

    struct Book b3 = {
        "Peter Lichter",
        "50 vicces konyv",
        2022,
        2,
        300
    };

    struct Book b4 = {
        "J.K.Rowling",
        "Harry Potter 2",
        1994,
        3,
        1500    
    };

    struct Book b5 = {
        "J.K.Rowling",
        "Harry Potter 3",
        1995,
        4,
        3500
    };

    struct Book books[5] = { b1, b2, b3, b4, b5};

    SortByAuthor (books, 5);

    printf ("--------------------------------\n");
    printf ("Sorted books (by author's name):\n");
    printf ("--------------------------------\n");

    for (int i = 0; i < 5; i++)
    {
        PrintBook (books[i]);
    }

    printf ("-----------------------\n");
    printf ("Num of books by author:\n");
    printf ("-----------------------\n");

    CountByAuthorAndPrint (books, 5);
    
    return 0;
}