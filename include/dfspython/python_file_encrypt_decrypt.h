#pragma once

#include <sodium.h>
#include "dfs/dfsexports.h"
#include "dfs/fileencrypt.h"
#include "dfs/filedecrypt.h"

// It is a lot easier to work with sqlite in python so for the first prototype, SQLite
// operations would be handled in python (after that in C/C++). This struct is used to
// give whether success or not (via status code) and the generated key and nonce (to store
// them in SQLite). This SQLite db remains on the client machine ONLY.

extern "C" {

struct keyNoncePair_secretbox {
    int statusCode;
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
};

DFS_EXPORT struct keyNoncePair_secretbox perform_dfs_divideFileIntoChunks_encryptedSecretbox(
    const char* inputFilePath, const char* inputFileName, const int numChunks,
    const char* headerToken, const int headerSizeInBytes, const char* outputDirPath);

DFS_EXPORT int perform_dfs_decryptAndReconstructFile_secretbox(
    const struct keyNoncePair_secretbox* usePair, const char* outputFilePath,
    const int headerSizeInBytes, const int numChunks, const char** chunkFilePaths);
    
}