#include "dataset.h"
#include "model.h"
#include "stdint.h"

#include "debugmalloc.h"

#define DEFAULT_NUM_NEURONS 200
#define DEFAULT_ACTIVATION_FUNCTION RELU
#define DEFAULT_NUM_EPOCHS 1
#define DEFAULT_LEARNING_RATE 0.000001

typedef enum {
    GENERAL, MODEL, LAYER
} ArgReadState;

typedef struct {

    char savePath[1000];
    char loadPath[1000];
    char dataFolderPath[1000];
    bool saveModelAfter;
    bool loadModelBefore;
    bool saveContinuously;

    bool shouldTrain;
    bool shouldShowImages;
    bool shouldTestAccuracy;
    bool shouldTestXOR;

    bool onlyPrintWrongImages;

    int numEpochs;
    double learningRate;

    LayerLayout* layerLayout;
    LayerLayout* layerLayoutHead;

} ProgramSetup;

/// @brief Sets the program into default mode.
ProgramSetup GetDefaultSetup ();
/// @brief Reads the command line arguments (passed as args here), 
/// and determines the mode, the program should be running in.
ProgramSetup ProcessArgs (int argc, char** argv);
/// @brief Prints images to stdout with their label for as long as the given dataset lasts 
void PrintImagesInfinitely (Dataset dataset);
/// @brief Tests the integrity of the code, by running the XOR problem on it.
/// If the backpropagation code is written successfully, the model should be able to learn
/// a non-linear function (the XOR function).
void TestXORProblem (
    int numNeuronsInHiddenLayer, 
    ActivationFunction activationFunction,
    int iterations,
    double learningRate,
    double maxDeviationFromResult
);
/// @brief Calculates the accuracy of a model on a dataset. 
double GetAccuracy (Model model, Dataset dataset);
/// @brief Fits a model with the given parameters. 
void FitModel (
    Model model, 
    Dataset trainSet, 
    Dataset testSet, 
    uint8_t numEpochs, 
    double learningRate,
    bool saveContinuously,
    const char* savePath
);
/// @brief Prints images to stdout with their labels and the given model's predictions.
void PrintImagesWithPredictions (Model model, Dataset dataset, bool onlyWrongs);