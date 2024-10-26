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
    memset(skipP3, 0, sizeof(skipP3));
    fgets(skipP3, sizeof(skipP3), filePtr);
    //printf("skipP3 is %s", skipP3);

    int checkCommentSection = 1;
    while (checkCommentSection) {
        char ch;
        ch = fgetc(filePtr);
        if (ch != EOF) {
            //printf("You got in the first character, it is [%c]\n", ch);
            if (ch == '#') {
                //printf("You hit a comment\n");
                char buffer[256];
                if (fscanf(filePtr, "%[^\n]%*c", buffer) == EOF) /////////////////////skips entire line
                    return NULL;
            }
            else {
                fseek(filePtr, -1L, SEEK_CUR);
                checkCommentSection = 0;
            }
        }
        else {
            return NULL;
        }
    }

    fscanf(filePtr, "%hu %hu", &(imgPtr->width), &(imgPtr->height));
    unsigned short skipIntensity = 0;
    fscanf(filePtr, "%hu", &skipIntensity);
    //printf("width is %hu and height is %hu and the intensity is %hu\n", imgPtr->width, imgPtr->height, skipIntensity);

    size_t totalPixelSize = sizeof(unsigned char) * ((imgPtr->width) * (imgPtr->height));

    imgPtr->pixelIntensityArr = malloc(totalPixelSize);   ////////// size of pixel times number of pixels
    memset(imgPtr->pixelIntensityArr, 0, totalPixelSize);
    unsigned short redInt, blueInt, greenInt;
    unsigned int offset = 0;

    while( fscanf(filePtr, "%hu %hu %hu", &redInt, &blueInt, &greenInt) != EOF )
    {
        *(imgPtr->pixelIntensityArr + offset) = (unsigned char)redInt;
                //printf("pixel number is %u and the intensity is %u\n", offset, redInt);  ///////////////////////////////////////////// DELETE AFTER TESTING
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
