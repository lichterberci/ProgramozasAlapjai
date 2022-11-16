#include "model.h"

#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define VERBOSE_PREDICTION 0
#define VERBOSE_BACKPROP 0

#define LRELU_ALPHA 0.3

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

double LReLU (double x) {
    return x >= 0 ? x : LRELU_ALPHA * x;
}
double LReLUDer (double x) {
    return x >= 0 ? 1 : LRELU_ALPHA;
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

    if (model.layers != NULL)
        free(model.layers);
}

void FreeLayer(Layer layer) {

    if (layer.weights != NULL)
        free(layer.weights);
    if (layer.biases != NULL)
        free(layer.biases);
}

void PrintModel(Model model) {

    printf("Model:\n");

    if (model.numLayers == 0)
        printf("\tEmpty");

    for (int i = 0; i < model.numLayers; i++) {

        const char* activationFunctionString = (
                model.layers[i].activationFunction == SIGMOID ? 
                "sigmoid" : model.layers[i].activationFunction == RELU ?
                "ReLU" : model.layers[i].activationFunction == LRELU ?
                "LReLU" : model.layers[i].activationFunction == SOFTMAX ?
                "softmax" : "Unknown"
            );

        printf("\tLayer[%d] (%d, %d, %s)\n", 
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

void PrintModelLayout(Model model) {

    printf("Model:\n");

    if (model.numLayers == 0)
        printf("\tEmpty");

    for (int i = 0; i < model.numLayers; i++) {

        const char* activationFunctionString = (
                model.layers[i].activationFunction == SIGMOID ? 
                "sigmoid" : model.layers[i].activationFunction == RELU ?
                "ReLU" : model.layers[i].activationFunction == LRELU ?
                "LReLU" : model.layers[i].activationFunction == SOFTMAX ?
                "softmax" : "Unknown"
            );

        printf("\tLayer[%d] (%d, %d, %s)\n", 
            i, 
            model.layers[i].inputDim, 
            model.layers[i].outputDim, 
            activationFunctionString
        );
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

Model CreateModel(int numHiddenLayers, ...) {

    Model model;
    model.numLayers = numHiddenLayers + 1;
    model.layers = malloc(model.numLayers * sizeof(Layer));

    va_list ap;
    
    va_start(ap, numHiddenLayers); // 2nd param: last arg

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

Model CreateModelFromLayout(LayerLayout* layout) {

    Model model = {0, NULL};
    LayerLayout* currentLayerLayout = layout;

    // calculate numLayers
    while (currentLayerLayout != NULL) {
        model.numLayers++;
        currentLayerLayout = currentLayerLayout->next;
    }

    model.numLayers++; // we have a non-hidden layer at the end

    model.layers = malloc(model.numLayers * sizeof(Layer));

    // populate hidden-layers
    currentLayerLayout = layout;
    int layerIndex = 0;
    uint32_t prevLayerDim = IMAGE_SIZE;
    while (currentLayerLayout != NULL) {
        Layer layer;

        layer.inputDim = prevLayerDim;
        layer.outputDim = currentLayerLayout->numNeurons;
        prevLayerDim = layer.outputDim;

        layer.activationFunction = currentLayerLayout->activationFunction;
        layer.weights = NULL;
        layer.biases = NULL;
        
        model.layers[layerIndex++] = layer;

        currentLayerLayout = currentLayerLayout->next;
    }

    // add last layer (not hidden layer)
    Layer lastLayer;
    lastLayer.inputDim = prevLayerDim;
    lastLayer.outputDim = NUM_CLASSES;
    lastLayer.activationFunction = SOFTMAX;
    lastLayer.weights = NULL;
    lastLayer.biases = NULL;
    model.layers[layerIndex] = lastLayer;

    return model;
}

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
                    "ReLU" : currentLayer.activationFunction == LRELU ?
                    "LReLU" : currentLayer.activationFunction == SOFTMAX ?
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
            if (out_neuronValues != NULL) {
                if (out_neuronValues[currentLayerIndex] != NULL)
                    out_neuronValues[currentLayerIndex][outputIndex] = currentSum;
                else
                    fprintf(stderr, "[ERROR] out_neurons is not null, but out_neurons[%d] is!\n", currentLayerIndex);
            }
            
            double value;

            if (currentLayerIndex < numLayers - 1) {
                if (currentLayer.activationFunction == SIGMOID)
                    value = Sigmoid(currentSum);
                else if (currentLayer.activationFunction == RELU)
                    value = ReLU(currentSum);
                else if (currentLayer.activationFunction == LRELU)
                    value = LReLU(currentSum);
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
    }

    // result is in the bottom 10 indicies of prevLayerValues

    Result result;

    memcpy(&(result.probs), &prevLayerValues, NUM_CLASSES * sizeof(Result));

    // implement stable softmax

    /*
    
    "But it is easy to guard against that by using the identity softmax(x) = softmax(x + c) 
    which holds for any scalar c: Subtracting max(x) from x leaves a vector 
    that has only non-positive entries, ruling out overflow and at least one element that is zero 
    ruling out a vanishing denominator (underflow in some but not all entries is harmless)."
    
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

    sumOfLayerExp += 0.00001; // anti zero --> it should not be a problem anyway, but just in case

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

double CalculateCost(uint8_t label, double* resultValues) {

    double cost = 0.0;

    for (uint8_t i = 0; i < NUM_CLASSES; i++) {

        // -sum(y[i] * log(s[i])) = sum(-y[i] * log(s[i]))
        cost -= (i == label ? 1 : 0) * log2l(resultValues[i] + 0.01); // +0.001 because log(0) is -inf
    }

    return cost;
}

void BackPropagate(Model model, double** neuronValues, LabeledImage* image, double learningRate, double** preallocatedDerBuffer, double* out_cost) {

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

    if (out_cost != NULL)
        *out_cost = cost;

    double** derBuffer;

    if (preallocatedDerBuffer == NULL)
        derBuffer = MakeValueBufferForModel(model.numLayers);
    else
        derBuffer = preallocatedDerBuffer;

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

                for (int j = 0; j < prevLayerDim; j++) {
                    
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
                    else if (model.layers[layerIndex].activationFunction == LRELU)
                        delta *= (
                                LReLUDer(neuronValues[layerIndex][i]) 
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
                
                if (VERBOSE_BACKPROP)
                    printf("[VERBOSE] dC/dw=%e (l=%d,i=%d,j=%d)\n", delta, layerIndex, i, j);

                model.layers[layerIndex].weights[j + i * model.layers[layerIndex].inputDim] -= delta * cost * learningRate;
            }

            // adjust bias
            // dn[l][i]/db[l] = f'(neuronValues[l][i])
            // --> dE/db = dE/dn * dn/db

            double delta = derBuffer[layerIndex][i];

            if (VERBOSE_BACKPROP)
                printf("[VERBOSE] dC/db=%e (l=%d,i=%d)\n", delta, layerIndex, i);

            model.layers[layerIndex].biases[i] -= delta * cost * learningRate;
        }
    }

    if (preallocatedDerBuffer == NULL)
        FreeValueBuffer(model, derBuffer);

    if (VERBOSE_BACKPROP)
        printf("--------------------------------------------------\n");
}

bool FitModelForImage (Model model, LabeledImage* image, double learningRate, double** preallocatedValueBuffer, double** preallocatedDerBuffer, double* out_cost) {

    double** valueBuffer;

    if (preallocatedValueBuffer == NULL)
        valueBuffer = MakeValueBufferForModel(model.numLayers);
    else
        valueBuffer = preallocatedValueBuffer;

    Result result = Predict(model, image->data, valueBuffer);

    BackPropagate(model, valueBuffer, image, learningRate, preallocatedDerBuffer, out_cost);

    if (IsResultOk(result) == false)
        PrintResult(result);

    return IsResultOk(result);
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

bool IsResultOk (Result result) {

    for (int i = 0; i < NUM_CLASSES; i++) {
        if (finite(result.probs[i]) == false) {
            return false;
        }
    }
    return true;
}

void SaveModelToFile (Model model, const char* filePath) {

    // check for .model file extension
    
    if (strlen(filePath) < strlen(".model")) {
        // filePath too short
        fprintf(stderr, "[ERROR] Path [\"%s\"] is too short!", filePath);
        exit(-1);
    }

    if (strcmp(filePath + strlen(filePath) - strlen(".model"), ".model") != 0) {
        // incorrect file extension
        fprintf(stderr, "[ERROR] Path [\"%s\"] has incorrect file extension! Should be .model!\n", filePath );
        exit(-1);
    }

    FILE* fp = fopen(filePath, "wb");

    printf("[LOG] Saving model to [\"%s\"]...\n", filePath);

    if (fp == NULL) {
        fprintf(stderr, "[ERROR] Cannot open file %s\n", filePath);
        return;
    }

    //numLayers

    fwrite(&model.numLayers, sizeof(uint8_t), 1, fp);

    // wandb

    for (uint8_t i = 0; i < model.numLayers; i++) {

        // layer header

        fwrite(&(model.layers[i].inputDim), sizeof(uint32_t), 1, fp);
        fwrite(&(model.layers[i].outputDim), sizeof(uint32_t), 1, fp);

        // activation function

        fwrite(&(model.layers[i].activationFunction), sizeof(model.layers[i].activationFunction), 1, fp);

        // weights

        const int numWeights = model.layers[i].inputDim * model.layers[i].outputDim;
        fwrite(model.layers[i].weights, sizeof(double), numWeights, fp);

        // biases

        const int numBiases = model.layers[i].outputDim;
        fwrite(model.layers[i].biases, sizeof(double), numBiases, fp);
    }

    printf("\033[A\33[2K\r");
    printf("[LOG] Model successfully saved to [\"%s\"]!\n", filePath);

    fclose(fp);
}

Model LoadModelFromFile (const char* filePath) {

    // check for .model file extension

    if (strlen(filePath) < strlen(".model")) {
        // filePath too short
        fprintf(stderr, "[ERROR] Path [\"%s\"] is too short!", filePath);
        exit(-1);
    }

    if (strcmp(filePath + strlen(filePath) - strlen(".model"), ".model") != 0) {
        // incorrect file extension
        fprintf(stderr, "[ERROR] Path [\"%s\"] has incorrect file extension! Should be .model!\n", filePath );
        exit(-1);
    }

    Model model = {0, NULL};

    FILE* fp = fopen(filePath, "rb");

    printf("[LOG] Loading model from [\"%s\"]...\n", filePath);

    if (fp == NULL) {
        fprintf(stderr, "[ERROR] Cannot open file %s\n", filePath);
        return model;
    }

    //numLayers

    fread(&model.numLayers, sizeof(uint8_t), 1, fp);

    model.layers = malloc(model.numLayers * sizeof(Layer));

    // wandb

    for (int i = 0; i < model.numLayers; i++) {

        // layer header

        fread(&(model.layers[i].inputDim), sizeof(uint32_t), 1, fp);
        fread(&(model.layers[i].outputDim), sizeof(uint32_t), 1, fp);

        // activation function

        fread(&(model.layers[i].activationFunction), sizeof(ActivationFunction), 1, fp);

        // weights

        const int numWeights = model.layers[i].inputDim * model.layers[i].outputDim;
        
        model.layers[i].weights = malloc(numWeights * sizeof(double));

        fread(model.layers[i].weights, sizeof(double), numWeights, fp);


        // biases

        const int numBiases = model.layers[i].outputDim;
        
        model.layers[i].biases = malloc(numWeights * sizeof(double));

        fread(model.layers[i].biases, sizeof(double), numBiases, fp);
    }

    printf("\033[A\33[2K\r");
    printf("[LOG] Model successfully loaded from [\"%s\"]!\n", filePath);

    fclose(fp);

    return model;
}

