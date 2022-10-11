#pragma once

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#define IMAGE_SIZE 784
#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28

#define PRINTING_THRESHOLD 0.0

typedef struct {

    double data[IMAGE_SIZE];
    uint8_t label;

} LabeledImage;

typedef struct {

    uint32_t numData; // numImages == numLabels
    LabeledImage* images;

} Dataset;

int ReadInt (FILE* fp);
uint8_t ReadByte (FILE* fp);
void FreeDataset (Dataset dataset);
double GetPixelOfImage(LabeledImage image, int x, int y);
void PrintLabeledImage (LabeledImage image);
Dataset ReadDatasetFromFile (const char* imagePath, const char* labelPath);