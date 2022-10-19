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
    // printf("o'(%3.2e) = %3.2e\n", x, exp(-x) / pow(1 + exp(-x), 2));
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

        // printf("layer[%d] - inputDIm: %d, outputDim: %d\n", i, layer.inputDim, layer.outputDim);

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
    model.layers = malloc(model.numLayers * sizeof(Layer));

    va_list ap;
    int numParams = numHiddenLayers * 2 + 1;
    
    va_start(ap, numParams);

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

/// @brief Forwards the given image through the given model, and optionally saves the SUMS of the neurons to a buffer (not the values after the activation function)
/// @param model the model, with which we want to predict
/// @param input input image's data
/// @param out_neuronValues the SUMS!!! of the neurons, so sum(w*n + b)
/// @return the prediction result
Result Predict(Model model, double* input, double** out_neuronValues) {

    Result emptyResult;

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

            printf("[VERBOSE] Layer[%d]: (%d, %d) act.fn=%s\n", 
                currentLayerIndex, 
                currentLayer.inputDim, 
                currentLayer.outputDim,
                activationFunctionString
            );
        }

        const int inputDim = currentLayer.inputDim;
        const int outputDim = currentLayer.outputDim;

        if (currentLayer.weights == NULL || currentLayer.biases == NULL) {
            fprintf(stderr, "[ERROR] Model is not initalized properly! (layer %d)\n", currentLayerIndex);
            return emptyResult;
        }

        memset(&currentLayerValues, 0, MAX_LAYER_DIM * sizeof(double));

        for (size_t outputIndex = 0; outputIndex < outputDim; outputIndex++) {

            double currentSum = 0;

            for (size_t inputIndex = 0; inputIndex < inputDim; inputIndex++) {

                double weight = currentLayer.weights[inputDim * outputIndex + inputIndex];
                double neuron = prevLayerValues[inputIndex];
                currentSum += weight * neuron;

                if (VERBOSE_PREDICTION)
                    printf("[VERBOSE] sum += n=%1.3f * w=%1.3f\n", neuron, weight);
            }

            double bias = currentLayer.biases[outputIndex];
            currentSum += bias;

            if (VERBOSE_PREDICTION)
                printf("[VERBOSE] sum += b=%1.3f\n", bias);


            // we want to save the sum, not the value 
            // (in backprop, we don't need the value, but we need the sum)
            if (out_neuronValues != NULL)
                if (out_neuronValues[currentLayerIndex] != NULL)
                    out_neuronValues[currentLayerIndex][outputIndex] = currentSum;
                else
                    fprintf(stderr, "[ERROR] out_neurons is not null, but out_neurons[%d] is!\n", currentLayerIndex);

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
                printf("[VERBOSE] sum = %.3f val = %.3f\n", currentSum, value);

            currentLayerValues[outputIndex] = value;
        }
        
        // calculated layer, time to move on

        memcpy(&prevLayerValues, &currentLayerValues, MAX_LAYER_DIM * sizeof(double));

        // if (out_neuronValues != NULL) {
        //     if (out_neuronValues[currentLayerIndex] != NULL) {
        //         memcpy(out_neuronValues[currentLayerIndex], &currentLayerValues, MAX_LAYER_DIM * sizeof(double));
        //     } else {
        //         printf("[WARNING] out_neuronValues is not null but out_neuronValues[%d] is!\n", currentLayerIndex);
        //     }
        // }
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
        printf("[VERBOSE] original (before softmax)\n");
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
        printf("[VERBOSE] sumexp: %e\n", sumOfLayerExp);

    for (int i = 0; i < NUM_CLASSES; i++) {
        result.probs[i] = exp(result.probs[i]) / sumOfLayerExp;
    }

    if (VERBOSE_PREDICTION) {
        printf("[VERBOSE] after softmax:\n");
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

/// @brief implements cross-entropy algorithm
/// @brief cross-entropy: -sum(y[i] * log(s[i])) for i: 0..NUM_CLASSES
/// @return Cross-entropy cost of the given image with the label
double CalculateCost(uint8_t label, double* resultValues) {

    double cost = 0.0;

    for (uint8_t i = 0; i < NUM_CLASSES; i++) {

        // -sum(y[i] * log(s[i])) = sum(-y[i] * log(s[i]))
        cost -= (i == label ? 1 : 0) * log2l(resultValues[i]);
    }

    return cost;
}

/// @brief Adjusts the model's weights
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
            printf("[VERBOSE] prev output [%d] = %f\n", i, neuronValues[numLayers - 1][i]);
        }

    double cost = CalculateCost (image->label, neuronValues[numLayers - 1]);

    if (VERBOSE_BACKPROP)
        printf("[VERBOSE] cost: %e\n", cost);

    double** derBuffer = MakeValueBufferForModel(model.numLayers);

    // we first differentiate the cost function, store the results in derBuffer
    // then we differentiate the softmax function and update derBuffer
    // note: the softmax derivative should be calculated for every pair!    

    // update neurons' derivatives
    for (int layerIndex = numLayers - 1; layerIndex >= 0; layerIndex--) {

        for (int i = 0; i < model.layers[layerIndex].outputDim; i++) {

            if (layerIndex == numLayers - 1) {

                // differentiate softmax and cross-entropy at the same time

                // https://towardsdatascience.com/derivative-of-the-softmax-function-and-the-categorical-cross-entropy-loss-ffceefc081d1

                // y[i]: i == label ? 1 : 0
                // z: wn + b
                // s: softmax(z)
                // --> dC/dz = s - y

                derBuffer[layerIndex][i] = neuronValues[layerIndex][i] - (i == image->label ? 1 : 0);

                if (VERBOSE_BACKPROP)
                    printf("[VERBOSE] dC/dz=%e (l=%d,i=%d)\n", derBuffer[layerIndex][i], layerIndex, i);

            } else {

                // calculate derivative from previous layer

                const int prevLayerIndex = layerIndex + 1;
                const int prevLayerDim = model.layers[prevLayerIndex].outputDim;
                // const int layerDim = model.layers[layerIndex].outputDim;

                derBuffer[layerIndex][i] = 0;

                for (int j = 0; j < model.layers[prevLayerIndex].outputDim; j++) {
                    
                    // dE/dn[l] = dE/dn[l+1] * dn[l+1]/dn[l]
                    //                         dn[l+1][i]/dn[l][j] = f'(neuronValue[l+1][j]) * layers[l+1].weights[i, j]

                    double delta = derBuffer[prevLayerIndex][j];

                    if (model.layers[layerIndex].activationFunction == SIGMOID)
                        delta *= (
                                SigmoidDer(neuronValues[layerIndex][i]) 
                                * model.layers[prevLayerIndex].weights[i + prevLayerDim * j]
                            );
                    else if (model.layers[layerIndex].activationFunction == RELU)
                        delta *= (
                                ReLUDer(neuronValues[layerIndex][i]) 
                                * model.layers[prevLayerIndex].weights[i + prevLayerDim * j]
                            );
                    else if (model.layers[layerIndex].activationFunction == SOFTMAX) {
                        delta *= (
                                1 // we have already calculated the gradient
                                * model.layers[prevLayerIndex].weights[i + prevLayerDim * j]
                            );
                        fprintf(stderr, "[ERROR] Softmax should not be used outside of the last layer!\n");
                    }
                    else
                        fprintf(stderr, "[ERROR] (%s:%d) Unknown activation function %d\n", __FILE__, __LINE__, model.layers[prevLayerIndex].activationFunction);
                    

                    derBuffer[layerIndex][i] += delta; 
                    // we only calculate the neurons' derivatives
                    // we don't want to take its negative (cuz it will be hella confusing)
                }

                if (VERBOSE_BACKPROP)
                    printf("[VERBOSE] dC/dz=%e (l=%d,i=%d)\n", derBuffer[layerIndex][i], layerIndex, i);
            }
        }
    }

    // neuron derivatives are done, the bulk of the work has been done

    // adjust weights and biases
    for (int layerIndex = numLayers - 1; layerIndex >= 0; layerIndex--) {

        // for every neuron in our layer
        for (int i = 0; i < model.layers[layerIndex].outputDim; i++) {
            
            // adjust weights
            for (int j = 0; j < model.layers[layerIndex].inputDim; j++) {
                
                // dn[l][i]/dw[l][i, j] = f'(neuronValues[l]) * neuronValues[l-1][j]
                // --> dE/dw = dE/dn * dn/dw
            
                double delta = derBuffer[layerIndex][i];

                double prevValue = layerIndex == 0 ? image->data[j] : neuronValues[layerIndex - 1][j];

                // we have already added the f^-1 part in the dC/dz part !!!

                delta *= prevValue;

                // if (model.layers[layerIndex].activationFunction == SIGMOID)
                //     delta *= SigmoidDer(neuronValues[layerIndex][i]) * prevValue;
                // else if (model.layers[layerIndex].activationFunction == RELU)
                //     delta *= ReLUDer(neuronValues[layerIndex][i]) * prevValue;
                // else if (model.layers[layerIndex].activationFunction == SOFTMAX)
                //     delta *= 1 * prevValue; // we have already calculated it
                // else
                //     fprintf(stderr, "[ERROR] Unknown activation function %d\n", model.layers[layerIndex].activationFunction);

                if (VERBOSE_BACKPROP)
                    printf("[VERBOSE] dC/dw=%e (l=%d,i=%d,j=%d)\n", delta, layerIndex, i, j);

                model.layers[layerIndex].weights[j + i * model.layers[layerIndex].inputDim] -= delta * cost * learningRate;
            }

            // adjust bias
            // dn[l][i]/db[l] = f'(neuronValues[l][i])
            // --> dE/db = dE/dn * dn/db

            double delta = derBuffer[layerIndex][i];

            // we have already added the f^-1 part in the dC/dz part !!!


            // if (model.layers[layerIndex].activationFunction == SIGMOID)
            //     delta *= SigmoidDer(neuronValues[layerIndex][i]);
            // else if (model.layers[layerIndex].activationFunction == RELU)
            //     delta *= ReLUDer(neuronValues[layerIndex][i]);
            // else if (model.layers[layerIndex].activationFunction == SOFTMAX)
            //     delta *= 1; // we have already calculated it
            // else
            //     printf("[ERROR] Unknown activation function %d\n", model.layers[layerIndex].activationFunction);
            
            if (VERBOSE_BACKPROP)
                printf("[VERBOSE] dC/db=%e (l=%d,i=%d)\n", delta, layerIndex, i);

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

double CalculateAvgCostForModel (Model model, LabeledImage* images, int numImages) {
    double avgCost = 0.0;
    for (int i = 0; i < numImages; i++) {
        Result res = Predict (model, images[i].data, NULL);
        double cost = CalculateCost (images[i].label, res.probs);
        avgCost += cost / numImages;
    }
    return avgCost;
}

/// @brief argmax
int GetPredictionFromResult(Result result) {

    int maxIndex = 0;
    double maxValue = result.probs[0];

    for (int i = 1; i < NUM_CLASSES; i++) {
        if (result.probs[i] > maxValue) {
            maxIndex = i;
            maxValue = result.probs[i];
        }  
    }

    return maxIndex;
}