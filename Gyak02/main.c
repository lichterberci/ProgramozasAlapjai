#include <stdio.h>
#include <math.h>

void Piramis () {

    printf ("N = ");
    int n;
    scanf ("%d", &n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            printf(" ");
        }
        for (int k = 0; k < 1 + 2 * i; k++)
        {
            printf ("0");
        }
        printf ("\n");        
    }

}

void PrintPrimes (int n) {

    int k = 2;
    while (n > 1)
    {
        if (n % k == 0) {
            printf ("%d\n", k);
            n /= k;
        } else {
            k++;
        }
    }
}

double CalculateSqrt (double lower, double upper, int n, int prec) {

    // implement Newtonian algorithm

    while ((upper - lower) > pow(10, -prec)) {

        double avg = (lower + upper) / 2;

        double sqr = avg * avg;

        if (sqr > n) {
            // adjust upper bound
            upper = avg;
        } else {
            // adjust lower bound
            lower = avg;
        }
    }

    return (lower + upper) / 2;
}

void RomanNumerals () {

    printf ("N = ");
    int n;
    scanf ("%d", &n);
    if (n < 0 || n >= 100) {
        fprintf (stderr, "Number not in range");
        return;
    }

    char result[50];
    int index = 0;

    if ((n / 100) == 90) {
        result[index++] = 'X';
        result[index++] = 'C';
    } 
    else if ((n / 50) == 1 && (n / 10) != 9)
        result[index++] = 'L';
    else if ((n / 10) == 4) {
        result[index++] = 'X';
        result[index++] = 'L';
    } 

    if (((n % 50) / 10) >= 1 && ((n % 50) / 10) < 4) {
        for (int i = 0; i < n / 10; i++)
        {
            result[index++] = 'X';
        }    
    }

    if ((n % 10) == 9) {
        result[index++] = 'I';
        result[index++] = 'X';
    }
    else if ((n % 10) >= 5) {
        result[index++] = 'V';
    }
    else if ((n % 10) == 4) {
        result[index++] = 'I';
        result[index++] = 'V';
    }

    if ((n % 5) < 4) {
        for (int i = 0; i < (n % 5); i++)
        {
            result[index++] = 'I';
        }
    }
    
    printf ("%s\n", result);

}

int main (void) {

    while (1) {
        RomanNumerals ();
    }
    
    return 0;
};