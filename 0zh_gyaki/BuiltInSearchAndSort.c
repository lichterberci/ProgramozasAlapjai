#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    float gpa;
} Person;

int PeopleCompare (const void* a, const void* b) {
    printf("Comparing %d and %d...\n", ((Person*)a)->id, ((Person*)b)->id);
    return (((Person*)a)->gpa - ((Person*)b)->gpa);
}

int NumCompare (const void* a, const void* b) {
    return *((int*)a) - *((int*)b);
}

int main (void) {

    // qsort numbers

    // Person people[5] = { 
    //     {
    //         0, 3.0
    //     },
    //     {
    //         1, 4.5
    //     },
    //     {
    //         2, 4.0
    //     },
    //     {
    //         3, 2.3
    //     }, 
    //     {
    //         4, 6.2
    //     }
    // };

    int nums[] = {3, 5, 0, 9, 1};

    // for (int i = 0; i < 5; i++)
    // {
    //     printf ("Person (%d) - %f\n", people[i].id, people[i].gpa);
    // }

    for (int i = 0; i < 5; i++)
    {
        printf("%d ", nums[i]);
    }
    printf("\n");
    

    // qsort (people, 5, sizeof (Person), PeopleCompare);
    qsort (nums, 5, 4, NumCompare);

    printf("--------------------------------\n");

    // for (int i = 0; i < 5; i++)
    // {
    //     printf ("Person (%d) - %f\n", people[i].id, people[i].gpa);
    // }
    
    for (int i = 0; i < 5; i++)
    {
        printf("%d ", nums[i]);
    }
    printf("\n");

    return 0;
}