#ifndef __SACI_UTILS_SU_DEBUG_H__
#define __SACI_UTILS_SU_DEBUG_H__

#include "saci-utils/su-types.h"

typedef enum saci_LogLevel {
    SACI_LOG_LEVEL_DEBUG = -1, // Should not be used unless debug enabled
    SACI_LOG_LEVEL_INFO = 0,
    SACI_LOG_LEVEL_WARN = 1,
    SACI_LOG_LEVEL_ERROR = 2,
} saci_LogLevel;

typedef enum saci_ContextLevel {
    SACI_LOG_CONTEXT_OPENGL = 0,
    SACI_LOG_CONTEXT_RENDERER = 1,
} saci_ContextLevel;

#define SACI_LOG_PRINT(level, context, message)                       \
    do {                                                              \
        saci_LogMessage(level, context, message, __FILE__, __LINE__); \
    } while (0)

void saci_LogMessage(int level, int context, const char* message, const char* file, int line);

// OpenGL logger
void saci_OpenGLDebugMessageCallback(saci_u32 source, saci_u32 type, saci_u32 id,
                                     saci_u32 severity, int length,
                                     const char* msg, const void* data);

#endif
