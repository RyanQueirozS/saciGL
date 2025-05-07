// TODO doc
/**
 * @file su-debug.h
 * @brief This header defines common debug functions and constants used in the saciCORE
 * and saciLIB.
 *
 * @details
 * This file provides both OpenGL and saciCORE/saciLIB debugging macros and functions
 */

#ifndef SACI_UTILS_SU_DEBUG_H
#define SACI_UTILS_SU_DEBUG_H

#include "saci-utils/su-types.h"

/**
 * @enum sa_logSeverity
 * @brief The severity of log information
 */
enum sa_logSeverity {
    sa_LOG_SEVERITY_LOW = 0,
    sa_LOG_SEVERITY_MEDIUM,
    sa_LOG_SEVERITY_HIGH,
};

/**
 * @enum sa_logContext
 * @brief The context of log information
 */
enum sa_logContext {
    sa_LOG_CONTEXT_OPENGL = 0,
    sa_LOG_CONTEXT_RENDERER,
    sa_LOG_CONTEXT_MODEL_LOADING,
    sa_LOG_CONTEXT_STBI,
    sa_LOG_CONTEXT_MEMORY_ALLOCATION,
    sa_LOG_CONTEXT_CONFIG,
};

enum sa_logDebugType {
    sa_LOG_DEBUG_TYPE_WINDOWING,
    sa_LOG_DEBUG_TYPE_TEXTURE,
    sa_LOG_DEBUG_TYPE_MODEL,
    sa_LOG_DEBUG_TYPE_OPENGL,
    sa_LOG_DEBUG_TYPE_RENDERER,
    sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
    sa_LOG_DEBUG_TYPE_RENDERER_BATCH,
    sa_LOG_DEBUG_TYPE_RENDERER_CALL,
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
 * @param[in] severity The severity of the error
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 * @param[in] file The file the error occoured
 * @param[in] line The line the error occoured
 */
void sa_Log_Error(enum sa_logSeverity severity,
                  enum sa_logContext context,
                  const char* message, const char* file, int line);

/**
 * @fn sa_Log_Warn
 * @brief Logs a warning using saci's print template
 * @note DO NOT USE THIS. There is already a provided macro that reduces param
 * amount
 *
 * @param[in] severity The severity of the error
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 * @param[in] file The file the error occoured
 * @param[in] line The line the error occoured
 */
void sa_Log_Warn(enum sa_logSeverity severity,
                 enum sa_logContext context,
                 const char* message, const char* file, int line);

/**
 * @fn sa_Log_Info
 * @brief Logs an information using saci's print template
 * @note DO NOT USE THIS. There is already a provided macro that reduces param
 * amount
 *
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 * @param[in] file The file the info comes from
 * @param[in] line The line the info comes from
 */
void sa_Log_Info(enum sa_logContext context,
                 const char* message, const char* file, int line);

/**
 * @fn sa_Log_Debug
 * @brief Logs a debug information using saci's print template
 * @note DO NOT USE THIS. There is already a provided macro that reduces param
 * amount
 *
 * @param[in] type The type of the debug
 * @param[in] context The context of the debug
 * @param[in] message The message explaining the debug
 * @param[in] file The file the debug comes from
 * @param[in] line The line the debug comes from
 */
void sa_Log_Debug(enum sa_logDebugType type, enum sa_logContext context,
                  const char* message, const char* file, int line);

/**
 * @define sa_Log_Info_Print_m
 * @brief A macro that prints info, using @ref sa_Log_Info
 *
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#define sa_Log_Info_Print_m(context, message)              \
    do {                                                   \
        sa_Log_Info(context, message, __FILE__, __LINE__); \
    } while (0)

#define sa_Log_InfoF_Print_m(context, fmt, ...)                                   \
    do {                                                                          \
        char _sa_log_info_buf[2048];                                              \
        snprintf(_sa_log_info_buf, sizeof(_sa_log_info_buf), fmt, ##__VA_ARGS__); \
        sa_Log_Info_Print_m(context, _sa_log_info_buf);                           \
    } while (0)

/**
 * @define sa_Log_Debug_Print_m
 * @brief A macro that prints debug info, using @ref sa_Log_Debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_WINDOWING) ||   \
    defined(SACI_DEBUG_MODE_TEXTURE) || defined(SACI_DEBUG_MODE_MODEL) ||   \
    defined(SACI_DEBUG_MODE_OPENGL) || defined(SACI_DEBUG_MODE_RENDERER) || \
    defined(SACI_DEBUG_MODE_RENDERER_FUNCTIONS) ||                          \
    defined(SACI_DEBUG_MODE_RENDERER_BATCH) ||                              \
    defined(SACI_DEBUG_MODE_RENDERER_CALL)

#  define sa_Log_Debug_Print_m(debug_type, context, message)              \
      do {                                                                \
          sa_Log_Debug(debug_type, context, message, __FILE__, __LINE__); \
      } while (0)

#else
#  define sa_Log_Debug_Print_m(debug_type, context, message)
#endif

/**
 * @define sa_Log_DebugF_Print_m
 * @brief A macro that prints formatted debug info, using @ref sa_Log_Debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_WINDOWING) ||   \
    defined(SACI_DEBUG_MODE_TEXTURE) || defined(SACI_DEBUG_MODE_MODEL) ||   \
    defined(SACI_DEBUG_MODE_OPENGL) || defined(SACI_DEBUG_MODE_RENDERER) || \
    defined(SACI_DEBUG_MODE_RENDERER_FUNCTIONS) ||                          \
    defined(SACI_DEBUG_MODE_RENDERER_BATCH) ||                              \
    defined(SACI_DEBUG_MODE_RENDERER_CALL)
#  define sa_Log_DebugF_Print_m(debug_type, context, fmt, ...)                    \
      do {                                                                        \
          char _sa_log_dbg_buf[2048];                                             \
          snprintf(_sa_log_dbg_buf, sizeof(_sa_log_dbg_buf), fmt, ##__VA_ARGS__); \
          sa_Log_Debug_Print_m(debug_type, context, _sa_log_dbg_buf);             \
      } while (0)
#else
#  define sa_Log_DebugF_Print_m(debug_type, context, fmt, ...)
#endif

/**
 * @definesa_Log_Warn_Print_m(
 * @brief A macro that prints warnings, using @ref sa_Log_Info
 *
 * @param[in] context The context of the warning
 * @param[in] message The message explaining the warning
 */
#define sa_Log_Warn_Print_m(severity, context, message)              \
    do {                                                             \
        sa_Log_Warn(severity, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @definesa_Log_WarnF_Print_m
 * @brief A macro that prints formatted warnings, using @ref sa_Log_Info
 *
 * @param[in] context The context of the warning
 * @param[in] message The message explaining the warning
 */
#define sa_Log_WarnF_Print_m(severity, context, fmt, ...)                         \
    do {                                                                          \
        char _sa_log_warn_buf[2048];                                              \
        snprintf(_sa_log_warn_buf, sizeof(_sa_log_warn_buf), fmt, ##__VA_ARGS__); \
        sa_Log_Warn_Print_m(severity, context, _sa_log_warn_buf);                 \
    } while (0)

/**
 * @define sa_Log_Error_Print_m
 * @brief A macro that prints errors, using @ref sa_Log_Error
 *
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 */
#define sa_Log_Error_Print_m(severity, context, message)              \
    do {                                                              \
        sa_Log_Error(severity, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @definesa_Log_ErrorF_Print_m
 * @brief A macro that prints formatted errors, using @ref sa_Log_Error
 *
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 */
#define sa_Log_ErrorF_Print_m(severity, context, fmt, ...)                      \
    do {                                                                        \
        char _sa_log_err_buf[2048];                                             \
        snprintf(_sa_log_err_buf, sizeof(_sa_log_err_buf), fmt, ##__VA_ARGS__); \
        sa_Log_Error_Print_m(severity, context, _sa_log_err_buf);               \
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

#define sa_Log_AssertF_Message_m(condition, fmt, ...)                                     \
    do {                                                                                  \
        if (!(condition)) {                                                               \
            char _sa_log_assert_buf[1024];                                                \
            snprintf(_sa_log_assert_buf, sizeof(_sa_log_assert_buf), fmt, ##__VA_ARGS__); \
            fprintf(stderr, "Assertion failed at %s:%d\nREASON: %s\n",                    \
                    __func__, __LINE__, _sa_log_assert_buf);                              \
            exit(EXIT_FAILURE);                                                           \
        }                                                                                 \
    } while (0)

// TODO doc
void sa_Log_OpenGL_Debug_Message_Callback(sa_u32 source, sa_u32 type, sa_u32 id, sa_u32 severity, int length, const char* msg, const void* data);

#endif // SACI_UTILS_SU_DEBUG_H
