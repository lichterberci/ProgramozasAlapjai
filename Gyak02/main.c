#include <stdio.h>

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

int main (void) {

    printf ("n = ");
    int n;
    scanf ("%d", &n);
    PrintPrimes (n);
    
    return 0;
};