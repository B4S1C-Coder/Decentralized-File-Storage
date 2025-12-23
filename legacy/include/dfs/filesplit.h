#pragma once
#include "dfs/dfsexports.h"

extern "C" {
DFS_EXPORT int divideFileIntoChunks_unencrypted(
    const char* inputFileName, const int numChunks, const char* headerToken,
    const int headerSizeInBytes, const char* outputDirPath);
}