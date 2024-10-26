#include "image.h"
#include <string.h>

Image *load_image(char *filename) {    
    FILE *filePtr = fopen(filename, "r");
    if (filePtr == NULL) {
        //fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    Image *imgPtr = malloc(sizeof(Image));       ///////// size is 12 bytes
    imgPtr->filename = malloc(strlen(filename) + 1);    ///////////// allocates memory for file name
    strncpy(imgPtr->filename, filename, strlen(filename));

    char skipP3[5];
    fscanf(filePtr, "%s", skipP3);
    fscanf(filePtr, "%hu %hu", &(imgPtr->width), &(imgPtr->height));
    unsigned short skipIntensity = 0;
    fscanf(filePtr, "%hu", &skipIntensity);

    size_t totalPixelSize = sizeof(unsigned char) * ((imgPtr->width) * (imgPtr->height));

    imgPtr->pixelIntensityArr = malloc(totalPixelSize);   ////////// size of pixel times number of pixels
    memset(imgPtr->pixelIntensityArr, 0, totalPixelSize);
    unsigned short redInt, blueInt, greenInt;
    unsigned int offset = 0;

    while( fscanf(filePtr, "%hu %hu %hu", &redInt, &blueInt, &greenInt) != EOF )
    {
        *(imgPtr->pixelIntensityArr + offset) = (unsigned char)redInt;
                printf("pixel number is %u and the intensity is %u/n", offset, redInt);  ///////////////////////////////////////////// DELETE AFTER TESTING
        offset++;
    }
    fclose(filePtr);
    return imgPtr;
}

void delete_image(Image *image) {
    if (image == NULL) {
        return;
    }
    free(image->filename);
    if (image->pixelIntensityArr != NULL) {
        free(image->pixelIntensityArr);
    } 
    free(image);
}

unsigned short get_image_width(Image *image) {
    return image->width;
}

unsigned short get_image_height(Image *image) {
    return image->height;
}

unsigned char get_image_intensity(Image *image, unsigned int row, unsigned int col) {
    unsigned int offset = (image->width * row) + col;
    return *(image->pixelIntensityArr + offset);
}

unsigned int hide_message(char *message, char *input_filename, char *output_filename) {
    (void)message;
    (void)input_filename;
    (void)output_filename;
    return 0;
}

char *reveal_message(char *input_filename) {
    (void)input_filename;
    return NULL;
}

unsigned int hide_image(char *secret_image_filename, char *input_filename, char *output_filename) {
    (void)secret_image_filename;
    (void)input_filename;
    (void)output_filename;
    return 10;
}

void reveal_image(char *input_filename, char *output_filename) {
    (void)input_filename;
    (void)output_filename;
}
