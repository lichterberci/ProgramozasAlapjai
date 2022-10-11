#pragma once

#include "stdint.h"

#include "dataset.h"

typedef struct {

    uint32_t inputDim;
    uint32_t outputDim;
    double* weights;
    double* biases;

} Layer;

typedef struct {

    uint8_t numLayers;
    Layer* layers;

} Model;