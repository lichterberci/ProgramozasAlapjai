#include "dataset.h"
#include "model.h"
#include "stdint.h"

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

ProgramSetup GetDefaultSetup ();
ProgramSetup ProcessArgs (int argc, char** argv);
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
    double learningRate,
    bool saveContinuously,
    const char* savePath
);
void PrintImagesWithPredictions (Model model, Dataset dataset, bool onlyWrongs);