/**
 * @file su-debug.h
 * @brief This header defines common debug functions and constants used in the saciCORE
 * and saciLIB.
 *
 * @details
 * This file provides both OpenGL and saciCORE/saciLIB debugging macros and functions
 */

#ifndef __SACI_UTILS_SU_DEBUG_H__
#define __SACI_UTILS_SU_DEBUG_H__

#include "saci-utils/su-types.h"

// TODO add a level and a type log

enum sa_Log_Level {
    sa_LOG_LEVEL_DEBUG = -1, // Should not be used unless debug enabled
    sa_LOG_LEVEL_INFO  = 0,
    sa_LOG_LEVEL_WARN  = 1,
    sa_LOG_LEVEL_ERROR = 2,
};

enum sa_Log_Context {
    sa_LOG_CONTEXT_OPENGL = 0,
    sa_LOG_CONTEXT_RENDERER,
    sa_LOG_CONTEXT_OBJ_LOADING,
    sa_LOG_CONTEXT_STBI,
    sa_LOG_CONTEXT_MEMORY_ALLOCATION,
};

void sa_Log_Message(int level, int context, const char* message, const char* file, int line);

#define sa_LOG_PRINT_m(level, context, message)                      \
    do {                                                             \
        sa_Log_Message(level, context, message, __FILE__, __LINE__); \
    } while (0)

void sa_OpenGL_Debug_Message_Callback(saci_u32 source, saci_u32 type, saci_u32 id, saci_u32 severity,
                                      int length, const char* msg, const void* data);

#endif
