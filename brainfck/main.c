#include "stdlib.h"
#include "stdio.h"

unsigned long long CustomStrlen (char* str) {
    unsigned long long result = 0;
    char* ptr = str;
    while (*ptr != '\0') {
        ptr++;
        result++;
    }
    return result;
}

char CustomGetChar () {
    char result;
    if (fscanf(stdin, "%c", &result) == EOF) return -1;
    return result;
}

unsigned long long GetLoopEndForPosition (char* program, unsigned long long* programPtr) {
    unsigned long long result = (*programPtr) + 1;
    int loopBalance = 1;
    while(result < CustomStrlen(program) - 1) { 
        if (program[result] == '[')
            loopBalance++;
        if (program[result] == ']')
            loopBalance--;
        if (loopBalance == 0)
            return result;
        result++;
    }
    fprintf(stderr, "Could not find loop end!");
    exit(-1);
}

unsigned long long GetLoopStartForPosition (char* program, unsigned long long* programPtr) {
    long long result = (*programPtr) - 1;
    int loopBalance = -1;
    while(result >= 0) { 
        if (program[result] == '[')
            loopBalance++;
        if (program[result] == ']')
            loopBalance--;
        if (loopBalance == 0)
            return (unsigned long long)result;
        result--;
    }
    fprintf(stderr, "Could not find loop start!");
    exit(-1);
}

void StepProgram (char c, char* data, char* program, short int* dataPtr, unsigned long long* programPtr) {

    switch (c)
    {
    case '>':
        (*dataPtr)++;
        if (*dataPtr >= (2 << 15)) {
            fprintf(stderr, "Data ptr pointing out of memory!\n");
            exit(-1);
        }
        break;
    case '<':
        (*dataPtr)--;
        if (*dataPtr < 0) {
            fprintf(stderr, "Data ptr pointing below 0!\n");
            exit(-1);
        }
        break;
    case '+':
        data[*dataPtr]++;
        break;
    case '-':
        data[*dataPtr]--;
        break;
    case '.':
        putchar(data[*dataPtr]);
        break;
    case ',':
    {
        char inputChar = CustomGetChar();
        data[*dataPtr] = inputChar;
        break;
    }
    case '[':
        if (data[*dataPtr] == 0) {
            // jump to the end of the loop
            *programPtr = GetLoopEndForPosition(program, programPtr); // we add the +1 at the end
        }
        break;
    case ']':
        if (data[*dataPtr] != 0) {
            *programPtr = GetLoopStartForPosition(program, programPtr) - 1; // we add the +1 at the end with the others    
        }   
        break;
    default:
        break;
    }

    (*programPtr)++; // we step the program ptr forward, so we don't read the same char every single step 

};

int main () {

    // char programkod[] ="[ThisprogramprintsSierpinskitriangleon80-columndisplay.]>++++[<++++++++>-]>++++++++[>++++<-]>>++>>>+>>>+<<<<<<<<<<[-[->+<]>[-<+>>>.<<]>>>[[->++++++++[>++++<-]>.<<[->+<]+>[->++++++++++<<+>]>.[-]>]]+<<<[-[->+<]+>[-<+>>>-[->+<]++>[-<->]<<<]<<<<]++++++++++.+++.[-]<]+++++*****Made*By:*NYYRIKKI*2002*****";

    // Bertalan (keresztnév)
    char programkod[] = "Ez a program kiírja a becenevem (Bertalan) ++++[++++>---<]>-.[-->+++<]>++.+++++++++++++.++.+[->+++<]>++.+++++++++++.-----------.+++++++++++++.";

    char data[2 << 15] = {0};
    short int dataPtr = 0;
    unsigned long long programPtr = 0;

    while (programPtr >= 0 && programPtr < CustomStrlen(programkod)) {
        StepProgram(programkod[programPtr], data, programkod, &dataPtr, &programPtr);
    }

    return 0;
}