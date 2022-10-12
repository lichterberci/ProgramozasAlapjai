#pragma once

#include "stdint.h"
#include "stdarg.h"

#include "dataset.h"

#define NUM_CLASSES 10
#define MAX_LAYER_DIM 1000

typedef enum {

    SIGMOID, RELU

} ActivationFunction;

typedef struct {

    uint32_t inputDim;
    uint32_t outputDim;

    // 2d array (inputDim * outputDim)
    // --> from input index 2 to output index 3 the index will be 3 * inputDim + 2
    //     (inputIndex + outputIndex * currentDim)
    //     --> this way the currentDim will step slower, which is good, because that will be in the outer for-loop
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
    double probs[NUM_CLASSES];
} Result;

double Sigmoid (double x);
double SigmoidDer (double x);
double ReLU (double x);
double ReLUDer (double x);

void PrintModel(Model model);
void PrintResult(Result result);
void FreeModel(Model model);
void FreeLayer(Layer layer);
void InitModelToRandom (Model* model, double randomRange);
Model CreateModel(int numHiddenLayers, ...);

Result Predict(Model model, double* input);
double CalculateError(Model model, uint8_t label, Result result);
void BackPropagate(Model model);