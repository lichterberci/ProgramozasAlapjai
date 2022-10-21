#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"

#include "dataset.h"
#include "model.h"
#include "manager.h"

typedef enum {
    GENERAL, MODEL, LAYER
} ArgReadState;

int main (int argc, char **argv) {

    // general args:
    // --version
    // --help
    // --train
    // --test-accuracy
    // --show-images

    // model args:
    // -seed [SEED]
    // -load [PATH] 
    // -model -layer 800 RELU 400 SIGMOID
    // -save [PATH]

    ArgReadState state = GENERAL;

    for (int i = 1; i < argc; i++) {
        if (state == GENERAL) {
            if (strcmp(argv[i], "-seed") == 0) {
                if (++i >= argc) fprintf(stderr, "[ERROR] Invalid number of arguments!\n");
                srand(atoi(argv[i]));
                continue;
            }
            if (strcmp(argv[i], "-model") == 0) {
                state = MODEL;
                continue;
            }

        }
    }

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
    Model model = CreateModel(2, 1, RELU, 1, RELU, SOFTMAX);
    InitModelToRandom(&model, 1.0);

    PrintModel(model);
    SaveModelToFile(model, "./test.model");
    Model model2 = LoadModelFromFile("./test.model");
    PrintModel(model2);
    
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