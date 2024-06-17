#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "dfs/filesplit.h"

// Note: The outputDirPath must end with a "/" (for Linux and newer Windows versions)
// and with "\" (for older versions of windows).
int divideFileIntoChunks_unencrypted(
    const char* inputFileName, const int numChunks, const char* headerToken,
    const int headerSizeInBytes, const char* outputDirPath)
{

    FILE* inputFile = fopen(inputFileName, "rb");

    if (!inputFile) {
        printf("[ C DLL: ERROR ] Unable to open %s input file | ERR = %s", inputFileName, strerror(errno));
        return -1;
    }

    // Determine the filesize in bytes
    fseek(inputFile, 0, SEEK_END);
    size_t fileSizeInBytes = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    // Determine the size of each chunk
    size_t bufferSize = fileSizeInBytes/numChunks + headerSizeInBytes;

    char* buffer = (char*)malloc(bufferSize + headerSizeInBytes*sizeof(char));
    // For now assuming that maximum number of chunks is 9,999,999,999 (IGNORE, JUST IGNORE)
    // This however as of now is NOT ENFORCED
    char chunkNumberBuffer[10];
    // 7: _chunk_, 10: chunkNumberBuffer[10] ki len i.e. 10
    char* outputFileName = (char*)malloc(strlen(outputDirPath) + strlen(inputFileName) + 7 + 10);
    strcpy(outputFileName, outputDirPath);
    strcat(outputFileName, inputFileName);

    printf("[ C DLL: INFO ] Operation: %zu chars -> %d chunks of %zu chars | UNENCRYPTED\n",
                    fileSizeInBytes, numChunks, bufferSize);

    int returnStatusCode = 0;

    for (size_t i = 0; i < numChunks; i++) {

        // Convert i to string
        snprintf(chunkNumberBuffer, 10, "%zu", i);

        strcat(outputFileName, "_chunk_");
        strcat(outputFileName, chunkNumberBuffer);

        FILE* outputFile = fopen(outputFileName, "wb");
        
        if (!outputFile) {
            printf("[ C DLL: ERROR ] Unable to open output file(s) for writing chunk\noutputFileName = %s , ERR = %s", outputFileName, strerror(errno));
            returnStatusCode = -1;
            break;
        }

        // Add suffix token to the buffer
        strcpy(buffer, headerToken);
        // Add the chunk data to the buffer
        size_t bytesRead = fread(buffer + headerSizeInBytes, 1, bufferSize, inputFile);

        // Although throughout the testing this warning has never occured, but it is a
        // good idea to have it in case the buffer size is small. If this warning
        // occurs then that means the bytes we read just now, not all of them are going
        // to fit in the buffer. This will lead to either loss of bytes or the starting
        // bytes in the buffer overwritten. This will lead to corrupted chunks and the
        // input file cannot be construted again from these chunks.

        if (bytesRead > bufferSize) {
            printf("[ C DLL: WARN ] Chunk: %zu, Buffer Size: %zu,"
                " Bytes Read: %zu | INSUFFICIENT BUFFER", i, bufferSize, bytesRead);
        }

        // Sometimes there can be unwanted trailling NULL characters when the chunk does
        // not occupy the entire buffer leading to the buffer contaning NULL characters
        // that weren't changed (these NULL chars came when we cleared the buffer)

        size_t bufferSizeWithoutNull = headerSizeInBytes + bytesRead;
        fwrite(buffer, 1, bufferSizeWithoutNull, outputFile);

        fclose(outputFile);

        // Set all the bytes in buffer to zero, effectively clearing it for next use
        memset(buffer, 0, (bufferSize + headerSizeInBytes) * sizeof(char));

        // RESET THE outputFileName buffer so that we can
        // generate the correct filename for the next buffer
        // strcpy(outputFileName, inputFileName);
        strcpy(outputFileName, outputDirPath);
        strcat(outputFileName, inputFileName);
    }

    fclose(inputFile);
    free(buffer);
    free(outputFileName);

    return returnStatusCode;
}

// int main() {
//     divideFileIntoChunks_unencrypted("text1.txt", 5,
//         "<OWNERSHIP-TOKEN>", sizeof("<OWNERSHIP-TOKEN>"),
//         "chunks/");
//     return 0;
// }