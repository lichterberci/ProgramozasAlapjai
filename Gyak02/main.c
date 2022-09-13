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

int main (void) {

    printf ("n = ");
    int n;
    scanf ("%d", &n);
    double res = CalculateSqrt (0, 1 << 30, n, 8);
    printf ("%.8lf\n", res);
    
    return 0;
};