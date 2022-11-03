#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"
#include "string.h"

#include "dataset.h"
#include "model.h"
#include "manager.h"


int main (int argc, char **argv) {

    srand(0); // default seed

    ProgramSetup setup = ProcessArgs(argc, argv);

    if (setup.shouldTrain == false && setup.shouldTestAccuracy == false && setup.shouldShowImages == false) {
        printf("[WARNING] No action selected!\n");
        return 0;
    }

    Model model = {
        0, NULL
    };

    if (setup.loadModelBefore) {
        model = LoadModelFromFile(setup.loadPath);
        if (model.numLayers == 0) // invalid loading
            exit(-1);
    }

    if (setup.layerLayout != NULL) {

        // loading has the higher priority
        if (setup.loadModelBefore == false) {
            model = CreateModelFromLayout(setup.layerLayout);
            InitModelToRandom(&model, 1.0);
        } else {
            printf("[WARNING] Model layout ignored because the model was loaded!\n");
        }

        setup.layerLayoutHead = setup.layerLayout;
        // free layout list
        while (setup.layerLayoutHead != NULL) {
            LayerLayout* tmp = setup.layerLayoutHead->next;
            free(setup.layerLayoutHead);
            setup.layerLayoutHead = tmp;
        }
        free(setup.layerLayout);
    }

    if (model.numLayers == 0) {
        // default model
        model = CreateModel (1, DEFAULT_NUM_NEURONS, DEFAULT_ACTIVATION_FUNCTION, SOFTMAX);
        InitModelToRandom(&model, 1.0);
    }

    PrintModelLayout(model);

    char trainImagePath[1000];
    sprintf(trainImagePath, "%s/train-images.idx3-ubyte", setup.dataFolderPath);
    char trainLabelPath[1000];
    sprintf(trainLabelPath, "%s/train-labels.idx1-ubyte", setup.dataFolderPath);
    char testImagePath[1000];
    sprintf(testImagePath, "%s/t10k-images.idx3-ubyte", setup.dataFolderPath);
    char testLabelPath[1000];
    sprintf(testLabelPath, "%s/t10k-labels.idx1-ubyte", setup.dataFolderPath);

    Dataset trainSet = ReadDatasetFromFile(trainImagePath, trainLabelPath);
    Dataset testSet = ReadDatasetFromFile(testImagePath, testLabelPath);

    if (trainSet.numData == 0 || testSet.numData == 0)
        exit(-1);

    ////                        V--- Number of hidden layers, don't forget to update!!!
    //Model model = CreateModel(2, 1, RELU, 1, RELU, SOFTMAX);
    //InitModelToRandom(&model, 1.0);

    if (setup.shouldTrain)
        FitModel(model, trainSet, testSet, setup.numEpochs, setup.learningRate, setup.saveContinuously, setup.savePath);
    
    if (setup.shouldTestAccuracy) {
        double trainAccuracy = GetAccuracy(model, trainSet);
        printf("[LOG] Accuracy on the training set: %2.1lf%%\n", trainAccuracy * 100);
        
        double testAccuracy = GetAccuracy(model, testSet);
        printf("[LOG] Accuracy on the test set: %2.1lf%%\n", testAccuracy * 100);
    }

    if (setup.saveModelAfter) 
        SaveModelToFile(model, setup.savePath);

    if (setup.shouldShowImages)
        PrintImagesWithPredictions(model, testSet, setup.onlyPrintWrongImages);

    FreeModel(model);
    FreeDataset(trainSet);
    FreeDataset(testSet);

    printf("Code exited safely!");
    return 0;
}