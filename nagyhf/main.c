#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"
#include "string.h"

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
    // --train TODO:
    // --test-accuracy TODO:
    // --show-images TODO:
    // -num-epochs [NUM_EPOCHS]
    // -learning-rate [LEARNING_RATE] --> xe-y pl.: 2e-5

    // model args:
    // -seed [SEED]
    // -load [PATH] 
    // -model -layer 800 RELU -layer 400 SIGMOID
    // -save [PATH]

    ArgReadState state = GENERAL;

    char savePath[100];
    char loadPath[100];
    bool saveModelAfter = false;
    bool loadModelBefore = false;

    int numEpochs = 1; // default
    double learningRate = 0.000001; // default

    LayerLayout * layerLayout = NULL;
    LayerLayout * layerLayoutHead = NULL;

    for (int i = 1; i < argc; i++) {
        if (state == MODEL) {
            if (strcmp(argv[i], "-layer") == 0) {
                state = LAYER;
                continue;
            }
            state = GENERAL;
        }
        if (state == GENERAL) {
            if (strcmp(argv[i], "--version") == 0) {
                printf("version: 1.0.0\n");
                return 0;
            }
            if (strcmp(argv[i], "--help") == 0) {
                printf("Possible arguments:\n");
                printf("\t--help\n");
                printf("\t--version\n");
                printf("\t-seed [SEED]\n");
                printf("\t-save [PATH]\n");
                printf("\t-load [PATH]\n");
                printf("\t-model [LAYERS?]\n");
                printf("\t-layer [NUM_NEURONS] [ACTIVATION_FUNCTION]\n");
                printf("\t-num-epochs [NUM_EPOCHS]\n");
                printf("\t-learning-rate [LEARNING_RATE]\n");
                return 0;
            }
            if (strcmp(argv[i], "-seed") == 0) {
                if (++i >= argc) fprintf(stderr, "[ERROR] Invalid number of arguments!\n");
                srand(atoi(argv[i]));
                continue;
            }
            if (strcmp(argv[i], "-model") == 0) {
                state = MODEL;
                continue;
            }
            if (strcmp(argv[i], "-save") == 0) {
                if (++i >= argc) fprintf(stderr, "[ERROR] Invalid number of arguments!\n");
                strcpy(savePath, argv[i]);
                saveModelAfter = true;
                continue;
            }
            if (strcmp(argv[i], "-load") == 0) {
                if (++i >= argc) fprintf(stderr, "[ERROR] Invalid number of arguments!\n");
                strcpy(loadPath, argv[i]);
                loadModelBefore = true;
                continue;
            }
            if (strcmp(argv[i], "-learning-rate") == 0) {
                if (++i >= argc) fprintf(stderr, "[ERROR] Invalid number of arguments!\n");
                int scalar, power;
                int numRead = sscanf(argv[i], "%de-%d", &scalar, &power);
                if (numRead != 2) {
                    fprintf(stderr, "[ERROR] Invalid format! (format: xe-y, pl.: 3e-2)\n");
                    continue;
                }
                learningRate = scalar * pow(10, -power);
                continue;
            }
            if (strcmp(argv[i], "-num-epochs") == 0) {
                if (++i >= argc) fprintf(stderr, "[ERROR] Invalid number of arguments!\n");
                numEpochs = atoi(argv[i]);
                continue;
            }
            fprintf(stderr, "[ERROR] Invalid argument '%s'!\n", argv[i]);
            continue;
        }
        if (state == LAYER) {
            int numNeurons = atoi(argv[i]);
            if (++i >= argc) fprintf(stderr, "[ERROR] Invalid number of arguments!\n");
            ActivationFunction actfn;
            if (strcmp(argv[i], "RELU")) {
                actfn = RELU;
            } else if (strcmp(argv[i], "SIGMOID")) {
                actfn = SIGMOID;
            } else {
                fprintf(stderr, "[ERROR] Invalid activation function '%s'!\n", argv[i]);
            } 
            LayerLayout* addedLayerLayout = malloc(sizeof(LayerLayout));
            addedLayerLayout->numNeurons = numNeurons;
            addedLayerLayout->activationFunction = actfn;
            addedLayerLayout->next = NULL;

            if (addedLayerLayout == NULL) {
                layerLayoutHead = addedLayerLayout;
                layerLayout = addedLayerLayout;
            } else {
                layerLayoutHead->next = addedLayerLayout;
                layerLayoutHead = addedLayerLayout;
            }

            state = MODEL;
            continue;
        }
    }

    Model model;

    if (loadModelBefore) {
        model = LoadModelFromFile(loadPath);
    }

    if (layerLayout != NULL) {

        // loading has the higher priority
        if (loadModelBefore == false) {
            model = CreateModelFromLayout(layerLayout);
            InitModelToRandom(&model, 1.0);
        } else {
            fprintf(stderr, "[ERROR] Model layout ignored because the model was loaded!\n");
        }

        layerLayoutHead = layerLayout;
        // free layout list
        while (layerLayoutHead != NULL) {
            LayerLayout* tmp = layerLayoutHead->next;
            free(layerLayoutHead);
            layerLayoutHead = tmp;
        }
        free(layerLayout);
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
    //Model model = CreateModel(2, 1, RELU, 1, RELU, SOFTMAX);
    //InitModelToRandom(&model, 1.0);
    
    // const int numEpochs = 1;
    // const double learningRate = 1 * pow(10, -7); // should be lower if the model is trained for many epochs

    FitModel(model, trainSet, testSet, numEpochs, learningRate);
    
    double trainAccuracy = GetAccuracy(model, trainSet);
    printf("[LOG] Accuracy on the training set: %2.1lf%%\n", trainAccuracy * 100);
    
    double testAccuracy = GetAccuracy(model, testSet);
    printf("[LOG] Accuracy on the test set: %2.1lf%%\n", testAccuracy * 100);

    PrintImagesWithPredictions(model, testSet);

    printf("Code exited safely!");
    return 0;
}