#ifdef _WIN32
#define DFS_EXPORT __declspec(dllexport)
#else
#define DFS_EXPORT __attribute__((visibility("default")))
#endif