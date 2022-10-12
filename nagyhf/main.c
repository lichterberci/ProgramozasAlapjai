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
    Model model = CreateModel(1, 1, SIGMOID, SIGMOID);
        
    InitModelToRandom(&model, 2.0);

    PrintModel(model);

    double dummyInput = 0.5;
    double* input = &dummyInput;

    Result result = Predict(model, input, NULL);
    PrintResult(result);

    LabeledImage dummyImage;
    dummyImage.data[0] = dummyInput;
    dummyImage.label = 3;

    FitModelForImage(model, &dummyImage, 1.0);

    Result result2 = Predict(model, input, NULL);
    PrintResult(result2);

    printf("Code exited safely!");
    return 0;
}