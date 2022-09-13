#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct Thing {

    int size;
    int value;

};

int SumValues (struct Thing** input, int length) {

    int sum = 0;
    for (int i = 0; i < length; i++)
        sum += input[i]->value;
    
    return sum;
};

int SumSize (struct Thing** input, int length) {

    int sum = 0;
    for (int i = 0; i < length; i++)
        sum += input[i]->size;
    
    return sum;
};


int GetBest (
    struct Thing** canUse, 
    int canUseLength, 
    struct Thing** used,
    int usedLength, 
    int capacity, 
    struct Thing** outputBufferPtr, 
    int* outputBufferLength
) {

    // return variables
    int maxValue = -1;
    struct Thing** maxOutputBufferPtr = NULL;
    int* maxOutputBufferLength = NULL;

    int currentSize = CalcSize (used, usedLength);

    for (int i = 0; i < canUseLength; i++)
    {
        struct Thing* chosen = canUse[i];

        if ((currentSize + chosen->size) <= capacity)
            continue;

        struct Thing** modCanUse = malloc (sizeof (struct Thing*) * (canUseLength - 1));

        // remove the chosen item from the canUse list
        // --> construct a newmodified list without the given item

        for (int j = 0, index = 0; j < canUseLength - 1; j++) {

            if (i == j)
                continue;

            modCanUse[index++] = canUse[j];
        }

        // append the chosen item to the used list

        struct Thing** modUsed = malloc (sizeof (struct Thing*) * (usedLength + 1));

        for (int j = 0; j < usedLength; j++)
            modUsed[j] = used[j];
        
        modUsed[usedLength] = chosen;

        struct Thing** _outputBufferPtr = NULL;
        int* _outputBufferLength = NULL;

        int value = GetBest (
            modCanUse, 
            canUseLength - 1, 
            modUsed, 
            usedLength + 1, 
            capacity, 
            _outputBufferPtr, 
            _outputBufferLength
        );

        // update best
        if (value > maxValue) {

            free (maxOutputBufferPtr);
            free (maxOutputBufferLength);

            maxValue = value;
            maxOutputBufferPtr = _outputBufferPtr;
            maxOutputBufferLength = _outputBufferLength;

        } else {

            free (_outputBufferPtr);
            free (_outputBufferLength);
        }
    }

    if (maxValue == -1) {

        // we cannot add any more things into the list
        // --> return currentSum and the current used, usedLength pair

        int currentValueSum = CalcValues (used, usedLength);

        outputBufferPtr = used;
        outputBufferLength = usedLength;

        return currentValueSum;
    }

    return maxValue;
}

int main (void) {

    const int NUM_THINGS = 3;

    const int CAPACITY = 3;

    struct Thing* thing1 = malloc (sizeof (struct Thing));
    thing1->size = 1;
    thing1->value = 1;

    struct Thing* thing2 = malloc (sizeof (struct Thing));
    thing2->size = 1;
    thing2->value = 1;

    struct Thing* thing3 = malloc (sizeof (struct Thing));
    thing3->size = 1;
    thing3->value = 1;

    struct Thing** completeList = malloc (sizeof (struct Thing*) * NUM_THINGS);

    completeList[0] = thing1;
    completeList[1] = thing2;
    completeList[2] = thing3;

    struct Thing** usedBuffer = malloc (sizeof (struct Thing*) * NUM_THINGS);

    Things** outputBuffer = malloc (sizeof (Things*) * NUM_THINGS);
    int* outputLengthBuffer = malloc (sizeof (int));

    int bestValue = GetBest (
        completeList, 
        NUM_THINGS, 
        usedBuffer, 
        0, 
        CAPACITY, 
        outputBuffer, 
        outputLengthBuffer
    );

    printf ("Best value is %d!\n", bestValue);

    for (int i = 0; i < *outputLengthBuffer; i++)
    {
        printf("Thing[%d] = { size: %d, value: %d }\n", i, outputLengthBuffer[i]->size, outputLengthBuffer[i]->value);
    }
    
    return 0;
};