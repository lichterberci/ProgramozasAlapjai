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

    printf ("start\n");

    const char* trainImagePath = "./data/train-images.idx3-ubyte";
    const char* trainLabelPath = "./data/train-labels.idx1-ubyte";
    const char* testImagePath = "./data/t10k-images.idx3-ubyte";
    const char* testLabelPath = "./data/t10k-labels.idx1-ubyte";

    // Dataset trainSet = ReadDatasetFromFile(trainImagePath, trainLabelPath);
    // Dataset testSet = ReadDatasetFromFile(testImagePath, testLabelPath);

    // if (trainSet.numData == 0 || testSet.numData == 0)
    //     exit(-1);

    //                        V--- Number of hidden layers, don't forget to update!!!
    Model model = CreateModel(1, 1, SIGMOID, SOFTMAX);
        
    InitModelToRandom(&model, 1.0);

    PrintModel(model);

    LabeledImage dummyImage;
    dummyImage.data[0] = 0.3;
    dummyImage.label = 0;

    FitModelForImage(model, &dummyImage, 1.0);

    PrintModel(model);

    // for (int i = 0; i < 100; i++)
    // {
    //     FitModelForImage(model, &(trainSet.images[0]), 0.001);
    //     if (i % 10 == 0) {
    //         Result result = Predict(model, (double*)&(trainSet.images[0].data), NULL);
    //         //PrintResult(result);
    //     }
    // }
    

    printf("Code exited safely!");
    return 0;
}