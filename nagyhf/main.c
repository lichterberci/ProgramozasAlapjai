#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

#include "dataset.h"


int main () {

    const char* trainImagePath = "./data/train-images.idx3-ubyte";
    const char* trainLabelPath = "./data/train-labels.idx1-ubyte";
    const char* testImagePath = "./data/t10k-images.idx3-ubyte";
    const char* testLabelPath = "./data/t10k-labels.idx1-ubyte";

    Dataset trainSet = ReadDatasetFromFile(trainImagePath, trainLabelPath);
    Dataset testSet = ReadDatasetFromFile(testImagePath, testLabelPath);

    if (trainSet.numData == 0 || testSet.numData == 0)
        exit(-1);

    const int start = 40000;
    const int num = 4;
    const int stride = 1;

    for (uint64_t i = start; i < start + num; i+=stride) {

        printf("i = %d\n", i);
        LabeledImage img = trainSet.images[i];
        PrintLabeledImage(img);
    }
    
    printf("Code exited safely!");
    return 0;
}