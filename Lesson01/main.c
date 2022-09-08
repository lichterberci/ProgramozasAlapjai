#include <stdio.h>
#include <math.h>

void F1 () {

    printf("Hello, world!\n");

};

void F5 () {

    printf("Tartaly festese\n\n");

    float h, d;

    printf ("Milyen magas? ");
    scanf ("%f", &h);
    printf ("Mennyi  az atmeroje? ");
    scanf ("%f", &d);

    float A = h * d * 3.14f + 2 * (d * d / 4) * 3.14f;
    float festek = A / 2;

    printf("\n%f doboz festek kell.\n", festek);

};

void F6 () {

    double a, b, c;
    printf ("a: ");
    scanf ("%lf", &a);
    printf ("b: ");
    scanf ("%lf", &b);
    printf ("c: ");
    scanf ("%lf", &c);

    double D = (b * b - (4 * a * c));

    int numMegoldasok = D > 0 ? 2 : D == 0 ? 1 : 0;

    printf ("%d\n", numMegoldasok);

};

void F9 () {

    double a, b, c;
    printf ("a: ");
    scanf ("%lf", &a);
    printf ("b: ");
    scanf ("%lf", &b);
    printf ("c: ");
    scanf ("%lf", &c);

    double D = (b * b - (4 * a * c));

    if (D > 0) {

        double x1 = (-b + D) / (2 * a);
        double x2 = (-b - D) / (2 * a);

        printf ("x1: %lf, x2: %lf\n", x1, x2);

        return;
    }

    if (D == 0) {

        double x = (-b) / (2 * a);

        printf ("x: %lf\n", x);

        return;
    }
    
    printf ("-\n");

};

void F7 () {

    // int n = 1;
    // while (n <= 20) {
    //     printf ("%d\n", n);
    //     n++;
    // }

    for (int i = 1; i <= 20; i++)
        printf ("%d\n", i);

};

void F8 () {

    int length;

    printf ("Mekkora legyen a szakasz? ");
    scanf ("%d", &length);

    printf ("+");
    for (size_t i = 0; i < length; i++)
    {
        printf("-");
    }
    
    printf ("+\n");

};

int main (void) {

    F1 ();
    F5 ();
    F6 ();
    F7 ();
    F8 ();
    F9 ();

    return 0;
};