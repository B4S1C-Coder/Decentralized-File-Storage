#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sodium.h>

int dfs_decryptAndReconstructFile_secretbox(
    const char* outputFilePath, const int headerSizeInBytes,
    const int numChunks, const char** chunkFilePaths,
    const unsigned char key[crypto_secretbox_KEYBYTES],
    const unsigned char nonce[crypto_secretbox_NONCEBYTES]
)
{
    FILE* outputFile = fopen(outputFilePath, "ab");

    if (!outputFile) {
        printf("[ DFS-CORE ] ERROR: %s, Unable to open: %s\n", strerror(errno), outputFilePath);
        return -1;
    }

    for (int i = 0; i < numChunks; i++) {
        FILE* chunkFile = fopen(chunkFilePaths[i], "rb");

        if (!chunkFile) {
            printf("[ DFS-CORE ] ERROR: %s, Unable to open: %s\n", strerror(errno), chunkFilePaths[i]);
            fclose(outputFile);
            return -1;
        }

        fseek(chunkFile, 0, SEEK_END);
        size_t chunkSizeInBytes = ftell(chunkFile);
        fseek(chunkFile, 0, SEEK_SET);

        size_t decryptedDataLength;

        if (chunkSizeInBytes >= crypto_secretbox_MACBYTES) {
            decryptedDataLength = chunkSizeInBytes - crypto_secretbox_MACBYTES;
        } else {
            printf("[ DFS-CORE ] ERROR: The chunk is smaller than Message Authentication Code."
                " This is usually not possible. The chunks might be corrupted or forged. File "
                "under consideration: %s\n", chunkFilePaths[i]);
            fcloseall();
            return -1;
        }

        unsigned char* bufferEncrypted = (unsigned char*)malloc(chunkSizeInBytes);
        unsigned char* bufferUnencrypted = (unsigned char*)malloc(decryptedDataLength);

        size_t bytesRead = fread(bufferEncrypted, 1, chunkSizeInBytes, chunkFile);
        fclose(chunkFile);

        if(crypto_secretbox_open_easy(bufferUnencrypted, bufferEncrypted, chunkSizeInBytes, nonce, key) != 0) {
            printf("[ DFS-CORE ] ERROR: Unable to decrypt. The chunk is either forged or you have"
                " the wrong key or nonce. File under consideration: %s\n", chunkFilePaths[i]);
            fcloseall();
            return -1;
        }

        // size_t numberOfTrailingNullChars = chunkSizeInBytes - bytesRead;
        // size_t numberOfTrailingNullChars = 0;
        // unsigned char* nullPtr = (unsigned char*)memchr(bufferUnencrypted + decryptedDataLength - headerSizeInBytes, 0, headerSizeInBytes);
        // if (nullPtr != NULL) {
        //     numberOfTrailingNullChars = (bufferUnencrypted + decryptedDataLength) - (nullPtr + 1);
        //     printf("[ DEBUG ] [%d] nullPtr = %c\n", i, nullPtr);
        // }

        size_t numberOfNonNullChars = decryptedDataLength - headerSizeInBytes;
        const unsigned char* nullCharPtr = (const unsigned char*)memchr(bufferUnencrypted+headerSizeInBytes, 0, decryptedDataLength-headerSizeInBytes);

        if (nullCharPtr != NULL) {
            printf("[ DEBUG ] [%d] entered nullCharPtr %p\n", i, nullCharPtr);
            numberOfNonNullChars = nullCharPtr - (bufferUnencrypted+headerSizeInBytes);
        }

        printf("[ DEBUG ] [%d] Number of non null characters = %zu\n", i, numberOfNonNullChars);

        // printf("[ DEBUG ] [%d] Number of trailing NULL characters = %zu\n", i, numberOfTrailingNullChars);

        // fwrite(bufferUnencrypted+headerSizeInBytes, 1,
        //     decryptedDataLength - headerSizeInBytes - numberOfTrailingNullChars, outputFile);

        fwrite(bufferUnencrypted+headerSizeInBytes, 1,
            numberOfNonNullChars, outputFile);

        printf("[ DEBUG ] [%d] Length written to file = %zu\n", i,
            numberOfNonNullChars - headerSizeInBytes);

        free(bufferEncrypted);
        free(bufferUnencrypted);

    }

    fclose(outputFile);
    return 0;
}