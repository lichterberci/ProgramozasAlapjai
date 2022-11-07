# __API documentation__


## __dataset.c__


---

## __dataset.h__
### __ReadInt__
```c
int ReadInt (FILE* fp);
```
__Summary:__
 Reads 4 bytes from a file and converts them into a signed int32_t




### __ReadByte__
```c
uint8_t ReadByte (FILE* fp);
```
__Summary:__
 Reads 1 byte from a file




### __FreeDataset__
```c
void FreeDataset (Dataset dataset);
```
__Summary:__
 Frees up a dataset (with all its images) 




### __GetPixelOfImage__
```c
double GetPixelOfImage(LabeledImage image, int x, int y);
```
__Summary:__
 Returns the pixel value of an image at the given coordinates




### __PrintLabeledImage__
```c
void PrintLabeledImage (LabeledImage image);
```
__Summary:__
 Prints the given image into stdout




### __ReadDatasetFromFile__
```c
Dataset ReadDatasetFromFile (const char* imagePath, const char* labelPath);
```
__Summary:__
 Reads 2 files (one for the images, one for the labels) and zips them into a single dataset 






---

## __main.c__
### __main__
```c
int main (int argc, char **argv) {
```
__Summary:__
 The entry point of the program. 






---

## __manager.c__


---

## __manager.h__
### __GetDefaultSetup__
```c
ProgramSetup GetDefaultSetup ();
```
__Summary:__
 Sets the program into default mode.




### __ProcessArgs__
```c
ProgramSetup ProcessArgs (int argc, char** argv);
```
__Summary:__
 Reads the command line arguments (passed as args here), 
 and determines the mode, the program should be running in.




### __PrintImagesInfinitely__
```c
void PrintImagesInfinitely (Dataset dataset);
```
__Summary:__
 Prints images to stdout with their label for as long as the given dataset lasts 




### __TestXORProblem__
```c
void TestXORProblem (
    int numNeuronsInHiddenLayer, 
    ActivationFunction activationFunction,
    int iterations,
    double learningRate,
    double maxDeviationFromResult
);
```
__Summary:__
 Tests the integrity of the code, by running the XOR problem on it.
 If the backpropagation code is written successfully, the model should be able to learn
 a non-linear function (the XOR function).




### __GetAccuracy__
```c
double GetAccuracy (Model model, Dataset dataset);
```
__Summary:__
 Calculates the accuracy of a model on a dataset. 




### __FitModel__
```c
void FitModel (
    Model model, 
    Dataset trainSet, 
    Dataset testSet, 
    uint8_t numEpochs, 
    double learningRate,
    bool saveContinuously,
    const char* savePath
);
```
__Summary:__
 Fits a model with the given parameters. 




### __PrintImagesWithPredictions__
```c
void PrintImagesWithPredictions (Model model, Dataset dataset, bool onlyWrongs);
```
__Summary:__
 Prints images to stdout with their labels and the given model's predictions.






---

## __model.c__


---

## __model.h__
### __Sigmoid__
```c
double Sigmoid (double x);
```
__Summary:__
 The sigmoid function
 f(x) = 1 / (1 + e^-x) 




### __SigmoidDer__
```c
double SigmoidDer (double x);
```
__Summary:__
 Derivative of the sigmoid function 




### __ReLU__
```c
double ReLU (double x);
```
__Summary:__
 The ReLU function
 f(x) = x >= 0 ? x : 0 




### __ReLUDer__
```c
double ReLUDer (double x);
```
__Summary:__
 The derivative of the ReLU function




### __PrintModel__
```c
void PrintModel(Model model);
```
__Summary:__
 Prints a model to stdout




### __PrintModelLayout__
```c
void PrintModelLayout(Model model);
```
__Summary:__
 Prints a model's architecture / layout to stdout 




### __PrintResult__
```c
void PrintResult(Result result);
```
__Summary:__
 Prints a result's probabilities to stdout




### __FreeModel__
```c
void FreeModel(Model model);
```
__Summary:__
 Frees up all the memory the given model uses (all weights, biases, layers, etc) 




### __FreeLayer__
```c
void FreeLayer(Layer layer);
```
__Summary:__
 Frees up all memory the given layer uses 




### __InitModelToRandom__
```c
void InitModelToRandom (Model* model, double randomRange);
```
__Summary:__
 Initializes the given model's weights and biases to a random value between [-randomRange, +randomRange]
 This will override all weights and all biases, so should only be called on uninitialized models!




### __CreateModel__
```c
Model CreateModel(int numHiddenLayers, ...);
```
__Summary:__
 Sets up the layers of a model from dimension and activation fn lists

- __numHiddenLayers__:  number of hidden layers
- __...__:  size, activation fn, size2, activation fn2, activation fn3 The number of additional parameters must be at least 1 (activation fn of last layer)
 @return model instance




### __CreateModelFromLayout__
```c
Model CreateModelFromLayout(LayerLayout* layout);
```
__Summary:__
 Creates an uninitialized model from a linked-list of LayerLayouts




### __MakeValueBufferForModel__
```c
double** MakeValueBufferForModel (int numLayers);
```
__Summary:__
 Allocates a buffer with dimensions defined by the model's layers. 
 It does not create a buffer for the -1st vector (the image)

- __model__:  @return pointer to the buffer




### __FreeValueBuffer__
```c
void FreeValueBuffer (Model model, double** buffer);
```
__Summary:__
 frees the value buffer




### __Predict__
```c
Result Predict(Model model, double* input, double** out_neuronValues);
```
__Summary:__
 Forwards the given image through the given model, and optionally saves the SUMS of the neurons to a buffer (not the values after the activation function)

- __model__:  the model, with which we want to predict
- __input__:  input image's data
- __out_neuronValues__:  the SUMS!!! of the neurons, so sum(w*n + b)

__Returns:__
 the prediction result



### __CalculateCost__
```c
double CalculateCost(uint8_t label, double* resultValues);
```
__Summary:__
 implements cross-entropy algorithm
 cross-entropy: -sum(y[i] * log(s[i])) for i: 0..NUM_CLASSES


__Returns:__
 Cross-entropy cost of the given image with the label



### __BackPropagate__
```c
void BackPropagate(
    Model model, 
    double** neuronValues, 
    LabeledImage* image, 
    double learningRate, 
    double** preallocatedDerBuffer, 
    double* out_cost
);
```
__Summary:__
 Adjusts the model's weights

- __neuronValues__:  holds the values of the neurons, filled during the prediction phase



### __FitModelForImage__
```c
bool FitModelForImage (Model model, LabeledImage* image, double learningRate, double** preallocatedValueBuffer, double** preallocatedDerBuffer, double* out_cost);
```
__Summary:__
 Runs predict and then backpropagation on the model with the given image and target

- __model__:  it is ok if it's not a pointer, because the layers variable will still point to the same memory address

__Returns:__
 whether the result is ok or inf/-inf/nan --> true = good, false = stop learning



### __CalculateAvgCostForModel__
```c
double CalculateAvgCostForModel (Model model, LabeledImage* images, int numImages);
```
__Summary:__
 Runs the model with the given image and gives back the cost




### __GetPredictionFromResult__
```c
int GetPredictionFromResult(Result result);
```
__Summary:__
 argmax




### __IsResultOk__
```c
bool IsResultOk (Result result);
```
__Summary:__
 Determines whether the result's probs are valid numbers.




### __SaveModelToFile__
```c
void SaveModelToFile (Model model, const char* filePath);
```
__Summary:__
 Writes the model's weights and biases to a file with the given path.




### __LoadModelFromFile__
```c
Model LoadModelFromFile (const char* filePath);
```
__Summary:__
 Reads the given file and returns the model stored in it.




