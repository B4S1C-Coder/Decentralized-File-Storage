#pragma once

#ifdef _WIN32
    #ifdef EXPORT_DYNAMIC
        #define DFS_EXPORT __declspec(dllexport)
    #else
        #define DFS_EXPORT
    #endif
#else
    #ifdef EXPORT_DYNAMIC
        #define DFS_EXPORT __attribute__((visibility("default")))
    #else
        #define DFS_EXPORT
    #endif
#endif