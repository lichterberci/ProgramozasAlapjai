#pragma once

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "debugmalloc.h"

#define IMAGE_SIZE 784
// #define IMAGE_SIZE 2
#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28

#define PRINTING_THRESHOLD 0.1

typedef struct {

    double data[IMAGE_SIZE];
    uint8_t label;

} LabeledImage;

typedef struct {

    uint32_t numData; // numImages == numLabels
    LabeledImage* images;

} Dataset;

/// @brief Reads 4 bytes from a file and converts them into a signed int32_t
int ReadInt (FILE* fp);
/// @brief Reads 1 byte from a file
uint8_t ReadByte (FILE* fp);
/// @brief Frees up a dataset (with all its images) 
void FreeDataset (Dataset dataset);
/// @brief Returns the pixel value of an image at the given coordinates
double GetPixelOfImage(LabeledImage image, int x, int y);
/// @brief Prints the given image into stdout
void PrintLabeledImage (LabeledImage image);
/// @brief Reads 2 files (one for the images, one for the labels) and zips them into a single dataset 
Dataset ReadDatasetFromFile (const char* imagePath, const char* labelPath);