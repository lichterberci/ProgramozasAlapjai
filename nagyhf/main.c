#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "mnist.h"

#define PRINTING_THRESHOLD 0.0

typedef struct {

    uint32_t numData; // numImages == numLabels
    uint32_t numRows;
    uint32_t numCols;
    double* images; // array pixels --> we need helper functions to read normally
    uint8_t* labels;

} Dataset;

typedef struct {

    uint32_t numRows;
    uint32_t numCols;
    double* data;
    uint8_t label;

} LabeledImage;

int ReadInt (FILE* fp) {
    uint8_t buffer[4];
    
    size_t readInts = fread(&buffer, 4, 1, fp);

    if (readInts != 1) {
        fprintf(stderr, "Could not read enough ints from filestream, while reading int! (# read ints: %d)\n", readInts);
        return -1;
    } 

    int result = 0;
    result |= buffer[0] << 24;
    result |= buffer[1] << 16;
    result |= buffer[2] << 8;
    result |= buffer[3];

    return result;
}

uint8_t ReadByte (FILE* fp) {
    uint8_t result;
    int readThings = fread(&result, 1, 1, fp);
    if (readThings != 1)
        fprintf(stderr, "[ERROR] Could not read byte!\n");
    return result;
}

void FreeDataset (Dataset dataset) {
    free (dataset.images);
    free (dataset.labels);
}

LabeledImage GetImageFromDataset (Dataset dataset, int index) {
    LabeledImage result = {
        dataset.numRows,
        dataset.numCols,
        (double*)(dataset.images + (index * (dataset.numRows * dataset.numCols) * sizeof(double))),
        dataset.labels[index]
    };
    return result;
}

double GetPixelOfImage(LabeledImage image, int x, int y) {
    return image.data[x * image.numCols + y];
}

double GetPixelOfImageInDataset (Dataset dataset, int imageIndex, int x, int y) {
    return dataset.images[(uint64_t)imageIndex * (dataset.numRows * dataset.numCols) + x * dataset.numCols + y];
}

void PrintLabeledImage (LabeledImage image) {

    printf("+");
    for (size_t i = 0; i < image.numRows * 4; i++)
        printf("-");
    printf("+\n");
    
    for (size_t i = 0; i < image.numCols; i++) {

        printf("|");

        for (size_t j = 0; j < image.numRows; j++) {

            double val = GetPixelOfImage(image, i, j);
            if (val > PRINTING_THRESHOLD)
                printf("%1.1f ", val);
            else
                printf("    ");
        }

        printf("|\n");
    }

    printf("+");
    for (size_t i = 0; i < image.numRows * 2 - 5; i++)
        printf("-");

    printf(" label: %d ", image.label);

    for (size_t i = 0; i < image.numRows * 2 - 5; i++)
        printf("-");
    printf("+\n");


}

Dataset ReadDatasetFromFile (const char* imagePath, const char* labelPath) {

    FILE* imageFP = fopen(imagePath, "br");
    FILE* labelFP = fopen(labelPath, "br");

    Dataset emptyResult = {
        0, 0, 0, NULL, NULL
    };

    if (imageFP == NULL) {
        fprintf(stderr, "[ERROR] Could not open image file %s\n", imagePath);
        return emptyResult;
    }

    if (labelFP == NULL) {
        fprintf(stderr, "[ERROR] Could not open label file %s\n", labelPath);
        return emptyResult;
    }

    int imageMagicNumber = ReadInt(imageFP);
    int numImages = ReadInt(imageFP);
    int numRows = ReadInt(imageFP);
    int numCols = ReadInt(imageFP);

    int imageSize = numCols * numRows; // 784
    int imageOffset = 4 * sizeof(int); // we start reading the images from here

    int labelMagicNumber = ReadInt(labelFP);
    int numLabels = ReadInt(labelFP);

    if (imageMagicNumber != 2051) {
        fprintf(stdout, "[WARNING] Magic number of image differs from 2051! (%d)\n", imageMagicNumber);
    }

    if (labelMagicNumber != 2049) {
        fprintf(stdout, "[WARNING] Magic number of label differs from 2049! (%d)\n", labelMagicNumber);
    }

    int labelOffset = 2 * sizeof(int); // we start reading the labels from here

    if (numLabels != numImages) {
        fprintf(stderr, "[ERROR] # images (%d) != # labels (%d)\n", numImages, numLabels);
        return emptyResult;
    }

    // allocate memory

    double* data = malloc (numImages * imageSize * sizeof(double));
    uint8_t* labels = malloc (numLabels * sizeof(uint8_t));

    if (data == NULL) {
        fprintf (stderr, "[ERROR] Could not allocate enough memory for [data]!\n");
        return emptyResult;
    }

    if (labels == NULL) {
        fprintf (stderr, "[ERROR] Could not allocate enough memory for [labels]!\n");
        return emptyResult;
    }

    // we read the image by reading it column by column (not row by row)

    printf("[LOG] Loading dataset from [\"%s\"] and [\"%s\"]!\n", imagePath, labelPath);

    uint64_t dataOffset = 0;

    for (size_t i = 0; i < numImages; i++) {

        if (i % 10000 == 0) {
            fprintf(stdout, "[LOG] Loading data... (%5d/%5d)\n", i, numImages);
        }

        for (size_t x = 0; x < numCols; x++) {
            for (size_t y = 0; y < numRows; y++) {
                
                // read pixel

                double val = ReadByte(imageFP) / 255.0;

                data[dataOffset++] = val;
            }
        }

        // read label

        uint8_t label = ReadByte(labelFP);

        labels[i] = label;
        
    }
    
    // we have finished reading this stuff

    fclose(imageFP);
    fclose(labelFP);

    printf("[LOG] Loading of dataset finished!\n");

    Dataset result = {
        numImages,
        numRows,
        numCols,
        data,
        labels
    };

    return result;
}


int main () {

    const char* trainImagePath = "./data/train-images.idx3-ubyte";
    const char* trainLabelPath = "./data/train-labels.idx1-ubyte";
    const char* testImagePath = "./data/t10k-images.idx3-ubyte";
    const char* testLabelPath = "./data/t10k-labels.idx1-ubyte";

    Dataset trainSet = ReadDatasetFromFile(trainImagePath, trainLabelPath);
    Dataset testSet = ReadDatasetFromFile(testImagePath, testLabelPath);

    if (trainSet.numData == 0 || testSet.numData == 0)
        exit(-1);

    const int start = 0;
    const int num = 4;
    const int stride = 1;

    for (uint64_t i = start; i < start + num; i+=stride) {

        printf("i = %d\n", i);
        LabeledImage img = GetImageFromDataset(trainSet, i);
        PrintLabeledImage(img);
        
    }
    
    printf("Code exited safely!");
    return 0;
}