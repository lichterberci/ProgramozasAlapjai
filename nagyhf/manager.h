#include "dataset.h"
#include "model.h"

void PrintImagesInfinitely (Dataset dataset);
void TestXORProblem (
    int numNeuronsInHiddenLayer, 
    ActivationFunction activationFunction,
    int iterations,
    double learningRate,
    double maxDeviationFromResult
);
double GetAccuracy (Model model, Dataset dataset);
void FitModel (
    Model model, 
    Dataset trainSet, 
    Dataset testSet, 
    uint8_t numEpochs, 
    double learningRate
);
void PrintImagesWithPredictions (Model model, Dataset dataset);