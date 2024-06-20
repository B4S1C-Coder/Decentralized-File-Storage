#pragma once

#include <sodium.h>
#include "dfs/dfsexports.h"
#include "dfs/fileencrypt.h"
#include "dfs/filedecrypt.h"

extern "C" {

struct keyNoncePair_secretbox {
    int statusCode;
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
};

struct keyNoncePair_secretbox perform_dfs_divideFileIntoChunks_encryptedSecretbox(
    const char* inputFilePath, const char* inputFileName, const int numChunks,
    const char* headerToken, const int headerSizeInBytes, const char* outputDirPath);

int perform_dfs_decryptAndReconstructFile_secretbox(
    struct keyNoncePair_secretbox usePair, const char* outputFilePath, const int headerSizeInBytes,
    const int numChunks, const char** chunkFilePaths);
    
}