// This file contains wrapper around functions in fileencrypt.c and filedecrypt.c
// The wrappers defined here are so that we can easily use the above mentioned
// functions in python. (All these functions will be exported by a DLL / SO file)
// and python can import them via the ctypes module (CDLL)

#include <sodium.h>
#include "dfs/fileencrypt.h"
#include "dfs/filedecrypt.h"
#include "dfspython/python_file_encrypt_decrypt.h"

// It is a lot easier to work with sqlite in python so for the first prototype, SQLite
// operations would be handled in python (after that in C/C++). This struct is used to
// give whether success or not (via status code) and the generated key and nonce (to store
// them in SQLite). This SQLite db remains on the client machine ONLY.

struct keyNoncePair_secretbox {
    int statusCode;
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
};

struct keyNoncePair_secretbox perform_dfs_divideFileIntoChunks_encryptedSecretbox(
    const char* inputFilePath, const char* inputFileName, const int numChunks,
    const char* headerToken, const int headerSizeInBytes, const char* outputDirPath)
{
    struct keyNoncePair_secretbox resultantPair;

    // Initialize sodium
    if (sodium_init() < 0) {
        printf("Couldn't initialize sodium.\n");
        resultantPair.statusCode = -1;
        return resultantPair;
    }

    // Generate key and nonce
    crypto_secretbox_keygen(resultantPair.key);
    randombytes_buf(resultantPair.nonce, crypto_secretbox_NONCEBYTES);

    // Call the actual function
    resultantPair.statusCode = dfs_divideFileIntoChunks_encryptedSecretbox(
        inputFilePath, inputFileName, numChunks, headerToken, headerSizeInBytes,
        outputDirPath, key, nonce);

    return resultantPair;
}

int perform_dfs_decryptAndReconstructFile_secretbox(
    struct keyNoncePair_secretbox usePair, const char* outputFilePath, const int headerSizeInBytes,
    const int numChunks, const char** chunkFilePaths)
{
    // Initialize sodium
    if (sodium_init() < 0) {
        printf("Couldn't initialize sodium.\n");
        return -1;
    }

    return dfs_decryptAndReconstructFile_secretbox(outputFilePath, headerSizeInBytes,
        numChunks, chunkFilePaths, usePair.key, usePair.nonce);
}