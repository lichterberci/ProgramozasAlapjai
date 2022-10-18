#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

int main (void) {

    char stack[256];
    int stackptr = 0;

    while (!foef(stdin)) {
        char c = getchar();

        // csak ({[]}) érdekel

        char * zarojelek = "({[]})";

        if (strchr(zarojelek, c) != -1) {
            if (strchr(zarojelek, c) < 3) {
                stack[stackptr++] = c;
            } else if (strchr(zarojelek, stack[stackptr]) == 6 - strchr(zarojelek, c)) {
                stackptr--;
            } else {
                printf("Error at char '%c'\n", c);
                return -1;
            }
        }

        return 0;
    }

    return 0;
}