#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sodium.h>
#include "dfs/filedecrypt.h"

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

        size_t numberOfNonNullChars = decryptedDataLength - headerSizeInBytes;
        const unsigned char* nullCharPtr = (const unsigned char*)memchr(bufferUnencrypted+headerSizeInBytes, 0, decryptedDataLength-headerSizeInBytes);

        if (nullCharPtr != NULL) {
            numberOfNonNullChars = nullCharPtr - (bufferUnencrypted+headerSizeInBytes);
        }

        fwrite(bufferUnencrypted+headerSizeInBytes, 1,
            numberOfNonNullChars, outputFile);

        free(bufferEncrypted);
        free(bufferUnencrypted);

    }

    fclose(outputFile);
    return 0;
}