/**
 * @file su-debug.h
 * @brief This header defines common debug functions and constants used in the saciCORE and saciLIB.
 *
 * @details
 * This file provides both OpenGL and saciCORE/saciLIB debugging macros and functions
 */

#ifndef __SACI_UTILS_SU_DEBUG_H__
#define __SACI_UTILS_SU_DEBUG_H__

#include "saci-utils/su-types.h"

/**
 * @typedef saci_LogLevel
 * @brief Enum with all of the available log levels.
 */
typedef enum saci_LogLevel {
    SACI_LOG_LEVEL_DEBUG = -1, // Should not be used unless debug enabled
    SACI_LOG_LEVEL_INFO = 0,
    SACI_LOG_LEVEL_WARN = 1,
    SACI_LOG_LEVEL_ERROR = 2,
} saci_LogLevel;

/**
 * @typedef saci_ContextLevel
 * @brief Enum with all of the available context levels.
 */
typedef enum saci_ContextLevel {
    SACI_LOG_CONTEXT_OPENGL = 0,
    SACI_LOG_CONTEXT_RENDERER = 1,
} saci_ContextLevel;

/**
 * @def SACI_LOG_PRINT
 * @brief macro that prints a diagnostic message
 *
 * @param level The level on which the message is created
 * @param level The context on which the message is created
 * @param level The message specifying what happened
 */
#define SACI_LOG_PRINT(level, context, message)                       \
    do {                                                              \
        saci_LogMessage(level, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @brief Prints a formmated diagnostic message
 *
 * @param level The level the message is created.
 * @param context The context the message is created.
 * @param message The message specifying what happened.
 * @param file The file it happened
 * @param line The line it happened
 */
void saci_LogMessage(int level, int context, const char* message, const char* file, int line);

/**
 * @brief OpenGL message logger
 *
 * @note see: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDebugMessageCallback.xhtml
 */
void saci_OpenGLDebugMessageCallback(saci_u32 source, saci_u32 type, saci_u32 id,
                                     saci_u32 severity, int length,
                                     const char* msg, const void* data);

#endif
