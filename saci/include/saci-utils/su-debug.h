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

enum sa_Log_Type_e {
    sa_LOG_TYPE_DEBUG = -1,
    sa_LOG_TYPE_INFO  = 0,

    sa_LOG_TYPE_WARN  = 1,
    sa_LOG_TYPE_ERROR = 2,
};

enum sa_Log_Severity_e {
    sa_LOG_SEVERITY_LOW = 0,
    sa_LOG_SEVERITY_MEDIUM,
    sa_LOG_SEVERITY_HIGH,
};

enum sa_Log_Context_e {
    sa_LOG_CONTEXT_OPENGL = 0,
    sa_LOG_CONTEXT_RENDERER,
    sa_LOG_CONTEXT_OBJ_LOADING,
    sa_LOG_CONTEXT_STBI,
    sa_LOG_CONTEXT_MEMORY_ALLOCATION,
};

void sa_Log_Should_Print_Origin(saci_Bool enable);

void sa_Log_Error(enum sa_Log_Type_e type, enum sa_Log_Severity_e severity,
                  enum sa_Log_Context_e context,
                  const char* message, const char* file, int line);

void sa_Log_Info(enum sa_Log_Type_e type, enum sa_Log_Context_e context,
                 const char* message, const char* file, int line);

#define sa_LOG_INFO_PRINT_m(type, context, message)              \
    do {                                                         \
        sa_Log_Info(type, context, message, __FILE__, __LINE__); \
    } while (0)

#define sa_LOG_ERROR_PRINT_m(type, severity, context, message)              \
    do {                                                                    \
        sa_Log_Error(type, severity, context, message, __FILE__, __LINE__); \
    } while (0)

void sa_OpenGL_Debug_Message_Callback(saci_u32 source, saci_u32 type, saci_u32 id, saci_u32 severity,
                                      int length, const char* msg, const void* data);

#endif
