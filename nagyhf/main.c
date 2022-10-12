#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

#include "dataset.h"
#include "model.h"


void PrintImagesInfinitely (Dataset dataset) {
    for (int i = 0; i < dataset.numData; i++)  {
        PrintLabeledImage(dataset.images[i]);
        printf("For next image press [Enter]...\n");
        getchar();    
    }
}


int main () {

    const char* trainImagePath = "./data/train-images.idx3-ubyte";
    const char* trainLabelPath = "./data/train-labels.idx1-ubyte";
    const char* testImagePath = "./data/t10k-images.idx3-ubyte";
    const char* testLabelPath = "./data/t10k-labels.idx1-ubyte";

    Dataset trainSet = ReadDatasetFromFile(trainImagePath, trainLabelPath);
    Dataset testSet = ReadDatasetFromFile(testImagePath, testLabelPath);

    if (trainSet.numData == 0 || testSet.numData == 0)
        exit(-1);

    //                        V--- Number of hidden layers, don't forget to update!!!
    Model model = CreateModel(3, 100, RELU, 100, RELU, 50, SIGMOID, SIGMOID);
    
    PrintModel(model);
    
    InitModelToRandom(&model, 4.0);

    PrintModel(model);

    // double dummyInput = 0.5;

    for (int i = 0; i < trainSet.numData; i++) {
        double* input = trainSet.images[i].data;
        // input = &dummyInput;

        Result result = Predict(model, input);

        // PrintResult(result);
        
        if (i % 1000 == 0)
            printf("[LOG] Predicting images... (%5d/%5d)\n", i, trainSet.numData);
    }

    printf("Code exited safely!");
    return 0;
}