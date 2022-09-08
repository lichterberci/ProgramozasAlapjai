#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Room {

    int roomNumber;
    char name[50]; // use strcpy !!! 
    struct Room* nextPtr;

};

void PrintFloor (struct Room* firstRoom) {

    struct Room* currentRoom = firstRoom;

    while (currentRoom != NULL) {

        printf ("Szoba %d - %s\n", currentRoom->roomNumber, currentRoom->name);
        currentRoom = currentRoom->nextPtr;

    }

};

struct Room* ReadFloorFromFile (const char* fileName) {

    FILE* file = fopen(fileName, "r");

    char line[64];
    char roomNumberStringBuffer[4];

    struct Room* startPtr = NULL;
    struct Room* prevRoomPtr = NULL;

    while (fgets(line, sizeof(line), file)) {

        char* ptrToFirstSpace = strchr (line, ' ');

        struct Room* currentRoom = malloc(sizeof(struct Room));

        // get rid of \n\r

        for (size_t i = 0; i < strlen(line); i++)
        {
            if (line[i] == '\n' || line[i] == '\r')
                line[i] = '\0';
        }               

        // get room number

        for (size_t i = 0; i < ptrToFirstSpace - line; i++)
        {
            roomNumberStringBuffer[i] = line[i];
        }
        
        roomNumberStringBuffer[ptrToFirstSpace - line] = 0;

        int roomNumber = atoi (roomNumberStringBuffer);
        
        // get name

        char nameBuffer[64];

        for (size_t i = 0; i < strchr (line, '\0') - ptrToFirstSpace; i++) {
            nameBuffer[i] = line[i + (int)(ptrToFirstSpace - line) + 1];
        }

        nameBuffer[strchr (line, '\0') - ptrToFirstSpace] = 0;

        // set up current room

        currentRoom->roomNumber = roomNumber;
        strcpy(currentRoom->name, nameBuffer);
        currentRoom->nextPtr = NULL;

        // update list

        if (startPtr == NULL)
            startPtr = currentRoom;
        else
            prevRoomPtr->nextPtr = currentRoom;

        prevRoomPtr = currentRoom;
    }

    fclose(file);

    return startPtr;

};

void GetNumberOfRooms (struct Room* firstRoom, int numRooms[8]) {

    // initialize

    for (size_t i = 0; i < 8; i++) {
        numRooms[i] = 0;
    }

    // read through the list and add 1 to the respective floor counter

    struct Room* currentPtr = firstRoom;

    while (currentPtr != NULL) {

        int floor = currentPtr->roomNumber / 100;

        numRooms[floor]++;

        currentPtr = currentPtr->nextPtr;
    }

}

int GetMostPopulatedFloor (int numRooms[8]) {

    int maxIndex = 0;
    int maxCount = numRooms[0];

    for (int i = 1; i < 8; i++) {

        if (numRooms[i] > maxCount) {
            maxIndex = i;
            maxCount = numRooms[i];
        }
    }

    return maxIndex;

};

struct Room* GetRoomByName (struct Room* firstRoom, const char* name) {

    struct Room* currentPtr = firstRoom;

    while (currentPtr != NULL) {

        if (strcmp (currentPtr->name, name) == 0) {
            return currentPtr;
        }

        currentPtr = currentPtr->nextPtr;
    }

    return NULL;

};

void ComplicatedFunction (const char* fileName, const char* name) {

    struct Room* list = ReadFloorFromFile (fileName);

    struct Room* roomByName = GetRoomByName (list, name);

    if (roomByName == NULL) {
        printf ("Nem szallt meg!\n");
        return;
    }

    int numRooms[8];

    GetNumberOfRooms (list, numRooms);

    int mostPopulatedFloor = GetMostPopulatedFloor(numRooms);

    if (roomByName->roomNumber / 100 == mostPopulatedFloor) {
        printf("Megszallt es a legzsufoltabb szinten!\n");
    } else {
        printf ("Megszallt, de nem a legzsufoltabb szinten!\n");
    }

};

int main (void) {

    ComplicatedFunction ("test.txt", "Al Bert");

    return 0;

}; 