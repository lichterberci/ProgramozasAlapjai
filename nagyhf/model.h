#pragma once

#include "stdint.h"
#include "stdarg.h"
#include "stdbool.h"

#include "dataset.h"

#define NUM_CLASSES 10 
// #define NUM_CLASSES 2
#define MAX_LAYER_DIM 1000

typedef enum {

    SIGMOID, RELU, SOFTMAX

} ActivationFunction;

typedef struct {

    uint32_t inputDim;
    uint32_t outputDim;

    // 2d array (inputDim * outputDim)
    // --> from input index 2 to output index 3 the index will be 3 * inputDim + 2
    //     (inputIndex + outputIndex * inputDim)
    //     --> this way the inputDim will step slower, which is good, because that will be in the outer for-loop
    double* weights; 
    // len = outputDim
    double* biases;
    
    ActivationFunction activationFunction;

} Layer;

typedef struct {
    
    uint8_t numLayers; // min 1 --> 1 = no hidden layers
    Layer* layers;

} Model;

typedef struct {
    uint32_t numNeurons;
    ActivationFunction activationFunction;
    struct LayerLayout* next; // for linked lists
} LayerLayout;

typedef struct {
    double probs[NUM_CLASSES];
} Result;

/// @brief The sigmoid function
/// @brief f(x) = 1 / (1 + e^-x) 
double Sigmoid (double x);
/// @brief Derivative of the sigmoid function 
double SigmoidDer (double x);
/// @brief The ReLU function
/// @brief f(x) = x >= 0 ? x : 0 
double ReLU (double x);
/// @brief The derivative of the ReLU function
double ReLUDer (double x);

/// @brief Prints a model to stdout
void PrintModel(Model model);
/// @brief Prints a model's architecture / layout to stdout 
void PrintModelLayout(Model model);
/// @brief Prints a result's probabilities to stdout
void PrintResult(Result result);
/// @brief Frees up all the memory the given model uses (all weights, biases, layers, etc) 
void FreeModel(Model model);
/// @brief Frees up all memory the given layer uses 
void FreeLayer(Layer layer);
/// @brief Initializes the given model's weights and biases to a random value between [-randomRange, +randomRange]
/// This will override all weights and all biases, so should only be called on uninitialized models!
void InitModelToRandom (Model* model, double randomRange);
/// @brief Sets up the layers of a model from dimension and activation fn lists
/// @param numHiddenLayers number of hidden layers
/// @param ... size, activation fn, size2, activation fn2, activation fn3
/// The number of additional parameters must be at least 1 (activation fn of last layer)
/// @return model instance
Model CreateModel(int numHiddenLayers, ...);
/// @brief Creates an uninitialized model from a linked-list of LayerLayouts
Model CreateModelFromLayout(LayerLayout* layout);
/// @brief Allocates a buffer with dimensions defined by the model's layers. 
/// @brief It does not create a buffer for the -1st vector (the image)
/// @param model 
/// @return pointer to the buffer
double** MakeValueBufferForModel (int numLayers);
/// @brief frees the value buffer
void FreeValueBuffer (Model model, double** buffer);
/// @brief Forwards the given image through the given model, and optionally saves the SUMS of the neurons to a buffer (not the values after the activation function)
/// @param model the model, with which we want to predict
/// @param input input image's data
/// @param out_neuronValues the SUMS!!! of the neurons, so sum(w*n + b)
/// @return the prediction result
Result Predict(Model model, double* input, double** out_neuronValues);
/// @brief implements cross-entropy algorithm
/// @brief cross-entropy: -sum(y[i] * log(s[i])) for i: 0..NUM_CLASSES
/// @return Cross-entropy cost of the given image with the label
double CalculateCost(uint8_t label, double* resultValues);
/// @brief Adjusts the model's weights
/// @param neuronValues holds the values of the neurons, filled during the prediction phase
void BackPropagate(
    Model model, 
    double** neuronValues, 
    LabeledImage* image, 
    double learningRate, 
    double** preallocatedDerBuffer, 
    double* out_cost
);
/// @brief Runs predict and then backpropagation on the model with the given image and target
/// @param model it is ok if it's not a pointer, because the layers variable will still point to the same memory address
/// @param image 
/// @param learningRate 
/// @returns whether the result is ok or inf/-inf/nan --> true = good, false = stop learning
bool FitModelForImage (Model model, LabeledImage* image, double learningRate, double** preallocatedValueBuffer, double** preallocatedDerBuffer, double* out_cost);
/// @brief Runs the model with the given image and gives back the cost
double CalculateAvgCostForModel (Model model, LabeledImage* images, int numImages);
/// @brief argmax
int GetPredictionFromResult(Result result);
/// @brief Determines whether the result's probs are valid numbers
bool IsResultOk (Result result);
/// @brief Writes the model's weights and biases to a file with the given path
void SaveModelToFile (Model model, const char* filePath);
/// @brief Reads the given file and returns the model stored in it
Model LoadModelFromFile (const char* filePath);