#pragma once
#include <sodium.h>
#include "dfs/dfsexports.h"

extern "C" {
DFS_EXPORT int dfs_divideFileIntoChunks_encryptedSecretbox(
    const char* inputFilePath, const char* inputFileName, 
    const int numChunks, const char* headerToken,
    const int headerSizeInBytes, const char* outputDirPath,
    unsigned char key[crypto_secretbox_KEYBYTES],
    unsigned char nonce[crypto_secretbox_NONCEBYTES]);
}