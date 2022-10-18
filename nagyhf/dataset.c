#include "dataset.h"

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
        0, NULL
    };

    if (imageFP == NULL) {
        fprintf(stderr, "[ERROR] Could not open image file %s\n", imagePath);
        fclose(imageFP);
        fclose(labelFP);
        return emptyResult;
    }

    if (labelFP == NULL) {
        fprintf(stderr, "[ERROR] Could not open label file %s\n", labelPath);
        fclose(imageFP);
        fclose(labelFP);
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

    if (imageSize != IMAGE_SIZE || numRows != IMAGE_HEIGHT || numCols != IMAGE_WIDTH) {
        fprintf(
            stderr, 
            "[ERROR] Image dimension mismatch! (imageSize = %d, numRows = %d, numCols = %d) " 
            "whereas it should be (%d, %d, %d)\n", 
            imageSize, numRows, numCols,
            IMAGE_SIZE, IMAGE_HEIGHT, IMAGE_WIDTH
        );
        fclose(imageFP);
        fclose(labelFP);
        return emptyResult;
    }

    int labelOffset = 2 * sizeof(int); // we start reading the labels from here

    if (numLabels != numImages) {
        fprintf(stderr, "[ERROR] # images (%d) != # labels (%d)\n", numImages, numLabels);
        fclose(imageFP);
        fclose(labelFP);
        return emptyResult;
    }

    // allocate memory
    LabeledImage* images = calloc(numImages, sizeof(LabeledImage));

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

                image.data[x * IMAGE_HEIGHT + y] = val;
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
