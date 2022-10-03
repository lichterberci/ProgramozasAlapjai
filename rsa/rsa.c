#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

static int log_level = 0;
static int max_bits = 8;

bool OverFlowDanger (uint64_t a, uint64_t b) {

    uint8_t log_a = 0;
    uint8_t log_b = 0;

    for (uint8_t i = 0; i < 64; i++)
    {
        if ((a & (1LL << i)) != 0) log_a = i + 1;
        if ((b & (1LL << i)) != 0) log_b = i + 1;
    }
    
    if (log_a + 1 + log_b + 1 >= 64) {
        if (log_level >= 1)
            printf("[WARNING] Overflow danger logs: a=0x%016llx, b=0x%016llx, log_a=%lld, log_b=%lld\n", a, b, log_a, log_b);
    }

    // if (a > (1LL << 20) && log_level >= 1)
    //     printf("[WARNING] Potentially dangerous product! log %llx = %d\n", a, log_a);
    // if (b > (1LL << 20) && log_level >= 1)
    //     printf("[WARNING] Potentially dangerous product! log %llx = %d\n", b, log_b);

    return log_a + 1 + log_b + 1 >= 64;
}

uint64_t PowMod (uint64_t base, uint64_t exponent, uint64_t modulo) {

    if (log_level >= 2)
        printf("[LOG] Calculating %lld ^ %lld mod %lld...\n", base, exponent, modulo);

    uint64_t result = 1;

    uint64_t _base = base;

    for (uint64_t i = 0; i < 16; i++)
    {

        if ((exponent & (1LL << i)) != 0) {

            if (OverFlowDanger(result, _base)) {
                if (log_level >= 1)
                    printf("[WARNING] (PowMod) Overflow danger when multiplying! (result=%lld exponent=%lld)\n", result, exponent);
            }

            result = (result * _base) % modulo;
        }

        if (OverFlowDanger(_base, _base)) {
            if (log_level >= 1)
                printf("[WARNING] (PowMod) Overflow danger when multiplying! (result=%lld exponent=%lld)\n", result, exponent);
        }

        _base = (_base * _base) % modulo;
    }
    
    return result;
}

uint64_t GCD (uint64_t a, uint64_t b) {

    if (log_level >= 2)
        printf("[LOG] Calculating GCD of %lld and %lld...\n", a, b);

    while (b > 0) {

        uint64_t t = b;
        b = a % b;
        a = t;
    }

    return a;
}

bool IsPrime (uint64_t n) {

    if (n < 2) return false;

    if ((n & 1) == 0) return false; // mod 2 == 0

    uint16_t a;

    if (log_level >= 2)
        printf("[LOG] Checking if %lld is prime...\n", n);

    for (int i = 0; i < 100; i++) {    

        a = 0;

        if ((n & 0xFF000000) > 0)
            a |= (uint64_t)rand() << 48;    
        if ((n & 0x00FF0000) > 0)
            a |= (uint64_t)rand() << 32;    
        if ((n & 0x0000FF00) > 0)
            a |= (uint64_t)rand() << 16;
        a |= rand();

        if (a >= n) {
            i--;
            continue;
        }

        if (GCD(a, n) != 1) 
            return false;

        if (PowMod(a, n - 1, n) != 1) 
            return false;
    }

    return true;
}

uint64_t GetPrime(uint8_t maxPower) {

    uint64_t result;

    do {

        result = 0;

        if (maxPower >= 48)
            result |= ((uint64_t)rand() << 48);
        if (maxPower >= 32)
            result |= ((uint64_t)rand() << 32);
        if (maxPower >= 16)
            result |= ((uint64_t)rand() << 16);

        result |= rand();

        uint64_t mask = (1 << maxPower) - 1;
        
        result &= mask;

        if (log_level >= 2)
            printf("[LOG] Trying potential prime %lld...\n", result);

    } while (IsPrime(result) == false);

    return result;
}

uint64_t GetRandomCoprime (uint64_t m, int maxPower) {

    uint64_t result;

    do {

        result = 0;

        if (maxPower >= 48)
            result |= ((uint64_t)rand() << 48);
        if (maxPower >= 32)
            result |= ((uint64_t)rand() << 32);
        if (maxPower >= 16)
            result |= ((uint64_t)rand() << 16);

        result |= rand();

        uint64_t mask = (1 << maxPower) - 1;
        
        result &= mask;

        if (log_level >= 2)
            printf("[LOG] Trying potential coprime %lld (to %lld)...\n", result, m);

    } while (GCD(result, m) != 1);

    return result;
}

uint64_t SolveForD (uint64_t c, uint64_t m) {

    // d is unknown
    // -->  c*d === 1 (m)
    //      a*d === e (m)

    uint64_t a1 = m;
    uint64_t e1 = 0;

    uint64_t a2 = c;
    uint64_t e2 = 1;

    uint64_t a_temp; 
    uint64_t e_temp; 

    while (a1 % a2 >= 1) {

        //   a1*d === e1 (m)
        // - a2*d === e2 (m)
        // =================
        //   a_t*d === e_t (m)

        if (OverFlowDanger(e2, (a1 / a2))) {
            if (log_level >= 1)
                printf(
                    "[WARNING] (SolveForD) Product can lead to overflow!\n"
                    "a=0x%016llx\n"
                    "b=0x%016llx\n",
                e2, (a1 / a2));
        }

        a_temp = a1 % a2;
        e_temp = e1 - e2 * (a1 / a2);

        if (a_temp == 1) return e_temp + m; // add the modulo, cuz we calculate the first negative result for some reason

        // swap

        a1 = a2;
        e1 = e2;

        a2 = a_temp;
        e2 = e_temp;
    }

    printf("[ERROR] loop ended without returning! (a1=%lld, e1=%lld, a2=%lld, e2=%lld, a_temp=%lld, e_temp=%lld)\n", a1, e1, a2, e2, a_temp, e_temp);

    return 0;
}

int main (int argc, char* argv[]) {

    if (argc >= 5) {
        if (strcmp (argv[3], "-log-level") == 0) {
            log_level = atoi (argv[4]);
        }
        else if (strcmp(argv[3], "-max-bits") == 0) {
            max_bits = atoi (argv[4]);
        }
    }
    if (argc >= 3) {
        if (strcmp (argv[1], "-log-level") == 0) {
            log_level = atoi (argv[2]);
        }
        else if (strcmp(argv[1], "-max-bits") == 0) {
            max_bits = atoi (argv[2]);
        }
    } else if (argc == 2) {
        if (strcmp (argv[1], "--verbose") == 0) {
            log_level = 2;
        } else if (strcmp (argv[1], "--warning") == 0) {
            log_level = 1;
        } else if (strcmp (argv[1], "--error") == 0) {
            log_level = 0;
        }
    } else {
        log_level = 0;
    }

    // perform RSA

    printf("max_bits: %d\n", max_bits);

    srand(time(NULL));

    uint64_t p = GetPrime(max_bits);
    uint64_t q = GetPrime(max_bits);
    uint64_t N = p * q;
    uint64_t m = (p - 1) * (q - 1);

    uint64_t c = GetRandomCoprime(m, max_bits);

    char x = 'a';

    uint64_t y = PowMod (x, c, N);

    uint64_t d = SolveForD (c, m);

    char x2 = PowMod (y, d, N);

    printf("-----------------------VARIABLES------------------\n");

    printf("p: %lld\n", p);
    printf("q: %lld\n", q);
    printf("N: %lld\n", N);
    printf("m: %lld\n", m);
    printf("c: %lld\n", c);

    printf("y = %lld ^ %lld mod %lld\n", x, c, N);
    printf("y: %lld\n", y);

    printf("%lld * d = 1 (mod %lld)\n", c, m);
    printf("d: %lld\n", d);

    printf("x2 = %lld ^ %lld mod %lld\n", y, d, N);
    printf("x2: %d\n", x2);

    printf("------------------------RESULTS-------------------\n");

    printf("The original message: '%c' (%d)\n", x, x);
    printf("The decrypted message: '%c' (%d)\n", x2, x2);

    return 0;
}