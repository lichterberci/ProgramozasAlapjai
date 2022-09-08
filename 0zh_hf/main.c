#include <stdio.h>

// reading a file

void ReadFile (const char* fileName) {

    FILE* fp;

    fp = fopen (fileName, "r");

    char buffer[100];

    fgets (buffer, sizeof (buffer), fp);

    printf ("%s", buffer);

    fclose (fp);

};

void WriteFile (const char* fileName, const char* message) {

    FILE* fp;

    fp = fopen (fileName, "w");

    fprintf (fp, message);

    fclose (fp);

};

int main (void) {

    WriteFile ("test.txt", "This is not that bad...");
    ReadFile ("test.txt");

    return 0;

}; 