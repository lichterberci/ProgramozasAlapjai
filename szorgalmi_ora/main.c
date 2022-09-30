#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define DEBUG 1
#define RUN_CONTINUOUSLY 0

#if DEBUG
#define FILENAME "E:\\ProgramozasAlapjai\\ProgramozasAlapjai\\szorgalmi_ora\\ora.svg"
#else
#define FILENAME "./ora.svg"
#endif

#define FILE_WIDTH 500
#define FILE_HEIGHT 500

#define HOUR_HAND_LENGTH 100
#define MINUTE_HAND_LENGTH 125
#define SECONDS_HAND_LENGTH 125
#define HOUR_HAND_COLOR "black"
#define MINUTE_HAND_COLOR "#222222"
#define SECONDS_HAND_COLOR "red"
#define HOUR_HAND_WIDTH 4
#define MINUTE_HAND_WIDTH 3
#define SECONDS_HAND_WIDTH 2

#define CLOCK_RADIUS 150
#define CLOCK_STROKE_COLOR "black"
#define CLOCK_FILL_COLOR "#dddddd"
#define CLOCK_STROKE_WIDTH 10

#define MARKER_LENGTH_THIN 15
#define MARKER_LENGTH_THICK 20
#define MARKER_STROKE_WIDTH_THIN 3
#define MARKER_STROKE_WIDTH_THICK 5
#define MARKER_COLOR "black"

#define PI 3.14159265359
#define PI_2 1.57079632679

typedef struct {
    double x, y;
} Position;

void WriteToFile (FILE* file, const char* input) {

    fprintf(file, "%s\n", input);
}

void WriteFileStart (FILE* file, int width, int height) {

    fprintf(
            file,
            "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n",
            width, height
    );
}

void DrawLine (FILE* file, int x1, int y1, int x2, int y2, int width, char color[8]) {

    fprintf (
            file,
            "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"%s\" stroke-width=\"%d\"/>\n",
            x1, y1, x2, y2, color, width
    );
}

void DrawCircle (FILE* file, int cx, int cy, int radius, int width, char color[8], char fill[8], const char* style) {

    fprintf (
            file,
            "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" stroke=\"%s\" fill=\"%s\" stroke-width=\"%d\" style = \"%s\"/>\n",
            cx, cy, radius, color, fill, width, style
    );
}

int MakeClock (const char* fileName, int hours, int minutes, int seconds) {
    // calculate positions of hands

    double angleOfHourHand = (PI * 2) * ((hours % 12) / 12.0);
    double angleOfMinutesHand = (PI * 2) * ((minutes) / 60.0);
    double angleOfSecondsHand = (PI * 2) * ((seconds) / 60.0);

    // printf ("Angles:\nH: %lf\nm: %lf\ns: %lf\n", angleOfHourHand, angleOfMinutesHand, angleOfSecondsHand);

    angleOfHourHand += angleOfMinutesHand / 12l;
    angleOfHourHand += angleOfSecondsHand / 12l / 60l;
    // modfl(2 * M_PI, &angleOfHourHand);

    angleOfMinutesHand += angleOfSecondsHand / 60l;
    // modfl(2 * M_PI, &angleOfMinutesHand);

    angleOfHourHand -= PI_2;
    angleOfMinutesHand -= PI_2;
    angleOfSecondsHand -= PI_2;

    // printf ("Angles:\nH: %lf\nm: %lf\ns: %lf\n", angleOfHourHand, angleOfMinutesHand, angleOfSecondsHand);

    Position endOfHourHand = {
            cos(angleOfHourHand) * HOUR_HAND_LENGTH,
            sin(angleOfHourHand) * HOUR_HAND_LENGTH,
    };

    Position endOfMinuteHand = {
            cos(angleOfMinutesHand) * MINUTE_HAND_LENGTH,
            sin(angleOfMinutesHand) * MINUTE_HAND_LENGTH,
    };

    Position endOfSecondsHand = {
            cos(angleOfSecondsHand) * SECONDS_HAND_LENGTH,
            sin(angleOfSecondsHand) * SECONDS_HAND_LENGTH,
    };

    FILE* file = fopen (fileName, "w");

    if (file == NULL) {
        fprintf (stderr, "ERROR: could not open file!");
        return -1;
    }

    Position markerOuterEnds[12];
    Position markerInnerEnds[12];

    for (int i = 0; i < 12; ++i) {

        double angle = 2 * PI * i / 12.0 - PI_2;

        markerOuterEnds[i].x = cos(angle) * CLOCK_RADIUS;
        markerOuterEnds[i].y = sin(angle) * CLOCK_RADIUS;

        markerInnerEnds[i].x = cos(angle) * (CLOCK_RADIUS - (((i % 3) == 0) ? MARKER_LENGTH_THICK : MARKER_LENGTH_THIN));
        markerInnerEnds[i].y = sin(angle) * (CLOCK_RADIUS - (((i % 3) == 0) ? MARKER_LENGTH_THICK : MARKER_LENGTH_THIN));
    }

    // write everything to the file

    WriteFileStart (file, FILE_WIDTH, FILE_HEIGHT);

    char gradientBuffer[1000];

    snprintf(gradientBuffer, sizeof(gradientBuffer),
             "<defs\n"
             "     id=\"defs2\">\n"
             "    <linearGradient\n"
             "       id=\"linearGradient3149\"\n"
             "       >\n"
             "      <stop\n"
             "         id=\"stop3145\"\n"
             "         offset=\"0\"\n"
             "         style=\"stop-color:#38f5fa;stop-opacity:1;\" />\n"
             "      <stop\n"
             "         style=\"stop-color:#4dced1;stop-opacity:1\"\n"
             "         offset=\"0.4\"\n"
             "         id=\"stop3150\" />\n"
             "      <stop\n"
             "         style=\"stop-color:#23696b;stop-opacity:1\"\n"
             "         offset=\"0.91\"\n"
             "         id=\"stop3157\" />\n"
             "      <stop\n"
             "         id=\"stop3147\"\n"
             "         offset=\"1\"\n"
             "         style=\"stop-color:#000000;stop-opacity:1\" />\n"
             "    </linearGradient>\n"
             "    <radialGradient\n"
             "       gradientUnits=\"userSpaceOnUse\"\n"
             "       r=\"%d\"\n"
             "       fy=\"%d\"\n"
             "       fx=\"%d\"\n"
             "       cy=\"%d\"\n"
             "       cx=\"%d\"\n"
             "       id=\"radialGradient3151\"\n"
             "       href=\"#linearGradient3149\"\n"
             "       />\n"
             "  </defs>"
             "<g>\n",
             CLOCK_RADIUS,
             FILE_HEIGHT / 2,
             FILE_WIDTH / 2,
             FILE_HEIGHT / 2,
             FILE_WIDTH / 2
         );

    WriteToFile(file, gradientBuffer);

    DrawCircle(file, FILE_WIDTH / 2, FILE_HEIGHT / 2, CLOCK_RADIUS, CLOCK_STROKE_WIDTH, CLOCK_STROKE_COLOR, CLOCK_FILL_COLOR,
               "opacity:1;mix-blend-mode:normal;fill-opacity:1;fill-rule:evenodd;;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none;fill:url(#radialGradient3151)"
    );

    for (int i = 0; i < 12; ++i) {
        DrawLine (
                file,
                FILE_WIDTH / 2 + markerInnerEnds[i].x,
                FILE_HEIGHT / 2 + markerInnerEnds[i].y,
                FILE_WIDTH / 2 + markerOuterEnds[i].x,
                FILE_HEIGHT / 2 + markerOuterEnds[i].y,
                ((i % 3) == 0) ? MARKER_STROKE_WIDTH_THICK : MARKER_STROKE_WIDTH_THIN,
                MARKER_COLOR
        );
    }

    DrawLine (file, FILE_WIDTH / 2, FILE_HEIGHT / 2, FILE_WIDTH / 2 + endOfSecondsHand.x, FILE_HEIGHT / 2 + endOfSecondsHand.y, SECONDS_HAND_WIDTH, SECONDS_HAND_COLOR);
    DrawLine (file, FILE_WIDTH / 2, FILE_HEIGHT / 2, FILE_WIDTH / 2 + endOfMinuteHand.x, FILE_HEIGHT / 2 + endOfMinuteHand.y, MINUTE_HAND_WIDTH, MINUTE_HAND_COLOR);
    DrawLine (file, FILE_WIDTH / 2, FILE_HEIGHT / 2, FILE_WIDTH / 2 + endOfHourHand.x, FILE_HEIGHT / 2 + endOfHourHand.y, HOUR_HAND_WIDTH, HOUR_HAND_COLOR);

    DrawCircle(file, FILE_WIDTH / 2, FILE_HEIGHT / 2, HOUR_HAND_WIDTH, 0, CLOCK_STROKE_COLOR, CLOCK_STROKE_COLOR, "");

    WriteToFile(file, "</g>");

    WriteToFile(file, "</svg>");

    fclose (file);

    return 0;
}

int main (void) {

    const char* fileName = FILENAME;

#if DEBUG && RUN_CONTINUOUSLY
    while (true) {

        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        int hours = tmp->tm_hour;
        int minutes = tmp->tm_min;
        int seconds = tmp->tm_sec;

        printf ("Writing time %02d:%02d:%02d...\n", hours, minutes, seconds);

        MakeClock(fileName, hours, minutes, seconds);

        sleep (1);
    };

    return 0;
#else
    int hours, minutes, seconds;
    int numRead = fscanf (stdin, "%d %d %d", &hours, &minutes, &seconds);

    if (numRead != 3) {
        fprintf (stderr, "ERROR: could not read input properly!");
        return -1;
    }

#if DEBUG
    printf ("Writing to file '%s'!\n", fileName);
    printf ("Time is %02d:%02d:%02d\n\n", hours, minutes, seconds);
#endif

    int errCode = MakeClock(fileName, hours, minutes, seconds);

    if (errCode != 0) {
        return errCode;
    }

#endif

#if DEBUG

    // check

    printf ("------------------------\n");
    printf ("The content of the file:\n");
    printf ("------------------------\n");

    FILE* file = fopen(fileName, "r");

    while (feof(file) == false) {
        putchar (fgetc(file));
    };

    printf ("\n------------------------\n");

    fclose (file);

#endif

    return 0;
}