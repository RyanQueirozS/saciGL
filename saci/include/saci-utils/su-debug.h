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

/**
 * @enum sa_Log_Type_e
 * @brief The type of log information
 */
enum sa_Log_Type_e {
    sa_LOG_TYPE_DEBUG = -1,
    sa_LOG_TYPE_INFO = 0,

    sa_LOG_TYPE_WARN = 1,
    sa_LOG_TYPE_ERROR = 2,
};

/**
 * @enum sa_Log_Severity_e
 * @brief The severity of log information
 */
enum sa_Log_Severity_e {
    sa_LOG_SEVERITY_LOW = 0,
    sa_LOG_SEVERITY_MEDIUM,
    sa_LOG_SEVERITY_HIGH,
};

/**
 * @enum sa_Log_Context_e
 * @brief The context of log information
 */
enum sa_Log_Context_e {
    sa_LOG_CONTEXT_OPENGL = 0,
    sa_LOG_CONTEXT_RENDERER,
    sa_LOG_CONTEXT_MODEL_LOADING,
    sa_LOG_CONTEXT_STBI,
    sa_LOG_CONTEXT_MEMORY_ALLOCATION,
};

/**
 * @brief sa_Log_Should_Print_Origin Enables or disables origin printing,
 * default is false, unless in debug mode
 *
 * @param[in] enable The option to enable or disable
 */
void sa_Log_Should_Print_Origin(sa_bool enable);

/**
 * @fn sa_Log_Error
 * @brief Logs an error using saci's print template
 * @note DO NOT USE THIS. There is already a provided macro that reduces param
 * amount
 *
 * @param[in] type The type of the error
 * @param[in] severity The severity of the error
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 * @param[in] file The file the error occoured
 * @param[in] line The line the error occoured
 */
void sa_Log_Error(enum sa_Log_Type_e type, enum sa_Log_Severity_e severity,
                  enum sa_Log_Context_e context,
                  const char* message, const char* file, int line);

/**
 * @fn sa_Log_Info
 * @brief Logs an information using saci's print template
 * @note DO NOT USE THIS. There is already a provided macro that reduces param
 * amount
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 * @param[in] file The file the info comes from
 * @param[in] line The line the info comes from
 */
void sa_Log_Info(enum sa_Log_Type_e type, enum sa_Log_Context_e context,
                 const char* message, const char* file, int line);

/**
 * @define sa_Log_Info_Print_m
 * @brief A macro that prints info, using @ref sa_Log_Info
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#define sa_Log_Info_Print_m(type, context, message)              \
    do {                                                         \
        sa_Log_Info(type, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @define sa_Log_Error_Print_m
 * @brief A macro that prints errors, using @ref sa_Log_Error
 *
 * @param[in] type The type of the error
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 */
#define sa_Log_Error_Print_m(type, severity, context, message)              \
    do {                                                                    \
        sa_Log_Error(type, severity, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @define sa_Log_Assert_Message_m
 * @brief A macro that asserts conditions
 *
 * @param[in] condition The condition
 * @param[in] message The message explaining the error
 */
#define sa_Log_Assert_Message_m(condition, message)                                                  \
    do {                                                                                             \
        if (!(condition)) {                                                                          \
            fprintf(stderr, "Assertion failed at %s:%d\nREASON: %s\n", __func__, __LINE__, message); \
            exit(EXIT_FAILURE);                                                                      \
        }                                                                                            \
    } while (0)

// TODO doc
void sa_Log_OpenGL_Debug_Message_Callback(sa_u32 source, sa_u32 type, sa_u32 id, sa_u32 severity,
                                          int length, const char* msg, const void* data);

#endif
