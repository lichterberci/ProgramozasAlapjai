#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "mnist.h"

#define PRINTING_THRESHOLD 0.0

#define IMAGE_SIZE 784
#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28

typedef struct {

    uint32_t numData; // numImages == numLabels
    LabeledImage* images;

} Dataset;

typedef struct {

    double data[IMAGE_SIZE];
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
    for (size_t i = 0; i < dataset.numData; i++) {
        free(dataset.images[i].data);
    }

    free(dataset.images);        
}

double GetPixelOfImage(LabeledImage image, int x, int y) {
    return image.data[x * IMAGE_HEIGHT + y];
}

void PrintLabeledImage (LabeledImage image) {

    printf("+");
    for (size_t i = 0; i < IMAGE_HEIGHT * 4; i++)
        printf("-");
    printf("+\n");
    
    for (size_t i = 0; i < IMAGE_WIDTH; i++) {

        printf("|");

        for (size_t j = 0; j < IMAGE_HEIGHT; j++) {

            double val = GetPixelOfImage(image, i, j);
            if (val > PRINTING_THRESHOLD)
                printf("%1.1f ", val);
            else
                printf("    ");
        }

        printf("|\n");
    }

    printf("+");
    for (size_t i = 0; i < IMAGE_HEIGHT * 2 - 5; i++)
        printf("-");

    printf(" label: %d ", image.label);

    for (size_t i = 0; i < IMAGE_HEIGHT * 2 - 5; i++)
        printf("-");
    printf("+\n");


}

Dataset ReadDatasetFromFile (const char* imagePath, const char* labelPath) {

    FILE* imageFP = fopen(imagePath, "rb");
    FILE* labelFP = fopen(labelPath, "rb");

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
    LabeledImage* images = calloc(numImages, sizeof(LabeledImage));

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

    for (size_t i = 0; i < numImages; i++) {

        if (i % 10000 == 0) {
            fprintf(stdout, "[LOG] Loading data... (%5d/%5d)\n", i, numImages);
        }

        LabeledImage image;

        for (size_t x = 0; x < numCols; x++) {
            for (size_t y = 0; y < numRows; y++) {
                
                // read pixel

                double val = ReadByte(imageFP) / 255.0;

                data[x * IMAGE_HEIGHT + y] = val;
            }
        }

        // read label

        uint8_t label = ReadByte(labelFP);

        image.label = label;
        
        images[i] = image;

    }

    // check if we have really read all data in the file

    uint64_t currentImagePos = ftell(imageFP);
    fseek(imageFP, 0, SEEK_END);
    uint64_t imageFileSize = ftell(imageFP);

    uint64_t currentLabelPos = ftell(labelFP);
    fseek(labelFP, 0, SEEK_END);
    uint64_t labelFileSize = ftell(labelFP);
    

    if (currentImagePos != imageFileSize) {
        fprintf(stderr, "[WARNING] Did not read the whole file! (ftell=%d, size=%d) [%s]\n", currentImagePos, imageFileSize, imagePath);
    }

    if (currentLabelPos != labelFileSize) {
        fprintf(stderr, "[WARNING] Did not read the whole file! (ftell=%d, size=%d) [%s]\n", currentLabelPos, labelFileSize, labelPath);
    }

    // we have finished reading this stuff

    fclose(imageFP);
    fclose(labelFP);

    printf("[LOG] Loading of dataset finished!\n");

    Dataset result = {
        numImages,
        images
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
        LabeledImage img = trainSet.images[i];
        PrintLabeledImage(img);
    }
    
    printf("Code exited safely!");
    return 0;
}