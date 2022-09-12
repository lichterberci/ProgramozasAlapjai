#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

int main (void) {

    char text[80 + 1];

    gets (text);

    int i = 0;
    int j = strlen(text) - 1;
    
    while (i < j) {

        // printf ("Checking '%c' = '%c'?\n", text[i], text[j]);

        char c1 = text[i];

        if ((('0' <= c1 && c1 <= '9') || ('a' <= c1 && c1 <= 'z') || ('A' <= c1 && c1 <= 'Z')) == false) {
            // printf ("Increase i\n");
            i++;
            continue;
        }

        char c2 = text[j];

        if ((('0' <= c2 &&  c2 <= '9') || ('a' <= c2 && c2 <='z') || ('A' <= c2 && c2 <='Z')) == false) {
            // printf ("Decrease j\n");
            j--;
            continue;
        }

        if (('A' <= c1 && c1 <='Z')) {
            c1 = c1 - 'A' + 'a';
        }

        if (('A' <= c2 && c2 <= 'Z')) {
            c2 = c2 - 'A' + 'a';
        }

        // printf ("(%c = %c?)\n", c1, c2);

        if (c1 != c2) {
            printf ("Nem palindrom!\n");
            return 0;
        }

        i++;
        j--;

    }

    printf ("Palindrom!\n");

    return 0;

}