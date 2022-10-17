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
    Model model = CreateModel(1, 2, RELU, SOFTMAX);
    
    InitModelToRandom(&model, 1.0);

    PrintModel(model);

    // LabeledImage testImage = trainSet.images[0];

    LabeledImage dummyImage1;
    dummyImage1.data[0] = 0.0;
    dummyImage1.data[1] = 0.0;
    dummyImage1.label = 0;

    LabeledImage dummyImage2;
    dummyImage2.data[0] = 0.0;
    dummyImage2.data[1] = 1.0;
    dummyImage2.label = 1;

    LabeledImage dummyImage3;
    dummyImage3.data[0] = 1.0;
    dummyImage3.data[1] = 0.0;
    dummyImage3.label = 1;

    LabeledImage dummyImage4;
    dummyImage4.data[0] = 1.0;
    dummyImage4.data[1] = 1.0;
    dummyImage4.label = 0;

    // LabeledImage image = testImage;

    const int numImages = 4;

    LabeledImage* images = malloc(numImages * sizeof(LabeledImage));

    images[0] = dummyImage1;
    images[1] = dummyImage2;
    images[2] = dummyImage3;
    images[3] = dummyImage4;

    const int iterations = 1000000;
    const double learningRate = 0.01;

    // PrintResult(Predict(model, image.data, NULL));

    for (size_t i = 0; i < iterations; i++) {

        if (iterations >= 100) {
            if (i % (iterations / 100) == 0) {
                double avgCost = CalculateAvgCostForModel (model, images, numImages);
                printf("%2d%% - avg. cost: %e\n", i * 100 / iterations, avgCost);
            }
        }

        // dummyImage.data[0] = rand() / RAND_MAX;

        FitModelForImage(model, &images[0], learningRate);
        FitModelForImage(model, &images[1], learningRate);
        FitModelForImage(model, &images[2], learningRate);
        FitModelForImage(model, &images[3], learningRate);

    }

    PrintModel(model);

    PrintResult(Predict(model, images[0].data, NULL));
    PrintResult(Predict(model, images[1].data, NULL));
    PrintResult(Predict(model, images[2].data, NULL));
    PrintResult(Predict(model, images[3].data, NULL));
    

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