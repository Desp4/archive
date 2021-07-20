#ifndef PLAYER_EXPORT
#define PLAYER_EXPORT

#ifdef PL_SHARED
    // On shared build
    #ifdef _MSC_VER
        // Microsoft
        #define PL_EXPORT __declspec(dllexport)
        #define PL_IMPORT __declspec(dllimport)
    #elif defined(__GNUC__)
        // GCC
        #define PL_EXPORT __attribute__((visibility("default")))
        #define PL_IMPORT
    #else
        // What are we using?? do nothing
        #define PL_EXPORT
        #define PL_IMPORT
        #pragma warning Unknown dynamic link import/export semantics.
    #endif
#else
    // On static build
    #define PL_EXPORT
    #define PL_IMPORT
#endif
#endif
