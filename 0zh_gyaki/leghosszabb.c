#include <stdio.h>
#include <string.h>

int main (int argc, char* argv[]) {

    char* fileName;

    if (argc == 2) {
        fileName = argv[1];
    }
    else {
        fileName = "test.txt";
    }

    FILE* file = fopen(fileName, "r");

    char longestWord[256];
    char currentWord[256];
    int count = 0;

    while (feof(file) == 0) {

        char c = (char)fgetc (file);

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {

            currentWord[count] = c;
            count++;
        }
        else {

            if (strlen(longestWord) < strlen(currentWord))
                strcpy (longestWord, currentWord);

            count = 0;
        }

    }

    fclose (file);

    printf ("Longest word: %s\n", longestWord);

    return 0;
}