#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>


bool IsPrime (int n) {

    if (n == 1)
        return false;

    for (int i = 2; i < n; i++)
        if (n % i == 0)
            return false;

    return true;
};

int main (int argc, char* argv[]) {

    int count = 0;
    int numbers[1000];

    for (int i = 1; i < argc; i++) {

        int n = atoi(argv[i]);

        numbers[i - 1] = n;

        if (n == 0) {
            count = i - 2;
            break;
        }
    }

    float avg = 0;

    for (int i = 0; i < count; i++) {
        avg += numbers[i];
    }

    avg /= count;

    for (int i = count - 1; i >= 0; i--)
    {
        int n = numbers[i];

        if (n < avg) {

            if (IsPrime (n))
            {
                printf ("%d\n", n);
            }
        }
    }
    
    return 0;

};