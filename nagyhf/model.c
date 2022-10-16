#include "model.h"

#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define VERBOSE_BACKPROP 0
#define VERBOSE_PREDICTION 0

int min (int a, int b) { 
    return a < b ? a : b;
}

double GetRandomDouble(double min, double max) {
    double result = (double)rand() / RAND_MAX;
    return (result * (max - min)) + min;
}

double Sigmoid (double x) {
    return 1 / (1 + exp(-x));
}

double SigmoidDer (double x) {
    return exp(-x) / pow(1 + exp(-x), 2);
}

double ReLU (double x) {
    return x >= 0 ? x : 0;
}

double ReLUDer (double x) {
    return x >= 0 ? 1 : 0;
}

void PrintResult(Result result) {
    printf("Result: probs = [ ");
    for (int i = 0; i < NUM_CLASSES; i++) {
        printf("%e", result.probs[i]);
        if (i < NUM_CLASSES - 1)
            printf(", ");
    }
    printf(" ]\n");
}

void FreeModel(Model model) {

    for (int i = 0; i < model.numLayers; i++)
        FreeLayer(model.layers[i]);

    free(model.layers);
}

void FreeLayer(Layer layer) {

    free(layer.weights);
    free(layer.biases);
}

void PrintModel(Model model) {

    printf("Model:\n");
    for (int i = 0; i < model.numLayers; i++) {

        const char* activationFunctionString = (
                model.layers[i].activationFunction == SIGMOID ? 
                "sigmoid" : model.layers[i].activationFunction == RELU ?
                "ReLU" : model.layers[i].activationFunction == SOFTMAX ?
                "softmax" : "Unknown"
            );

        printf("\tLayer[%d] (indim=%d, outdim=%d, act.fn=%s)\n", 
            i, 
            model.layers[i].inputDim, 
            model.layers[i].outputDim, 
            activationFunctionString
        );


        printf("\t\tweights: [");

        if (model.layers[i].weights != NULL) {
            for (int j = 0; j < min(10, model.layers[i].inputDim * model.layers[i].outputDim); j++) {

                printf("%e", model.layers[i].weights[j]);

                if (j < model.layers[i].inputDim * model.layers[i].outputDim - 1)
                    printf(", ");
            }

            if (10 < model.layers[i].inputDim * model.layers[i].outputDim)
                printf("...");
        } else
            printf(" NULL ");

        printf("]\n");

        printf("\t\tbiases: [");

        if (model.layers[i].biases != NULL) {
            
            for (int j = 0; j < min(10, model.layers[i].outputDim); j++) {

                printf("%e", model.layers[i].biases[j]);

                if (j < model.layers[i].outputDim - 1)
                    printf(", ");
            }

            if (10 < model.layers[i].outputDim)
                printf("...");
        } else
            printf(" NULL ");

        printf("]\n");
    }
}

void InitModelToRandom (Model* model, double randomRange) {

    for (size_t i = 0; i < model->numLayers; i++) {
        Layer layer = model->layers[i];

        if (i == 0) {
            if (layer.inputDim != IMAGE_SIZE) {
                fprintf(stdout, "[WARNING] The model's first layer has %d dimensions (not %d)\n", layer.inputDim, IMAGE_SIZE);
            }
        } else {
            if (layer.inputDim != model->layers[i - 1].outputDim) {
                fprintf(stdout, "[WARNING] There is a mismatch between layer %d's dim (%d) and layer %d's dim (%d)%\n", 
                    i - 1, model->layers[i - 1].outputDim, 
                    i, layer.inputDim
                );
            }
        }

        if (i == model->numLayers - 1) {
            if (layer.outputDim != NUM_CLASSES) {
                fprintf(stdout, "[WARNING] The model's last layer has %d dimensions (not %d)\n", layer.outputDim, NUM_CLASSES);
            }
        }

        if (layer.weights != NULL) {
            fprintf(stdout, "[WARNING] Layer weights are potentially being overridden! (weight pointer is not null)\n");
            free(layer.weights);
        }

        if (layer.biases != NULL) {
            fprintf(stdout, "[WARNING] Layer biases are potentially being overridden! (biases pointer is not null)\n");
            free(layer.biases);
        }

        layer.weights = malloc(layer.inputDim * layer.outputDim * sizeof(double));
        layer.biases = malloc(layer.outputDim * sizeof(double));

        for (size_t i = 0; i < layer.inputDim * layer.outputDim; i++) {
            layer.weights[i] = GetRandomDouble(-randomRange, randomRange);
        }

        for (size_t i = 0; i < layer.outputDim; i++) {
            layer.biases[i] = GetRandomDouble(-randomRange, randomRange);
        }

        model->layers[i] = layer;
    }
}

/// @brief Sets up the layers of a model from dimension and activation fn lists
/// @param numHiddenLayers number of hidden layers
/// @param ... size, activation fn, size2, activation fn2, activation fn3
/// The number of additional parameters must be at least 1 (activation fn of last layer)
/// @return model instance
Model CreateModel(int numHiddenLayers, ...) {

    Model model;
    model.numLayers = numHiddenLayers + 1;

    va_list ap;
    int numParams = numHiddenLayers * 2 + 1;
    va_start(ap, numHiddenLayers);

    int prevLayerDim = IMAGE_SIZE;

    for (int i = 0; i < numHiddenLayers + 1; i++) {

        int outputDim;

        if (i < numHiddenLayers)
            outputDim = va_arg(ap, int);
        else
            outputDim = NUM_CLASSES;

        ActivationFunction activationFn = va_arg(ap, ActivationFunction);        

        if (outputDim > MAX_LAYER_DIM) {
            fprintf(stderr, "[ERROR] Output dimension too big for layer %d! (dim=%d, max=%d)\n", i, outputDim, MAX_LAYER_DIM);
        }

        Layer layer = {
            prevLayerDim,
            outputDim,
            NULL, // weights
            NULL, // biases
            activationFn
        };

        model.layers[i] = layer;

        prevLayerDim = outputDim;
    }
    
    va_end(ap);

    return model;
}

Result Predict(Model model, double* input, double** out_neuronValues) {

    const int numLayers = model.numLayers;

    double prevLayerValues[MAX_LAYER_DIM] = {0};
    double currentLayerValues[MAX_LAYER_DIM] = {0};

    memcpy(&prevLayerValues, input, IMAGE_SIZE * sizeof(double));

    for (int currentLayerIndex = 0; currentLayerIndex < numLayers; currentLayerIndex++) {
        
        Layer currentLayer = model.layers[currentLayerIndex];

        if (VERBOSE_PREDICTION) {
            const char* activationFunctionString = (
                    currentLayer.activationFunction == SIGMOID ? 
                    "sigmoid" : currentLayer.activationFunction == RELU ?
                    "ReLU" : currentLayer.activationFunction == SOFTMAX ?
                    "softmax" : "Unknown"
                );

            printf("[LOG] Layer[%d]: (%d, %d) act.fn=%s\n", 
                currentLayerIndex, 
                currentLayer.inputDim, 
                currentLayer.outputDim,
                activationFunctionString
            );
        }

        int inputDim = currentLayer.inputDim;
        int outputDim = currentLayer.outputDim;

        memset(&currentLayerValues, 0, MAX_LAYER_DIM * sizeof(double));

        for (size_t outputIndex = 0; outputIndex < outputDim; outputIndex++) {

            double currentSum = 0;

            for (size_t inputIndex = 0; inputIndex < inputDim; inputIndex++) {

                double weight = currentLayer.weights[outputIndex * inputDim + inputIndex];
                double neuron = prevLayerValues[inputIndex];
                currentSum += weight * neuron;

                if (VERBOSE_PREDICTION)
                    printf("[LOG] sum += n=%1.3f * w=%1.3f\n", neuron, weight);
            }

            double bias = currentLayer.biases[outputIndex];
            currentSum += bias;

            if (VERBOSE_PREDICTION)
                printf("[LOG] sum += b=%1.3f\n", bias);

            double value;

            if (currentLayerIndex < numLayers - 1) {
                if (currentLayer.activationFunction == SIGMOID)
                    value = Sigmoid(currentSum);
                else if (currentLayer.activationFunction == RELU)
                    value = ReLU(currentSum);
                else
                    fprintf(stderr, "[ERROR] unknown activation function %d!\n", currentLayer.activationFunction);
            } else {

                // last layer --> we don't want sigmoid or relu, we want softmax (hard-coded)

                value = currentSum; // we have to fill out the sums firs, so we have something to normalize with
            }

            if (VERBOSE_PREDICTION)
                printf("[LOG] sum = %.3f val = %.3f\n", currentSum, value);

            currentLayerValues[outputIndex] = value;
        }
        
        // calculated layer, time to move on

        memcpy(&prevLayerValues, &currentLayerValues, MAX_LAYER_DIM * sizeof(double));

        if (out_neuronValues != NULL) {
            if (out_neuronValues[currentLayerIndex] != NULL) {
                memcpy(out_neuronValues[currentLayerIndex], &currentLayerValues, MAX_LAYER_DIM * sizeof(double));
            } else {
                printf("[WARNING] out_neuronValues is not null but out_neuronValues[%d] is!\n", currentLayerIndex);
            }
        }
    }

    // result is in the bottom 10 indicies of prevLayerValues

    Result result;

    memcpy(&(result.probs), &prevLayerValues, NUM_CLASSES * sizeof(Result));

    // implement softmax

    /*
    
    But it is easy to guard against that by using the identity softmax(x) = softmax(x + c) 
    which holds for any scalar c: Subtracting max(x) from x leaves a vector 
    that has only non-positive entries, ruling out overflow and at least one element that is zero 
    ruling out a vanishing denominator (underflow in some but not all entries is harmless).
    
    */

    double sumOfLayerExp = 0;

    if (VERBOSE_PREDICTION) {
        printf("original (before softmax)\n");
        PrintResult(result);
    }

    double maxTerm = result.probs[0];

    for (int i = 1; i < NUM_CLASSES; i++)
        if (result.probs[i] > maxTerm) 
            maxTerm = result.probs[i];

    for (int i = 0; i < NUM_CLASSES; i++)
        result.probs[i] -= maxTerm;
    

    for (int i = 0; i < NUM_CLASSES; i++) {
        sumOfLayerExp += exp(result.probs[i]);
    }

    sumOfLayerExp += 0.00001; // anti zero

    if (VERBOSE_PREDICTION)
        printf("sumexp: %e\n", sumOfLayerExp);

    for (int i = 0; i < NUM_CLASSES; i++) {
        result.probs[i] = exp(result.probs[i]) / sumOfLayerExp;
    }

    if (VERBOSE_PREDICTION) {
        printf("after softmax:\n");
        PrintResult(result);
    }

    if (out_neuronValues != NULL) {
        if (out_neuronValues[numLayers - 1] != NULL) {
            memcpy(out_neuronValues[numLayers - 1], &(result.probs), NUM_CLASSES * sizeof(double));
        } else {
            printf("[WARNING] out_neuronValues is not null but out_neuronValues[%d] is!\n", numLayers - 1);
        }
    }

    return result;
}

/// @brief Allocates a buffer with dimensions defined by the model's layers. 
/// @brief It does not create a buffer for the -1st vector (the image)
/// @param model 
/// @return pointer to the buffer
double** MakeValueBufferForModel (int numLayers) {

    double** result = malloc(numLayers * sizeof(double*));

    for (int i = 0; i < numLayers; i++) {
        result[i] = malloc(MAX_LAYER_DIM* sizeof(double)); // faster than malloc + memset but we hopefully dont need to set it to 0
    }

    return result;
}

void FreeValueBuffer (Model model, double** buffer) {

    if (buffer == NULL) return;

    for (int i = 0; i < model.numLayers; i++) {
        free(buffer[i]);
    }

    free(buffer);
}

/// @brief implements MSE for a single image
/// @return MSE cost of the given image with the label
double CalculateCost(uint8_t label, double* resultValues) {

    double cost = 0.0;

    for (uint8_t i = 0; i < NUM_CLASSES; i++) {
        
        if (label == i) // target = 1
            cost += pow(1 - resultValues[i], 2);
        else // target = 0
            cost += pow(0 - resultValues[i], 2);            
    }

    cost /= NUM_CLASSES; // we want the average at the end
    
    return cost;
}

/// @brief Adjusts the model's weights
/// @param model 
/// @param neuronValues holds the values of the neurons, filled during the prediction phase
void BackPropagate(Model model, double** neuronValues, LabeledImage* image, double learningRate) {

    if (VERBOSE_BACKPROP)
        printf("---------------------BACKPROP--------------------\n");

        

    int numLayers = model.numLayers;

    if (neuronValues == NULL) {
        fprintf(stderr, "[ERROR] Neuron values are NULL during backprop!\n");
        return;
    }

    if (VERBOSE_BACKPROP)
        for (int i = 0; i < NUM_CLASSES; i++) {
            printf("prev output [%d] = %f\n", i, neuronValues[numLayers - 1][i]);
        }

    double cost = CalculateCost (image->label, neuronValues[numLayers - 1]);

    if (VERBOSE_BACKPROP)
        printf("[LOG] cost: %e\n", cost);

    double** derBuffer = MakeValueBufferForModel(model.numLayers);

    // we first differentiate the cost function, store the results in derBuffer
    // then we differentiate the softmax function and update derBuffer
    // note: the softmax derivative should be calculated for every pair!

    // calculate derivative from cost
    for (int i = 0; i < NUM_CLASSES; i++) {
        
        // update the last layer's neuron derivatives by the cost's derivative

        // differentiate the cost function

        derBuffer[numLayers - 1][i] = (2 / NUM_CLASSES) * (neuronValues[numLayers - 1][i] - (image->label == i ? 1 : 0));

        if (VERBOSE_BACKPROP)
            printf("[LOG] cost der: %lf (i=%d) \n", derBuffer[numLayers - 1][i], i);
    }
    

    // update neurons' derivatives
    for (int layerIndex = numLayers - 1; layerIndex >= 0; layerIndex--) {

        for (int i = 0; i < model.layers[layerIndex].outputDim; i++) {

            if (layerIndex == numLayers - 1) {

                // differentiate softmax

                // S = neuronValues[layerIndex]
                // n = softmax^-1(neuronValues[layerIndex])
                // gamma = (i == j ? 1 : 0)
                // ==> dS[i]/dn[j] = S[i](gamma - S[j])

                double softMaxDerSum = 0;

                // this is already the softmax derivative as well!!!
                for (int j = 0; j < NUM_CLASSES; j++) {
                    uint8_t gamma = i == j ? 1 : 0;
                    softMaxDerSum += neuronValues[layerIndex][i] * (gamma - neuronValues[layerIndex][j]);
                }
                            
                derBuffer[layerIndex][i] *= softMaxDerSum;

                if (VERBOSE_BACKPROP)
                    printf("[LOG] dC/dn=%e (l=%d,i=%d)\n", derBuffer[layerIndex][i], layerIndex, i);

            } else {

                // calculate derivative from previous layer

                const int prevLayerIndex = layerIndex + 1;
                const int prevLayerDim = model.layers[prevLayerIndex].outputDim;

                derBuffer[layerIndex][i] = 0;

                for (int j = 0; j < model.layers[prevLayerIndex].outputDim; j++) {
                    
                    // dE/dn[l] = dE/dn[l+1] * dn[l+1]/dn[l]
                    //                         dn[l+1][i]/dn[l][j] = f'(neuronValue[l+1][j]) * layers[l+1].weights[i, j]

                    double delta = derBuffer[prevLayerIndex][j];

                    if (model.layers[prevLayerIndex].activationFunction == SIGMOID)
                        delta *= (
                                SigmoidDer(neuronValues[prevLayerIndex][j]) 
                                * model.layers[prevLayerIndex].weights[i + prevLayerDim * j]
                            );
                    else if (model.layers[prevLayerIndex].activationFunction == RELU)
                        delta *= (
                                ReLUDer(neuronValues[prevLayerIndex][j]) 
                                * model.layers[prevLayerIndex].weights[i + prevLayerDim * j]
                            );
                    else if (model.layers[prevLayerIndex].activationFunction == SOFTMAX)
                        delta *= (
                                neuronValues[prevLayerIndex][j] 
                                * model.layers[prevLayerIndex].weights[i + prevLayerDim * j]
                            );
                    else
                        fprintf(stderr, "[ERROR] (%s:%d) Unknown activation function %d\n", __FILE__, __LINE__, model.layers[prevLayerIndex].activationFunction);
                    

                    derBuffer[layerIndex][i] -= delta;
                }

                if (VERBOSE_BACKPROP)
                    printf("[LOG] dC/dn=%e (l=%d,i=%d)\n", derBuffer[layerIndex][i], layerIndex, i);
            }
        }
    }

    // neuron derivatives are done, the bulk of the work has been done

    // adjust weights and biases
    for (int layerIndex = 0; layerIndex < numLayers; layerIndex++) {

        // for every neuron in our layer
        for (int i = 0; i < model.layers[layerIndex].outputDim; i++) {
            
            // adjust weights
            for (int j = 0; j < model.layers[layerIndex].inputDim; j++) {
                
                // dn[l][i]/dw[l][i, j] = f'(neuronValues[l]) * neuronValues[l-1][j]
                // --> dE/dw = dE/dn * dn/dw
            
                double delta = derBuffer[layerIndex][i];

                double prevValue = layerIndex == 0 ? image->data[j] : neuronValues[layerIndex - 1][j];

                if (model.layers[layerIndex].activationFunction == SIGMOID)
                    delta *= SigmoidDer(neuronValues[layerIndex][i]) * prevValue;
                else if (model.layers[layerIndex].activationFunction == RELU)
                    delta *= ReLUDer(neuronValues[layerIndex][i]) * prevValue;
                else if (model.layers[layerIndex].activationFunction == SOFTMAX)
                    delta *= 1 * prevValue; // we have already calculated it
                else
                    printf("[ERROR] Unknown activation function %d\n", model.layers[layerIndex].activationFunction);

                if (VERBOSE_BACKPROP)
                    printf("[LOG] dC/dw=%e (l=%d,i=%d,j=%d)\n", delta, layerIndex, i, j);

                model.layers[layerIndex].weights[j + i * model.layers[layerIndex].inputDim] -= delta * cost * learningRate;
            }

            // adjust bias
            // dn[l][i]/db[l] = f'(neuronValues[l][i])
            // --> dE/db = dE/dn * dn/db

            double delta = derBuffer[layerIndex][i];

            if (model.layers[layerIndex].activationFunction == SIGMOID)
                delta *= SigmoidDer(neuronValues[layerIndex][i]);
            else if (model.layers[layerIndex].activationFunction == RELU)
                delta *= ReLUDer(neuronValues[layerIndex][i]);
            else if (model.layers[layerIndex].activationFunction == SOFTMAX)
                delta *= 1; // we have already calculated it
            else
                printf("[ERROR] Unknown activation function %d\n", model.layers[layerIndex].activationFunction);
            
            if (VERBOSE_BACKPROP)
                printf("[LOG] dC/db=%e (l=%d,i=%d)\n", delta, layerIndex, i);

            model.layers[layerIndex].biases[i] -= delta * cost * learningRate;
        }
    }

    FreeValueBuffer(model, derBuffer);

    if (VERBOSE_BACKPROP)
        printf("--------------------------------------------------\n");
}

/// @brief Runs predict and then backpropagation on the model with the given image and target
/// @param model it is ok if it's not a pointer, because the layers variable will still point to the same memory address
/// @param image 
/// @param learningRate 
void FitModelForImage (Model model, LabeledImage* image, double learningRate) {

    double** valueBuffer = MakeValueBufferForModel (model.numLayers);

    Result result = Predict (model, image->data, valueBuffer);

    BackPropagate (model, valueBuffer, image, learningRate);

    FreeValueBuffer (model, valueBuffer);
}