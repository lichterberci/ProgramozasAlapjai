#include "model.h"

#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

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
        printf("%1.1f", result.probs[i]);
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
                "ReLU" : "Unknown"
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
                fprintf(stdout, "[WARNING] There is a mismatch between layer %d's dim (%d) and layer %d's dim (%d)%\n", i - 1, model->layers[i - 1].outputDim, i, layer.inputDim);
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

Result Predict(Model model, double* input) {

    double prevLayerValues[MAX_LAYER_DIM] = {0};
    double currentLayerValues[MAX_LAYER_DIM] = {0};

    // printf("input: %1.3f\n", input[0]);

    // printf("[LOG] Predicting... (numLayers = %d)\n", model.numLayers);

    memcpy(&prevLayerValues, input, IMAGE_SIZE * sizeof(double));

    // printf("prevval: %1.3f\n", prevLayerValues[0]);
    // printf("copy size: %d\n", min(IMAGE_SIZE, model.layers[0].inputDim));

    for (int currentLayerIndex = 0; currentLayerIndex < model.numLayers; currentLayerIndex++) {
        
        Layer currentLayer = model.layers[currentLayerIndex];

        // const char* activationFunctionString = (
        //         currentLayer.activationFunction == SIGMOID ? 
        //         "sigmoid" : currentLayer.activationFunction == RELU ?
        //         "ReLU" : "Unknown"
        //     );

        // printf("[LOG] Layer[%d]: (%d, %d) act.fn=%s\n", 
        //     currentLayerIndex, 
        //     currentLayer.inputDim, 
        //     currentLayer.outputDim,
        //     activationFunctionString
        // );

        int inputDim = currentLayer.inputDim;
        int outputDim = currentLayer.outputDim;

        memset(&currentLayerValues, 0, MAX_LAYER_DIM * sizeof(double));

        for (size_t outputIndex = 0; outputIndex < outputDim; outputIndex++) {

            double currentSum = 0;

            for (size_t inputIndex = 0; inputIndex < inputDim; inputIndex++) {

                double weight = currentLayer.weights[outputIndex * inputDim + inputIndex];
                double neuron = prevLayerValues[inputIndex];
                currentSum += weight * neuron;

                // printf("[LOG] sum += n=%1.3f * w=%1.3f\n", neuron, weight);
            }

            double bias = currentLayer.biases[outputIndex];
            currentSum += bias;

            // printf("[LOG] sum += b=%1.3f\n", bias);

            double value;

            if (currentLayer.activationFunction == SIGMOID)
                value = Sigmoid(currentSum);
            else if (currentLayer.activationFunction == RELU)
                value = ReLU(currentSum);
            else
                fprintf(stderr, "[ERROR] unknown activation function %d!\n", currentLayer.activationFunction);

            // printf("[LOG] sum = %.3f val = %.3f\n", currentSum, value);

            currentLayerValues[outputIndex] = value;
        }
        
        // calculated layer, time to move on

        memcpy(&prevLayerValues, &currentLayerValues, MAX_LAYER_DIM * sizeof(double));
    }

    // result is in the bottom 10 indicies of prevLayerValues

    Result result;

    memcpy(&(result.probs), &prevLayerValues, NUM_CLASSES * sizeof(Result));

    return result;
}