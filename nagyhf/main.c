#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "mnist.h"

typedef struct {

    uint8_t numData; // numImages == numLabels
    double* images; // array pixels --> we need helper functions to read normally
    uint8_t* labels;

} DataSet;

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
    fread(&result, 1, 1, fp);
    return result;
}

DataSet ReadDataSetFromFile (const char* imagePath, const char* labelPath) {

    FILE* imageFP = fopen(imagePath, "r");
    FILE* labelFP = fopen(labelPath, "r");

    DataSet result = {
        0, NULL, NULL
    };

    if (imageFP == NULL) {
        fprintf(stderr, "[ERROR] Could not open image file %s\n", imagePath);
        return result;
    }

    if (labelFP == NULL) {
        fprintf(stderr, "[ERROR] Could not open label file %s\n", labelPath);
        return result;
    }

    int imageMagicNumber = ReadInt(imageFP);
    int numImages = ReadInt(imageFP);
    int numRows = ReadInt(imageFP);
    int numCols = ReadInt(imageFP);

    int imageSize = numCols * numRows; // 784
    int imageOffset = 4 * sizeof(int); // we start reading the images from here

    int labelMagicNumber = ReadInt(labelFP);
    int numLabels = ReadInt(labelFP);

    int labelOffset = 2 * sizeof(int); // we start reading the labels from here

    if (numLabels != numImages) {
        fprintf(stderr, "[ERROR] # images (%d) != # labels (%d)\n", numImages, numLabels);
        return result;
    }

    // allocate memory

    double** data = malloc (numImages * numCols * numRows * sizeof(double));
    uint8_t* labels = malloc (numLabels * sizeof(uint8_t));

    // we read the image by reading it column by column (not row by row)

    for (size_t i = 0; i < numImages; i++) {
        for (size_t x = 0; x < numCols; x++) {
            for (size_t y = 0; y < numRows; y++) {
                
                // read pixel

                uint64_t pos = imageOffset + i * imageSize + x * numCols + y;
                fseek(imageFP, pos, SEEK_SET);
                double val = ReadByte(imageFP)/255.0;

                uint64_t dataOffset = x * numCols + y;

                data[dataOffset] = val;
            }
        }

        // read label

        fseek(labelFP, labelOffset + i, SEEK_SET);

        uint8_t label = ReadByte(labelFP);

        labels[i] = label;
    }
    
    // we have finished reading this stuff

    fclose(imageFP);
    fclose(labelFP);

    result = {
        numImages,
        data,
        labels
    };

    return result;

}


int main () {

    // read mnist data

    const char* fileName = "./data/train-images.idx3-ubyte";
    const char* labelFileName = "./data/train-labels.idx1-ubyte";


    return 0;
}