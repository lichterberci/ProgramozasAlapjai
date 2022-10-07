#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"
#include "mnist.h"


int main () {

    load_mnist();

	int i;
	for (i=0; i<784; i++) {
		printf("%1.1f ", test_image[0][i]);
		if ((i+1) % 28 == 0) putchar('\n');
	}

    printf("label: %d\n", test_label[0]);


    return 0;
}