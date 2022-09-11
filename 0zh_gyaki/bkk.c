#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define STOP_NAME_MAX_LEN 60
#define LINE_NAME_MAX_LEN 30

enum Mode {
    EASY, BEAST
};

struct Line {

    char lineName[LINE_NAME_MAX_LEN];
    struct Stop* firstStop;
    struct Line* next;

} line_t;

struct Stop {

    char stopName[STOP_NAME_MAX_LEN];
    struct Stop* next;

} stop_t;

struct Line* ReadLines (const char* fileName) {

    FILE* file = fopen (fileName, "r");

    char inputLine[1024];

    struct Line* lines = NULL;
    struct Line* lastLineInList = NULL;

    while (fgets (inputLine, sizeof (inputLine), file)) {

        bool isFirstWord = true;

        char temp[1024];
        int j = 0;

        struct Line* currentLine = malloc(sizeof (struct Line));
        struct Stop* lastStopInList = NULL;

        for (int i = 0; i < strlen(inputLine) + 1; i++) {

            if (inputLine[i] == '\t' || inputLine[i] == '\n' || inputLine[i] == '\r' || inputLine[i] == '\0') {

                if (isFirstWord) {

                    temp[j++] = 0;

                    strcpy(currentLine->lineName, temp);

                    isFirstWord = false;

                    j = 0;

                } else {

                    temp[j++] = 0;
                    
                    struct Stop* currentStop = malloc (sizeof (struct Stop));

                    strcpy(currentStop->stopName, temp);

                    currentStop->next = NULL;

                    if (lastStopInList != NULL) {
                        lastStopInList->next = currentStop;
                    } else {
                        currentLine->firstStop = currentStop;
                    }

                    lastStopInList = currentStop;

                    j = 0;

                }

            } else {

                temp[j++] = inputLine[i];

            }

        }

        // line is complete

        if (lastLineInList != NULL) {
            lastLineInList->next = currentLine;
        } else {
            lines = currentLine;
        }

        lastLineInList = currentLine;

    }

    fclose (file);

    return lines;

}

bool EasyAlgorithm (const char* fileName, const char* line1, const char* line2) {

    struct Line* lines = ReadLines (fileName);

    struct Stop* line1Stops = NULL;
    struct Stop* line2Stops = NULL;

    struct Line* _line = lines;

    while (_line != NULL) {

        if (strcmp (_line->lineName, line1) == 0) {
            line1Stops = _line->firstStop;
        }
        else if (strcmp (_line->lineName, line2) == 0) {
            line2Stops = _line->firstStop;
        }
        _line = _line->next;
    }

    struct Stop* stop1 = line1Stops;
    struct Stop* stop2 = line2Stops;

    bool result = false;

    while (stop1 != NULL && result == false) {

        stop2 = line2Stops;

        while (stop2 != NULL && result == false) {

            if (strcmp (stop1->stopName, stop2->stopName) == 0) {

                result = true;
                break;
            }

            stop2 = stop2->next;

        }

        stop1 = stop1->next;

    }
    
    // free the lines

    _line = lines;

    while (_line != NULL) {

        struct Stop* _stop = _line->firstStop;

        while (_stop != NULL) {

            _stop = _stop->next;
            free (_stop);
        }

        _line = _line->next;

        free (_line);
    }

    return result;

}

bool BeastAlgorithm (const char* fileName, const char* line1, const char* line2) {

}


int main (int argc, char* argv[]) {

    enum Mode mode;
    char* fileName;
    char* line1;
    char* line2;

    if (argc == 5) {

        fileName = argv[1];
        mode = strcmp(argv[2], "beast") == 0 ? BEAST : EASY;
        line1 = argv[3];
        line2 = argv[4];

        printf ("Done with reading args!\n");
    } else {
        exit (-1);
    }

    bool result = false;

    if (mode == EASY) {
        result = EasyAlgorithm (fileName, line1, line2);
    } else {
        result = BeastAlgorithm (fileName, line1, line2);
    }

    printf("You %s able to change from %s to %s!", (result ? "are" : "aren't"), line1, line2);

    return 0;
}