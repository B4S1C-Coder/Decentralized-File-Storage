#pragma once
#include "dfs/dfsexports.h"

extern "C" {
DFS_EXPORT int copyContentsOfFile(
    const char* inputFileName, const char* outputFileName);
}