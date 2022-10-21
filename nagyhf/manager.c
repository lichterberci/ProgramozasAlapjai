#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"
#include "sys/time.h"

#include "dataset.h"
#include "model.h"
#include "manager.h"

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

        FitModelForImage(model, &images[0], learningRate, NULL, NULL);
        FitModelForImage(model, &images[1], learningRate, NULL, NULL);
        FitModelForImage(model, &images[2], learningRate, NULL, NULL);
        FitModelForImage(model, &images[3], learningRate, NULL, NULL);

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
    double learningRate
) {

    struct timeval prevThousandStart;
    gettimeofday(&prevThousandStart, NULL);

    printf("[LOG] Starting learning phase...\n");

    // preallocate buffers to speed up computations
    double** valueBuffer = MakeValueBufferForModel(model.numLayers);
    double** derBuffer = MakeValueBufferForModel(model.numLayers);

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
                printf("[LOG] Fitting... epoch: %d/%d image: %5d/%5d (ETA: %s)\n", epoch + 1, numEpochs, imageIndex, trainSet.numData, etaString);
            }

            bool isResultOk = FitModelForImage(model, &trainSet.images[imageIndex], learningRate, valueBuffer, derBuffer);
        
            if (isResultOk == false) {
                fprintf(stderr, "[ERROR] Result has ±INFINITY in probs!\n");
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

void PrintImagesWithPredictions (Model model, Dataset dataset) {

    for (int i = 0; i < dataset.numData; i++)
    {
        LabeledImage image = dataset.images[i];
        PrintLabeledImage(image);

        Result result = Predict(model, image.data, NULL);
        PrintResult(result);

        printf("The accuracy fn says: %d\n", GetPredictionFromResult(result));
        
        printf("Press Q to quit, anything else to continue...\n");
        char c = fgetc(stdin);
        if (c == 'q' || c == 'Q')
            return;
    }
}