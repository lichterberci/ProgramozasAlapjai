#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"

#include "dataset.h"
#include "model.h"
#include "manager.h"

int main () {

    srand(0); // set the seed

    const char* trainImagePath = "./data/train-images.idx3-ubyte";
    const char* trainLabelPath = "./data/train-labels.idx1-ubyte";
    const char* testImagePath = "./data/t10k-images.idx3-ubyte";
    const char* testLabelPath = "./data/t10k-labels.idx1-ubyte";

    Dataset trainSet = ReadDatasetFromFile(trainImagePath, trainLabelPath);
    Dataset testSet = ReadDatasetFromFile(testImagePath, testLabelPath);

    if (trainSet.numData == 0 || testSet.numData == 0)
        exit(-1);

    //                        V--- Number of hidden layers, don't forget to update!!!
    Model model = CreateModel(2, 800, RELU, 800, RELU, SOFTMAX);
    InitModelToRandom(&model, 1.0);
    
    const int numEpochs = 1;
    const double learningRate = 1 * pow(10, -7); // should be lower if the model is trained for many epochs

    FitModel(model, trainSet, testSet, numEpochs, learningRate);
    
    double trainAccuracy = GetAccuracy(model, trainSet);
    printf("[LOG] Accuracy on the training set: %2.1lf%%\n", trainAccuracy * 100);
    
    double testAccuracy = GetAccuracy(model, testSet);
    printf("[LOG] Accuracy on the test set: %2.1lf%%\n", testAccuracy * 100);

    PrintImagesWithPredictions(model, testSet);

    printf("Code exited safely!");
    return 0;
}