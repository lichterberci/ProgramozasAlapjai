#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct Time {
    int hours;
    int minutes;
    int seconds;
};

int main (int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "The program needs a text to work on!\n");
        return 1;
    }

    char* text = argv[1];

    struct Time time;

    if (strchr(text, 'h') != NULL) {
        // 15h 09m 53s

        int hours = 0;
        hours += (text[0] - '0') * 10;
        hours += (text[1] - '0') * 1;

        int minutes = 0;
        minutes += (text[4] - '0') * 10;
        minutes += (text[5] - '0') * 1;

        int seconds = 0;
        seconds += (text[8] - '0') * 10;
        seconds += (text[9] - '0') * 1;

        time.hours = hours;
        time.minutes = minutes;
        time.seconds = seconds;

    } else if (strstr (text, "AM") != NULL || strstr(text, "PM") != NULL) {

        // 10:15 AM

        int isPM = strstr (text, "AM") == NULL;

        int hours = 0;
        hours += (text[0] - '0') * 10;
        hours += (text[1] - '0') * 1;

        hours += isPM * 12;

        int minutes = 0;
        minutes += (text[3] - '0') * 10;
        minutes += (text[4] - '0') * 1;

        int seconds = 0;

        time.hours = hours;
        time.minutes = minutes;
        time.seconds = seconds;

    } else {

        // 23:17:06

        int hours = 0;
        hours += (text[0] - '0') * 10;
        hours += (text[1] - '0') * 1;

        int minutes = 0;
        minutes += (text[3] - '0') * 10;
        minutes += (text[4] - '0') * 1;

        int seconds = 0;
        seconds += (text[6] - '0') * 10;
        seconds += (text[7] - '0') * 1;

        time.hours = hours;
        time.minutes = minutes;
        time.seconds = seconds;

    }

    printf ("Time - %02d:%02d:%02d\n", time.hours, time.minutes, time.seconds);

    return 0;

}