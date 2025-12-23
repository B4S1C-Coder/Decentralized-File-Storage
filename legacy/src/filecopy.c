#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dfs/filecopy.h"

int copyContentsOfFile(const char* inputFileName, const char* outputFileName) {
    FILE* inputFile = fopen(inputFileName, "rb");

    if (!inputFile) {
        printf("Unable to open input file.");
        return -1;
    }

    // Getting the length/size of the input file
    // Move cursor to the end of the file
    fseek(inputFile, 0, SEEK_END);
    // Current position of the cursor in the file will give the uptil that point
    long fileLength = ftell(inputFile);
    // Move cursor back to start of the file
    fseek(inputFile, 0, SEEK_SET);

    char* buffer = (char*)malloc(fileLength*sizeof(char));

    printf("[C DLL] Reading %ld characters ...\n", fileLength);
    fread(buffer, 1, fileLength, inputFile);

    fclose(inputFile);

    FILE* outputFile = fopen(outputFileName, "wb");

    int returnStatusCode = 0;

    if (outputFile) {
        fwrite(buffer, 1, fileLength, outputFile);
        fclose(outputFile);
    } else {
        printf("Unable to open output file.");
        returnStatusCode = -1;
    }

    free(buffer);
    return returnStatusCode;
}