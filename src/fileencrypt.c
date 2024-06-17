#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sodium.h>

// TO-DO: Implement the function using stream encryption via
// crypto_secretstream_xchacha20poly1305_<> which is better suited for this purpose

// Below function NOT tested yet
int dfs_divideFileIntoChunks_encryptedSecretbox(
    const char* inputFilePath, const char* inputFileName, 
    const int numChunks, const char* headerToken,
    const int headerSizeInBytes, const char* outputDirPath,
    unsigned char key[crypto_secretbox_KEYBYTES],
    unsigned char nonce[crypto_secretbox_NONCEBYTES]
)
{
    FILE* inputFile = fopen(inputFilePath, "rb");

    if (!inputFile) {
        printf("[ DFS-CORE ] ERROR: %s, Unable to open: %s\n", strerror(errno), inputFilePath);
    }

    // Determine file size
    fseek(inputFile, 0, SEEK_END);
    size_t fileSizeInBytes = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    // Each bufferUnencrypted essentially stores the data of one chunk in the format:
    // <HEADER-TOKEN><DATA-OF-CHUNK> ]---> This will get encrypted
    size_t bytesToReadPerChunk = fileSizeInBytes / numChunks;
    unsigned char* bufferUnencrypted = (unsigned char*)malloc(bytesToReadPerChunk+headerSizeInBytes);

    // By the below format you can have a maximum of 99,999,999,999,999,999,999 chunks
    // This is obviously absurd and since (on 64 bit) size_t can be of 20 digits. 
    // outputFilePath = outputDirPath+inputfilename.LOOPING_VAR.dat\0
    size_t outputFilePathBufferSize = strlen(outputDirPath)+strlen(inputFileName)+1+20+1+3+1;

    for (size_t i = 0; i < numChunks; i++) {
        
        char* outputFilePath = (char*)malloc(outputFilePathBufferSize);
        // Constructing the output file path
        snprintf(outputFilePath, outputFilePathBufferSize, "%s%s.%zu%s",
            outputDirPath, inputFileName, i, "dat");

        // Check if output file can be opened, if not then abort the operation
        FILE* outputFile = fopen(outputFilePath, 'wb');

        if (!outputFile) {
            printf("[ DFS-CORE ] ERROR: %s, Unable to open: %s\n", strerror(errno), outputFilePath);
            return -1;
        }

        // Add the header token to the buffer
        strcpy(bufferUnencrypted, headerToken);
        size_t bytesRead = fread(bufferUnencrypted+headerSizeInBytes, 1, bytesToReadPerChunk, inputFile);

        // Encrypt the buffer, it will be larger than bufferUnencrypted
        unsigned char* bufferEncrypted = (unsigned char*)malloc(bytesToReadPerChunk+headerSizeInBytes+crypto_secretbox_MACBYTES);
        
        int encryptionResult = crypto_secretbox_easy(bufferEncrypted, bufferUnencrypted,
            bytesToReadPerChunk+headerSizeInBytes, nonce, key);

        if (encryptionResult == -1) {
            printf("[ DFS-CORE ] ERROR: Some error in encrypting the file. Aborting ...\n");
            free(bufferEncrypted);
            free(bufferUnencrypted);
            fcloseall();
            return -1;
        }

        fwrite(bufferEncrypted, 1, sizeof(bufferEncrypted), outputFile);
        fclose(outputFile);

        // RESET bufferUnencrypted
        memset(bufferUnencrypted, 0, bytesToReadPerChunk+headerSizeInBytes);
        free(bufferEncrypted);
        free(outputFilePath);
    }

    fclose(inputFile);
    free(bufferUnencrypted);

    return 0;
}