#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"

#include "dataset.h"
#include "model.h"


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

        FitModelForImage(model, &images[0], learningRate);
        FitModelForImage(model, &images[1], learningRate);
        FitModelForImage(model, &images[2], learningRate);
        FitModelForImage(model, &images[3], learningRate);

    }

    PrintModel(model);

    Result result1 = Predict(model, images[0].data, NULL);
    PrintResult(result1);
    if (
        fabs(result1.probs[0] - 1) <= maxDeviationFromResult
        && fabs(result1.probs[1] - 0) <= maxDeviationFromResult
    ) {
        /* This prints a ✓. */
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
        /* This prints a ✓. */
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
        /* This prints a ✓. */
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
        /* This prints a ✓. */
        printf("Test 4: PASSED\n");   
    } else {
        printf("Test 4: NOT PASSED\n");
    }

    FreeModel(model);

    free(images);
}

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
    Model model = CreateModel(1, 200, RELU, SOFTMAX);

    InitModelToRandom(&model, 1.0);

    // LETS DO THIS SHIT !!!
    
    const int numEpochs = 5;
    const double learningRate = 0.00001;

    printf("Starting learning phase...\n");

    for (int epoch = 0; epoch < numEpochs; epoch++) {
        for (int imageIndex = 0; imageIndex < trainSet.numData; imageIndex++) {
            
            if (imageIndex % 1000 == 0) {
                printf("\33[2K\r\033[A");
                printf("Fitting... epoch: %d/%d image: %5d/%5d\n", epoch, numEpochs, imageIndex, trainSet.numData);
            }

            FitModelForImage(model, &trainSet.images[imageIndex], learningRate);
        }
    }
    
    printf("\33[2K\r\033[A");
    printf("Learning finished!\n");

    printf("Starting calculating accuracy\n"); // line to erase

    int passedTests = 0;

    for (int i = 0; i < testSet.numData; i++) {

        if (i % 1000 == 0) {
            printf("\33[2K\r\033[A");
            printf("Calculating accuracy... (%5d/%5d)\n", i, testSet.numData);
        }

        LabeledImage testImage = testSet.images[i];
        Result result = Predict(model, testImage.data, NULL);
        int prediction = GetPredictionFromResult(result);
        if (testImage.label == prediction)
            passedTests++;
    }

    printf("\33[2K\r\033[A");
    printf("Accuracy: %d/%d (%2.2lf%%)\n", passedTests, testSet.numData, (passedTests * 100.0 / testSet.numData));

    printf("Code exited safely!");
    return 0;
}