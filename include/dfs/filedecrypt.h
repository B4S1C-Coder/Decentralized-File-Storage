#pragma once
#include <sodium.h>
#include "dfs/dfsexports.h"

extern "C" {
DFS_EXPORT int dfs_decryptAndReconstructFile_secretbox(
    const char* outputFilePath, const int headerSizeInBytes,
    const int numChunks, const char** chunkFilePaths,
    const unsigned char key[crypto_secretbox_KEYBYTES],
    const unsigned char nonce[crypto_secretbox_NONCEBYTES]);
}