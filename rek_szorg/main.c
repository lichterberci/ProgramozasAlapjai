#include "stdio.h"
#include "stdlib.h"

char* alloc_char (int depth) {
    int c = getchar();

    if (c == EOF || c == (int)'\n') {
        char* str = malloc(depth + 1);
        str[depth] = 0;
        return str;
    }

    char* str = alloc_char(depth + 1);
    str[depth] = (char)c;
    return str;
}

char* sort_beolvas (void) {
    return alloc_char(0);
}

int main (void) {

    char* str = sort_beolvas();

    printf("%s\n", str);

    free(str);

    return 0;
}