#include "image.h"
#include <string.h>
#include <math.h>

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

void saveImageAsPPM (const Image *image, char *output_filename) {
     FILE *fPtr = fopen(output_filename, "w");

    if (fPtr == NULL) {
        printf("Error opening file!\n");
        return;
    }
    /////////////////////////write a single line with P3 in it 
    fprintf(fPtr, "P3\n");
    unsigned int width = image->width;
    unsigned int height = image->height;
    unsigned int pixelAmount = width * height;


    //By now you have the correct width and height
    fprintf(fPtr, "%u %u\n", width, height);
    fprintf(fPtr, "255\n");

    for (unsigned int i = 0; i < pixelAmount; i++) {
        fprintf(fPtr, "%u %u %u ", (image->pixelIntensityArr)[i], (image->pixelIntensityArr)[i], (image->pixelIntensityArr)[i]);
        fprintf(fPtr,"\n");
    }
 
    fclose(fPtr);
}

/*
Given a string of N characters, the hide_message function encodes as many of these characters (in order, left-to-right) in the image as possible, 
reserving space for the 8-bit ASCII code for the null character. 

If the image can accommodate at most M ASCII codes (where M < N), including the ASCII code for the null character, 
then encode the first M-1 characters of the input string and the null character. 

You may assume that the image contains at least 8 pixels. Return the number of printable characters encoded in the output image, not including the null character.

printable characters is pixelCount / 8
*/
unsigned int hide_message(char *message, char *input_filename, char *output_filename) {

    Image *image = load_image(input_filename);

    if (image == NULL) {
        return 0;
    }

    unsigned int pixelAmount = image->width * image->height;
    unsigned int temp[8];
    int tempBinaryStorageForChar[8];
    int messageCharCount = 0;

    //printf("The pixelAmount / 8 is %u and sizeof message is %lu\n", pixelAmount / 8, sizeof(message));
    if (pixelAmount / 8 < sizeof(message) * 8) {
        pixelAmount -= 8;
        for (unsigned int i = 0; i < 8; i++) {
            unsigned int offset = i + pixelAmount - (pixelAmount % 8);  ///////// offset finds index of last 8 pixels to encode with 0 at the end
            printf("The offset is %u \n", offset);
            printf("The original intensity is %u \n", image->pixelIntensityArr[offset]);
            if ((image->pixelIntensityArr)[offset] % 2 == 1) {        
                (image->pixelIntensityArr)[offset]--;
            }
            printf("The new intensity is %u \n", image->pixelIntensityArr[offset]);

        }
    }

    for (unsigned int i = 0; i < pixelAmount - (pixelAmount % 8); i+=8) {

        char messageChar = message[messageCharCount];  /////////////// one character at a time from message
        messageCharCount++;

        temp[0] = (image->pixelIntensityArr)[i];
        temp[1] = (image->pixelIntensityArr)[i + 1];
        temp[2] = (image->pixelIntensityArr)[i + 2];
        temp[3] = (image->pixelIntensityArr)[i + 3];
        temp[4] = (image->pixelIntensityArr)[i + 4];
        temp[5] = (image->pixelIntensityArr)[i + 5];
        temp[6] = (image->pixelIntensityArr)[i + 6];
        temp[7] = (image->pixelIntensityArr)[i + 7];

        // this loop turns a single char from message into the binary bits
        for (int j = 7; j >= 0; j--) {
            tempBinaryStorageForChar[7 - j] = (messageChar >> j) & 1;  ////////////// this is supposed to store all the individual binary values that make up the char 
        }

        /// This loop does the encoding
        for (int j = 0; j < 8; j++) {
            int oneOrZero = temp[j] % 2;    /////////////this figures out if the last bit is 1 or 0

            if (oneOrZero > tempBinaryStorageForChar[j]) { /////// since these values can only be either 1 or 0, it is self explanatory
                temp[j] -= 1;
            }
            else if (oneOrZero < tempBinaryStorageForChar[j]) {
                temp[j] += 1;
            }
            ////////////////////////////// by this point, each pixel intensity in temp should have the char's binary encoded
        }
        printf("offset is %u\n", i);
        printf("O is %u, A is %u \n", image->pixelIntensityArr[i], temp[0]);
        printf("O is %u, A is %u  \n", image->pixelIntensityArr[i + 1], temp[1]);
        printf("O is %u, A is %u  \n", image->pixelIntensityArr[i + 2], temp[2]);
        printf("O is %u, A is %u  \n", image->pixelIntensityArr[i + 3], temp[3]);
        printf("O is %u, A is %u  \n", image->pixelIntensityArr[i + 4], temp[4]);
        printf("O is %u, A is %u  \n", image->pixelIntensityArr[i + 5], temp[5]);
        printf("O is %u, A is %u  \n", image->pixelIntensityArr[i + 6], temp[6]);
        printf("O is %u, A is %u  \n", image->pixelIntensityArr[i + 7], temp[7]);

        (image->pixelIntensityArr)[i] = temp[0];
        (image->pixelIntensityArr)[i + 1] = temp[1];
        (image->pixelIntensityArr)[i + 2] = temp[2];
        (image->pixelIntensityArr)[i + 3] = temp[3];
        (image->pixelIntensityArr)[i + 4] = temp[4];
        (image->pixelIntensityArr)[i + 5] = temp[5];
        (image->pixelIntensityArr)[i + 6] = temp[6];
        (image->pixelIntensityArr)[i + 7] = temp[7];

    }
    saveImageAsPPM (image, output_filename);
    delete_image(image);
    return messageCharCount;
}

/*
The reveal_message function opens the file of the given filename, extracts the message hidden in the image, 
and returns a new string containing the message. 

Assume that only printable characters (and spaces) are hidden in the message and that the message is terminated with a null character.

*/
char *reveal_message(char *input_filename) {
    Image *image = load_image(input_filename);
    unsigned int pixelAmount = image->width * image->height;

    char *stringBuff = malloc (pixelAmount / 8);
    memset(stringBuff, 0, pixelAmount / 8);

    if (image == NULL) {
        return stringBuff;
    }

    unsigned char temp[8];
    unsigned int charCounter = 0;

    for (unsigned int i = 0; i < pixelAmount - (pixelAmount % 8); i+=8) {
        if ((pixelAmount - i) < 8){
            break;
        }

        temp[0] = (image->pixelIntensityArr)[i];
        temp[1] = (image->pixelIntensityArr)[i + 1];
        temp[2] = (image->pixelIntensityArr)[i + 2];
        temp[3] = (image->pixelIntensityArr)[i + 3];
        temp[4] = (image->pixelIntensityArr)[i + 4];
        temp[5] = (image->pixelIntensityArr)[i + 5];
        temp[6] = (image->pixelIntensityArr)[i + 6];
        temp[7] = (image->pixelIntensityArr)[i + 7];

        printf("//////////////////////////////////////////////////////////////////////////");
        printf("offset is %u\n", i);
        printf("last bit is %u  \n", temp[0] >> 7 & 1);
        printf("last bit is %u  \n", temp[1] >> 7 & 1);
        printf("last bit is %u  \n", temp[2] >> 7 & 1);
        printf("last bit is %u  \n", temp[3] >> 7 & 1);
        printf("last bit is %u  \n", temp[4] >> 7 & 1);
        printf("last bit is %u  \n", temp[5] >> 7 & 1);
        printf("last bit is %u  \n", temp[6] >> 7 & 1);
        printf("last bit is %u  \n", temp[7] >> 7 & 1);
        
        int asciiIndex = 0;
        for (int j = 0; j < 8; j++) {
            unsigned int c = (temp[j] % 2);
            asciiIndex += c * pow(2, 8 - 1 - j);
        }
        stringBuff[charCounter] = asciiIndex;
        charCounter++;
        printf("%c\n", asciiIndex);
    }
    delete_image(image);
    return stringBuff;
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
