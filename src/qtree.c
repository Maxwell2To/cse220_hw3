#include "qtree.h"
#include "math.h"
QTNode* createNode(unsigned char intensity, unsigned int startRow, unsigned int pixHeight, unsigned int startCol, unsigned int pixWidth);

double getAverageIntensity(Image *image, unsigned int startRow, unsigned int startCol, unsigned int pixHeight, unsigned int pixWidth){
    ////////////////to get average intensity, just make a simple nested for loop that goes pixel by pixel
    //////////////it will tally up all the pixel intensities and calculate average
    //////////to get number of pixels, just multipy height by width
    double numOfPixels = (double) pixHeight * pixWidth;
    double sumIntensity = 0.0;
    for (unsigned int i = startRow; i < pixHeight; i++) {
        for (unsigned int j = startCol; j < pixWidth; j++) {
            sumIntensity += (double) get_image_intensity(image, i, j);
        }
    }
    return sumIntensity / numOfPixels;
}

double calculateRMSE(Image *image, unsigned int startRow, unsigned int startCol, unsigned int pixHeight, unsigned int pixWidth){
    double averageIntensity = getAverageIntensity(image, startRow, startCol, pixHeight, pixWidth);
    double numOfPixels = (double) pixHeight * pixWidth;
    double sumOfSquaredDiff = 0.0;

    for (unsigned int i = startRow; i < pixHeight; i++) {
        for (unsigned int j = startCol; j < pixWidth; j++) {
            sumOfSquaredDiff += pow(((double) get_image_intensity(image, i, j) - averageIntensity), 2);
        }
    }
    return sqrt(sumOfSquaredDiff/numOfPixels);
}

//recursive function 
QTNode *makeQTTree(Image *image, double max_rmse, unsigned int startRow, unsigned int startCol, unsigned int pixHeight, unsigned int pixWidth){
    double averageIntensity = getAverageIntensity(image, startRow, startCol, pixHeight, pixWidth);

    QTNode *temp = createNode((unsigned char)averageIntensity, 0, get_image_height(image), 0, get_image_width(image));

    if(calculateRMSE(image, startRow, startCol, pixHeight, pixWidth) > max_rmse){
        ///you know you have to split
        //Calculate quadrant 1 (child 1)
        //calculate child 1's startRow, StartCol, height, width
        temp->child1 = makeQTTree(image, max_rmse, startRow, startCol, pixHeight / 2, pixWidth / 2);

        if (pixWidth != 1)
            temp->child2 = makeQTTree(image, max_rmse, startRow, startCol + pixWidth / 2, pixHeight / 2, pixWidth / 2);

        if (pixHeight != 1)
            temp->child3 = makeQTTree(image, max_rmse, startRow + pixHeight / 2, startCol, pixHeight / 2, pixWidth / 2);

        if (pixHeight != 1 && pixWidth != 1)
            temp->child4 = makeQTTree(image, max_rmse, startRow + pixHeight / 2, startCol + pixWidth / 2, pixHeight / 2, pixWidth / 2);

        //if your child 2 width is greater than 0, make a QTTree. Otherwise, it will remain as null
        //temp->child2 = makeQTTree(image, temp, max_rmse, childStartRow, childStartCol, childPixHeight, childPixWidth)

        //if your child 3 height is greater than 0, make a QTTree. Otherwise, it will remain as null
        //temp->child3 = makeQTTree(image, temp, max_rmse, childStartRow, childStartCol, childPixHeight, childPixWidth)

        //if your child 4 height is greater than 0, make a QTTree. Otherwise, it will remain as null
        //temp->child4 = makeQTTree(image, temp, max_rmse, childStartRow, childStartCol, childPixHeight, childPixWidth)

        //every child must have startRow, startCol, pixHeight, pixWidth set specifically for them

/*To handle the special case of a single row of pixels, split the row of pixels in half, setting children 3 and 4 to NULL. 
Likewise, to handle the special case of a single column of pixels, split the column in half, setting children 2 and 4 to NULL.
*/


    }
    ///you dont have to split
    return temp;

}

QTNode *create_quadtree(Image *image, double max_rmse) {
    if (image == NULL){
        return NULL;
    }

    QTNode * root = makeQTTree(image, max_rmse, 0, 0, image->height, image->width);
    return root;
}

QTNode *get_child1(QTNode *node) {
    if (node == NULL)
        return 0;
    return node->child1;
}

QTNode *get_child2(QTNode *node) {
    if (node == NULL)
        return 0;
    return node->child2;
}

QTNode *get_child3(QTNode *node) {
    if (node == NULL)
        return 0;
    return node->child3;
}

QTNode *get_child4(QTNode *node) {
    if (node == NULL)
        return 0;
    return node->child4;
}

unsigned char get_node_intensity(QTNode *node) {
    if (node == NULL)
        return 0;
    return node->intensity;
}

void delete_quadtree(QTNode *root) {
    if (root == NULL) {
        return;
    }
    delete_quadtree(root->child1);
    delete_quadtree(root->child2);
    delete_quadtree(root->child3);
    delete_quadtree(root->child4);
    free(root);
}

void set_pixel_intensity(unsigned char **array, unsigned int row, unsigned int col, unsigned char intensity) {
    array[row][col] = intensity;  ////////////////maybe have the bottom function call this function
}

unsigned int getMax(unsigned int a, unsigned int b){
    if (a > b)
        return a;
    return b;
}

unsigned int findMaxWidth(QTNode *root){
    if (root == NULL)
        return 0;
    unsigned int currentNodeWidth = root->pixWidth + root->startCol;

    unsigned int width1 = findMaxWidth(root->child1);    
    unsigned int width2 = findMaxWidth(root->child2);    
    unsigned int width3 = findMaxWidth(root->child3);  
    unsigned int width4 = findMaxWidth(root->child4); 

    return getMax(getMax(getMax(getMax(currentNodeWidth, width1), width2), width3), width4);
}

unsigned int findMaxHeight(QTNode *root){
    if (root == NULL)
        return 0;
    unsigned int currentNodeHeight = root->pixHeight + root->startRow;

    unsigned int height1 = findMaxHeight(root->child1);    
    unsigned int height2 = findMaxHeight(root->child2);    
    unsigned int height3 = findMaxHeight(root->child3);  
    unsigned int height4 = findMaxHeight(root->child4); 

    return getMax(getMax(getMax(getMax(currentNodeHeight, height1), height2), height3), height4);
}

QTNode* createNode(unsigned char intensity, unsigned int startRow, unsigned int pixHeight, unsigned int startCol, unsigned int pixWidth) {
    QTNode *newNode = (QTNode*)malloc(sizeof(QTNode));
    newNode->intensity = intensity;
    newNode->startRow = startRow;
    newNode->pixHeight = pixHeight;
    newNode->startCol = startCol;
    newNode->pixWidth = pixWidth;

    newNode->child1 = NULL;  
    newNode->child2 = NULL;  
    newNode->child3 = NULL;  
    newNode->child4 = NULL;   
    return newNode;
}

QTNode* readEncodedLineAndCreateNode(FILE* fp, char* nodeMode)
{
   unsigned int avgIntensity, startRow, pixHeight, startCol, pixWidth;
    if (fscanf(fp, "%c %u %u %u %u %u\n",
        nodeMode, &avgIntensity, &startRow, &pixHeight, &startCol, &pixWidth)==EOF) {
        return NULL; // fscanf done.
    }
    // fscanf OK

    //printf("%c %u %u %u %u %u\n", *nodeMode, avgIntensity, startRow, pixHeight, startCol, pixWidth); 

    return createNode(avgIntensity, startRow, pixHeight, startCol, pixWidth);
}

QTNode* convertEncodedLinesIntoQTTree(FILE* fp) {
    char nodeMode; // 'N' or 'L'
    QTNode* node = readEncodedLineAndCreateNode(fp, &nodeMode);
    if (node == NULL)
        return NULL; // we done reading lines
    if (nodeMode == 'N') {
        int currentChild = 0;
        // the next line we process will be a child of this node
        // figure out which child.
        while(currentChild < 4){
            switch (currentChild) {
            case 0:
                node->child1 = convertEncodedLinesIntoQTTree(fp);
                //printf("added child1\n");
                break;
            case 1:
                node->child2 = convertEncodedLinesIntoQTTree(fp);
                //printf("added child2\n");
                break;
            case 2:
                node->child3 = convertEncodedLinesIntoQTTree(fp);
                //printf("added child3\n");
                break;
            case 3:
                node->child4 = convertEncodedLinesIntoQTTree(fp);
                //printf("added child4\n");
                break;
            }
            currentChild++;
            if (currentChild > 3) {
                //the next line that we read is no longer the child of the current node.
                return node;
            }
        }
    }
    // else nodeMode == 'L', this is leaf node
    // no child to link with next line we read
    // to the current node
    //printf("adding current line as a leaf\n");
    return node;
}

void traverseAllQTNodesAndSet2DArray(QTNode *root, unsigned char **array) {
    if (root == NULL){
        return;
    }
    
    //set all pixels in 2D array (starting row, starting column)
    unsigned int width = root->pixWidth;
    unsigned int height = root->pixHeight;
    unsigned char intensity = root->intensity;
    
    for (unsigned int i = root->startRow; i < height; i++) {
        for (unsigned int j = root->startCol; j < width; j++) {
            set_pixel_intensity(array, i, j, intensity); 
        }   
    }
    traverseAllQTNodesAndSet2DArray(root->child1, array);
    traverseAllQTNodesAndSet2DArray(root->child2, array);
    traverseAllQTNodesAndSet2DArray(root->child3, array);
    traverseAllQTNodesAndSet2DArray(root->child4, array);
}


void save_qtree_as_ppm(QTNode *root, char *filename) {
    FILE *fPtr = fopen(filename, "w");

    if (fPtr == NULL) {
        printf("Error opening file!\n");
        return;
    }
    /////////////////////////write a single line with P3 in it 
    fprintf(fPtr, "P3\n");
    unsigned int maxWidth = findMaxWidth(root); 
    unsigned int maxHeight = findMaxHeight(root);

    //By now you have the correct width and height
    fprintf(fPtr, "%u %u\n", maxWidth, maxHeight);
    fprintf(fPtr, "255\n");
    // Dynamically allocate memory for the 2D array
    unsigned char **array = (unsigned char **)malloc(maxHeight * sizeof(unsigned char *));
    for (unsigned int i = 0; i < maxHeight; i++) {
        array[i] = (unsigned char *)malloc(maxWidth * sizeof(unsigned char));
    }

    // Initialize the array (optional)
    for (unsigned int i = 0; i < maxHeight; i++) {
        for (unsigned int j = 0; j < maxWidth; j++) {
            array[i][j] = 0;
        }
    }

    traverseAllQTNodesAndSet2DArray(root, array);

    ///////////////////Anything below this is for pushing into ppm file
    for (unsigned int i = 0; i < maxHeight; i++) {
        for (unsigned int j = 0; j < maxWidth; j++) {
            ///////////////////////////////fprintf three times on each pixel
            ///////////////////////////////each pixel is array[i][j]
            fprintf(fPtr, "%u %u %u ", array[i][j], array[i][j], array[i][j]);
        }
        fprintf(fPtr,"\n");
    }
    /////////////////////////////////dont need the 2D array anymore so we free up memory
    for (unsigned int i = 0; i < maxHeight; i++) {
        free(array[i]);
    }
    free(array);
    fclose(fPtr);
}

QTNode *load_preorder_qt(char *filename) {
    FILE *fPtr = fopen(filename, "r");

    if (fPtr == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }
    QTNode *root = convertEncodedLinesIntoQTTree(fPtr);
    fclose(fPtr);
    return root;
}

void convertQTNodesBackToEncoded(QTNode *root, FILE *fPtr) {
    if (root == NULL) {
        return;
    }

    if (root->child1 == NULL && root->child2 == NULL && root->child3 == NULL && root->child4 == NULL) {
        fprintf(fPtr, "L %u %u %u %u %u\n", root->intensity, root->startRow, root->pixHeight, root->startCol, root->pixWidth); 
        return;
    }
    else
        fprintf(fPtr, "N %u %u %u %u %u\n", root->intensity, root->startRow, root->pixHeight, root->startCol, root->pixWidth); 

    convertQTNodesBackToEncoded(root->child1, fPtr);
    convertQTNodesBackToEncoded(root->child2, fPtr);
    convertQTNodesBackToEncoded(root->child3, fPtr);
    convertQTNodesBackToEncoded(root->child4, fPtr);
}

void save_preorder_qt(QTNode *root, char *filename) {
    FILE *fPtr = fopen(filename, "w");

    if (fPtr == NULL) {
        printf("Error opening file!\n");
        return;
    }

    convertQTNodesBackToEncoded(root, fPtr);
    fclose(fPtr);
}

