#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"
#include "sys/time.h"
#include "string.h"

#include "dataset.h"
#include "model.h"
#include "manager.h"

ProgramSetup GetDefaultSetup () {

    ProgramSetup setup;

    strcpy(setup.dataFolderPath, "./data");
    setup.saveModelAfter = false;
    setup.loadModelBefore = false;
    setup.saveContinuously = false;

    setup.shouldTrain = false;
    setup.shouldShowImages = false;
    setup.shouldTestAccuracy = false;
    setup.shouldTestXOR = false;

    setup.onlyPrintWrongImages = false;

    setup.numEpochs = DEFAULT_NUM_EPOCHS; // default
    setup.learningRate = DEFAULT_LEARNING_RATE; // default

    setup.layerLayout = NULL;
    setup.layerLayoutHead = NULL;

    return setup;
}

ProgramSetup ProcessArgs (int argc, char** argv) {

    ProgramSetup setup = GetDefaultSetup();

    ArgReadState state = GENERAL;

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
                exit(0);
            }
            if (strcmp(argv[i], "--help") == 0) {
                printf("Possible arguments:\n");
                printf("\t--help\n");
                printf("\t--version\n");
                printf("\t--train\n");
                printf("\t--test-accuracy\n");
                printf("\t--show-images\n");
                printf("\t--only-wrong-images\n");
                printf("\t--save-continuously\n");
                printf("\t-seed [SEED]\n");
                printf("\t-save [PATH]\n");
                printf("\t-load [PATH]\n");
                printf("\t-data-folder [PATH]\n");
                printf("\t-model [LAYERS?]\n");
                printf("\t-layer [NUM_NEURONS] [ACTIVATION_FUNCTION]\n");
                printf("\t-num-epochs [NUM_EPOCHS]\n");
                printf("\t-learning-rate [LEARNING_RATE]\n");
                exit(0);
            }
            if (strcmp(argv[i], "--train") == 0) {
                setup.shouldTrain = true;
                continue;
            }
            if (strcmp(argv[i], "--test-accuracy") == 0) {
                setup.shouldTestAccuracy = true;
                continue;
            }
            if (strcmp(argv[i], "--show-images") == 0) {
                setup.shouldShowImages = true;
                continue;
            }
            if (strcmp(argv[i], "--only-wrong-images") == 0) {
                setup.onlyPrintWrongImages = true;
                continue;
            }
            if (strcmp(argv[i], "--save-continuously") == 0) {
                setup.saveContinuously = true;
                continue;
            }
            if (strcmp(argv[i], "-seed") == 0) {
                if (++i >= argc) { { fprintf(stderr, "[ERROR] Invalid number of arguments!\n");  exit(-1); }  exit(-1); }
                srand(atoi(argv[i]));
                continue;
            }
            if (strcmp(argv[i], "-model") == 0) {
                state = MODEL;
                continue;
            }
            if (strcmp(argv[i], "-save") == 0) {
                if (++i >= argc) { fprintf(stderr, "[ERROR] Invalid number of arguments!\n");  exit(-1); }
                strcpy(setup.savePath, argv[i]);
                setup.saveModelAfter = true;
                continue;
            }
            if (strcmp(argv[i], "-data-folder") == 0) {
                if (++i >= argc) { fprintf(stderr, "[ERROR] Invalid number of arguments!\n");  exit(-1); }
                strcpy(setup.dataFolderPath, argv[i]);
                printf("%s\n", setup.dataFolderPath);
                continue;
            }
            if (strcmp(argv[i], "-load") == 0) {
                if (++i >= argc) { fprintf(stderr, "[ERROR] Invalid number of arguments!\n");  exit(-1); }
                strcpy(setup.loadPath, argv[i]);
                setup.loadModelBefore = true;
                continue;
            }
            if (strcmp(argv[i], "-learning-rate") == 0) {
                if (++i >= argc) { fprintf(stderr, "[ERROR] Invalid number of arguments!\n");  exit(-1); }
                int scalar, power;
                int numRead = sscanf(argv[i], "%de-%d", &scalar, &power);
                if (numRead != 2) {
                    fprintf(stderr, "[ERROR] Invalid format! (format: xe-y, pl.: 3e-2)\n");
                    continue;
                }
                setup.learningRate = scalar * pow(10, -power);
                continue;
            }
            if (strcmp(argv[i], "-num-epochs") == 0) {
                if (++i >= argc) { fprintf(stderr, "[ERROR] Invalid number of arguments!\n");  exit(-1); }
                setup.numEpochs = atoi(argv[i]);
                continue;
            }
            fprintf(stderr, "[ERROR] Invalid argument '%s'!\n", argv[i]);
            exit(-1);
        }
        if (state == LAYER) {
            int numNeurons;

            if (sscanf(argv[i], "%d", &numNeurons) == 0) {
                state = GENERAL;
                i--; // we want to look at this again
                continue;
            }

            if (++i >= argc) { fprintf(stderr, "[ERROR] Invalid number of arguments!\n");  exit(-1); }
            ActivationFunction actfn;
            if (strcmp(argv[i], "RELU") == 0 || strcmp(argv[i], "relu") == 0) {
                actfn = RELU;
            } else if (strcmp(argv[i], "SIGMOID") == 0 || strcmp(argv[i], "sigmoid") == 0) {
                actfn = SIGMOID;
            } else {
                fprintf(stderr, "[ERROR] Invalid activation function '%s'!\n", argv[i]);
                exit(-1);
            } 
            LayerLayout* addedLayerLayout = malloc(sizeof(LayerLayout));
            addedLayerLayout->numNeurons = numNeurons;
            addedLayerLayout->activationFunction = actfn;
            addedLayerLayout->next = NULL;

            if (setup.layerLayoutHead == NULL) {
                setup.layerLayoutHead = addedLayerLayout;
                setup.layerLayout = addedLayerLayout;
            } else {
                setup.layerLayoutHead->next = addedLayerLayout;
                setup.layerLayoutHead = addedLayerLayout;
            }

            state = MODEL;
            continue;
        }
    }

    return setup;
}

void PrintImagesInfinitely (Dataset dataset) {
    for (int i = 0; i < dataset.numData; i++)  {
        PrintLabeledImage(dataset.images[i]);
        printf("For next image press [Enter]...\n");
        getchar();    
    }
}

// eg.: TestXORProblem (4, RELU, 1000000, 0.001, 0.2);
void TestXORProblem (
    int numNeuronsInHiddenLayer, 
    ActivationFunction activationFunction,
    int iterations,
    double learningRate,
    double maxDeviationFromResult
) {

    if (IMAGE_SIZE != 2) {
        printf("[WARNING] Set IMAGE_SIZE to 2!\n");
        return;
    }

    if (NUM_CLASSES != 2) {
        printf("[WARNING] Set NUM_CLASSES to 2!\n");
        return;
    }

    // data

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

    const int numImages = 4;

    LabeledImage* images = malloc(numImages * sizeof(LabeledImage));

    images[0] = dummyImage1;
    images[1] = dummyImage2;
    images[2] = dummyImage3;
    images[3] = dummyImage4;

    // model

    //                        V--- Number of hidden layers, don't forget to update!!!
    Model model = CreateModel(1, numNeuronsInHiddenLayer, activationFunction, SOFTMAX);
    
    InitModelToRandom(&model, 1.0);

    PrintModel(model);

    // fit the model

    for (size_t i = 0; i < iterations; i++) {

        if (iterations >= 100) {
            if (i % (iterations / 100) == 0) {
                double avgCost = CalculateAvgCostForModel (model, images, numImages);
                printf("%2d%% - avg. cost: %e\n", i * 100 / iterations, avgCost);
            }
        }

        FitModelForImage(model, &images[0], learningRate, NULL, NULL, NULL);
        FitModelForImage(model, &images[1], learningRate, NULL, NULL, NULL);
        FitModelForImage(model, &images[2], learningRate, NULL, NULL, NULL);
        FitModelForImage(model, &images[3], learningRate, NULL, NULL, NULL);

    }

    PrintModel(model);

    Result result1 = Predict(model, images[0].data, NULL);
    PrintResult(result1);
    if (
        fabs(result1.probs[0] - 1) <= maxDeviationFromResult
        && fabs(result1.probs[1] - 0) <= maxDeviationFromResult
    ) {
        printf("Test 1: PASSED\n");   
    } else {
        printf("Test 1: NOT PASSED\n");
    }
    Result result2 = Predict(model, images[1].data, NULL);
    PrintResult(result2);
    if (
        fabs(result2.probs[0] - 0) <= maxDeviationFromResult
        && fabs(result2.probs[1] - 1) <= maxDeviationFromResult
    ) {
        printf("Test 2: PASSED\n");   
    } else {
        printf("Test 2: NOT PASSED\n");
    }
    Result result3 = Predict(model, images[2].data, NULL);
    PrintResult(result3);
    if (
        fabs(result3.probs[0] - 0) <= maxDeviationFromResult
        && fabs(result3.probs[1] - 1) <= maxDeviationFromResult
    ) {
        printf("Test 3: PASSED\n");   
    } else {
        printf("Test 3: NOT PASSED\n");
    }
    Result result4 = Predict(model, images[3].data, NULL);
    PrintResult(result4);
    if (
        fabs(result4.probs[0] - 1) <= maxDeviationFromResult
        && fabs(result4.probs[1] - 0) <= maxDeviationFromResult
    ) {
        printf("Test 4: PASSED\n");   
    } else {
        printf("Test 4: NOT PASSED\n");
    }

    FreeModel(model);

    free(images);
}

double GetAccuracy (Model model, Dataset dataset) {

    printf("[LOG] Starting measuring accuracy\n"); // line to erase

    int passedTests = 0;

    for (int i = 0; i < dataset.numData; i++) {

        if (i % 1000 == 0) {
            printf("\033[A\33[2K\r");
            printf("[LOG] Measuring accuracy... (%5d/%5d)\n", i, dataset.numData);
        }

        LabeledImage testImage = dataset.images[i];

        Result result = Predict(model, testImage.data, NULL);

        int prediction = GetPredictionFromResult(result);

        if (testImage.label == prediction)
            passedTests++;
    }

    printf("\033[A\33[2K\r");
    printf("[LOG] Done measuring accuracy!!\n");

    return (double)passedTests / dataset.numData;
}

void FitModel (
    Model model, 
    Dataset trainSet, 
    Dataset testSet, 
    uint8_t numEpochs, 
    double learningRate,
    bool saveContinuously,
    const char* savePath
) {

    struct timeval prevThousandStart;
    gettimeofday(&prevThousandStart, NULL);

    printf("[LOG] Starting learning phase...\n");

    // preallocate buffers to speed up computations
    double** valueBuffer = MakeValueBufferForModel(model.numLayers);
    double** derBuffer = MakeValueBufferForModel(model.numLayers);

    double avgCost = 0.0;

    for (int epoch = 0; epoch < numEpochs; epoch++) {
        for (int imageIndex = 0; imageIndex < trainSet.numData; imageIndex++) {
            
            if (imageIndex % 1000 == 0) {

                char etaString[20];

                const int remainingIterations = (numEpochs - 1 - epoch) * trainSet.numData + (trainSet.numData - imageIndex);

                struct timeval now;
                gettimeofday(&now, NULL);

                double secs = (double)(now.tv_usec - prevThousandStart.tv_usec) / 1000000 + (double)(now.tv_sec - prevThousandStart.tv_sec);
                
                prevThousandStart = now;

                double etaSecs = secs * remainingIterations / 1000;

                if (etaSecs <= 60)
                    sprintf(etaString, "%2.1lfs", etaSecs);
                else if (etaSecs < 3600)
                    sprintf(etaString, "%2.1lfm", etaSecs / 60);
                else
                    sprintf(etaString, "%2.1lfh", etaSecs / 3600);

                printf("\033[A\33[2K\r");
                printf("[LOG] Fitting... epoch: %d/%d image: %5d/%5d (ETA: %s) [avg. cost: %e]\n", epoch + 1, numEpochs, imageIndex, trainSet.numData, etaString, avgCost);
            
                avgCost = 0.0;

                if (saveContinuously) {
                    SaveModelToFile(model, savePath);
                    printf("\033[A\33[2K\r");
                }
            }

            double cost;

            bool isResultOk = FitModelForImage(model, &trainSet.images[imageIndex], learningRate, valueBuffer, derBuffer, &cost);
        
            avgCost += cost / 1000;

            if (isResultOk == false) {
                fprintf(stderr, "[ERROR] Result has +/-INFINITY in probs!\n");
                exit(-1);
            }
        }

        double accuracy = GetAccuracy(model, testSet);
        printf("\033[A\33[2K\r");
        printf("\033[A\33[2K\r");
        printf("[LOG] Finished epoch %d! (acc: %2.1lf%%)\n", epoch + 1, accuracy * 100);
        printf("\n"); // line to delete
    }
    
    printf("\033[A\33[2K\r");
    printf("[LOG] Learning finished!\n");

    FreeValueBuffer(model, valueBuffer);
    FreeValueBuffer(model, derBuffer);

}

void PrintImagesWithPredictions (Model model, Dataset dataset, bool onlyWrongs) {

    for (int i = 0; i < dataset.numData; i++)
    {
        LabeledImage image = dataset.images[i];
        Result result = Predict(model, image.data, NULL);
        uint8_t guess = GetPredictionFromResult(result);

        if (guess == image.label)
            continue;

        PrintLabeledImage(image);
        PrintResult(result);
        printf("Guess: %d\n", guess);
        
        printf("Type in Q to quit, anything else to continue...\n");
        char c = fgetc(stdin);
        if (c == 'q' || c == 'Q')
            return;
    }
}
