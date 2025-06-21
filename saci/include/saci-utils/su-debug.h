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

#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_WINDOWING) ||   \
    defined(SACI_DEBUG_MODE_TEXTURE) || defined(SACI_DEBUG_MODE_MODEL) ||   \
    defined(SACI_DEBUG_MODE_OPENGL) || defined(SACI_DEBUG_MODE_RENDERER) || \
    defined(SACI_DEBUG_MODE_RENDERER_FUNCTIONS) ||                          \
    defined(SACI_DEBUG_MODE_RENDERER_BATCH) ||                              \
    defined(SACI_DEBUG_MODE_RENDERER_CALL)
#  define SACI_DEBUG_MODE_ENABLED
#endif

#include "saci-utils/su-types.h"

/**
 * @enum su_logSeverity
 * @brief The severity of log information
 */
enum su_logSeverity {
    su_LOG_SEVERITY_LOW = 0,
    su_LOG_SEVERITY_MEDIUM,
    su_LOG_SEVERITY_HIGH,
};

/**
 * @enum su_logContext
 * @brief The context of log information
 */
enum su_logContext {
    su_LOG_CONTEXT_OPENGL = 0,
    su_LOG_CONTEXT_RENDERER,
    su_LOG_CONTEXT_MODEL_LOADING,
    su_LOG_CONTEXT_STBI,
    su_LOG_CONTEXT_MEMORY,
    su_LOG_CONTEXT_CONFIG,
};

enum su_logDebugType {
    su_LOG_DEBUG_TYPE_WINDOWING,
    su_LOG_DEBUG_TYPE_TEXTURE,
    su_LOG_DEBUG_TYPE_MODEL,
    su_LOG_DEBUG_TYPE_OPENGL,
    su_LOG_DEBUG_TYPE_RENDERER,
    su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
    su_LOG_DEBUG_TYPE_RENDERER_BATCH,
    su_LOG_DEBUG_TYPE_RENDERER_CALL,
};

/**
 * @brief su_Log_Should_Print_Origin Enables or disables origin printing,
 * default is false, unless in debug mode
 *
 * @param[in] enable The option to enable or disable
 */
void su_Log_Should_Print_Origin(sa_bool enable);

/**
 * @fn su_Log_Error
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
void su_Log_Error(enum su_logSeverity severity,
                  enum su_logContext context,
                  const char* message, const char* file, int line);

/**
 * @fn su_Log_Warn
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
void su_Log_Warn(enum su_logSeverity severity,
                 enum su_logContext context,
                 const char* message, const char* file, int line);

/**
 * @fn su_Log_Info
 * @brief Logs an information using saci's print template
 * @note DO NOT USE THIS. There is already a provided macro that reduces param
 * amount
 *
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 * @param[in] file The file the info comes from
 * @param[in] line The line the info comes from
 */
void su_Log_Info(enum su_logContext context,
                 const char* message, const char* file, int line);

/**
 * @fn su_Log_Debug
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
void su_Log_Debug(enum su_logDebugType type, enum su_logContext context,
                  const char* message, const char* file, int line);

/**
 * @define su_Log_Info_Print_m
 * @brief A macro that prints info, using @ref su_Log_Info
 *
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#define su_Log_Info_Print_m(context, message)              \
    do {                                                   \
        su_Log_Info(context, message, __FILE__, __LINE__); \
    } while (0)

#define su_Log_InfoF_Print_m(context, fmt, ...)                                 \
    do {                                                                        \
        char su_log_info_buf[2048];                                             \
        snprintf(su_log_info_buf, sizeof(su_log_info_buf), fmt, ##__VA_ARGS__); \
        su_Log_Info_Print_m(context, su_log_info_buf);                          \
    } while (0)

/**
 * @define su_Log_Debug_Print_m
 * @brief A macro that prints debug info, using @ref su_Log_Debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#ifdef SACI_DEBUG_MODE_ENABLED
#  define su_Log_Debug_Print_m(debug_type, context, message)              \
      do {                                                                \
          su_Log_Debug(debug_type, context, message, __FILE__, __LINE__); \
      } while (0)

#else
#  define su_Log_Debug_Print_m(debug_type, context, message)
#endif

/**
 * @define su_Log_Debug_Print_m
 * @brief A macro that prints debug info if a condition is met, using @ref su_Log_Debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#ifdef SACI_DEBUG_MODE_ENABLED
#  define su_Log_Debug_Condition_Print_m(condition, debug_type, context, message) \
      do {                                                                        \
          if (condition) {                                                        \
              su_Log_Debug(debug_type, context, message, __FILE__, __LINE__);     \
          }                                                                       \
      } while (0)

#else
#  define su_Log_Debug_Condition_Print_m(condition, debug_type, context, message)
#endif

/**
 * @define su_Log_DebugF_Print_m
 * @brief A macro that prints formatted debug info, using @ref su_Log_Debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#ifdef SACI_DEBUG_MODE_ENABLED
#  define su_Log_DebugF_Print_m(debug_type, context, fmt, ...)                  \
      do {                                                                      \
          char su_log_dbg_buf[2048];                                            \
          snprintf(su_log_dbg_buf, sizeof(su_log_dbg_buf), fmt, ##__VA_ARGS__); \
          su_Log_Debug_Print_m(debug_type, context, su_log_dbg_buf);            \
      } while (0)
#else
#  define su_Log_DebugF_Print_m(debug_type, context, fmt, ...)
#endif

/**
 * @define su_Log_Warn_Print_m(
 * @brief A macro that prints warnings, using @ref su_Log_Info
 *
 * @param[in] context The context of the warning
 * @param[in] message The message explaining the warning
 */
#define su_Log_Warn_Print_m(severity, context, message)              \
    do {                                                             \
        su_Log_Warn(severity, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @define su_Log_WarnF_Print_m
 * @brief A macro that prints formatted warnings, using @ref su_Log_Info
 *
 * @param[in] context The context of the warning
 * @param[in] message The message explaining the warning
 */
#define su_Log_WarnF_Print_m(severity, context, fmt, ...)                       \
    do {                                                                        \
        char su_log_warn_buf[2048];                                             \
        snprintf(su_log_warn_buf, sizeof(su_log_warn_buf), fmt, ##__VA_ARGS__); \
        su_Log_Warn_Print_m(severity, context, su_log_warn_buf);                \
    } while (0)

/**
 * @define su_Log_Error_Print_m
 * @brief A macro that prints errors, using @ref su_Log_Error
 *
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 */
#define su_Log_Error_Print_m(severity, context, message)              \
    do {                                                              \
        su_Log_Error(severity, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @define su_Log_ErrorF_Print_m
 * @brief A macro that prints formatted errors, using @ref su_Log_Error
 *
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 */
#define su_Log_ErrorF_Print_m(severity, context, fmt, ...)                    \
    do {                                                                      \
        char su_log_err_buf[2048];                                            \
        snprintf(su_log_err_buf, sizeof(su_log_err_buf), fmt, ##__VA_ARGS__); \
        su_Log_Error_Print_m(severity, context, su_log_err_buf);              \
    } while (0)

/**
 * @define su_Log_Assert_Message_m
 * @brief A macro that asserts conditions
 *
 * @param[in] condition The condition
 * @param[in] message The message explaining the error
 */
#define su_Log_Assert_Message_m(condition, message)                                              \
    do {                                                                                         \
        if (!(condition)) {                                                                      \
            fprintf(stderr, "[ASSERTION FAILED]: LOCATION %s:%d: CONDITION: (%s) MESSAGE: %s\n", \
                    __func__, __LINE__, #condition, message);                                    \
            exit(EXIT_FAILURE);                                                                  \
        }                                                                                        \
    } while (0)

#define su_Log_AssertF_Message_m(condition, fmt, ...)                                    \
    do {                                                                                 \
        if (!(condition)) {                                                              \
            char log_assert_buf[1024];                                                   \
            snprintf(log_assert_buf, sizeof(log_assert_buf), fmt, ##__VA_ARGS__);        \
            fprintf(stderr,                                                              \
                    "[ASSERTION FAILED]: LOCATION: %s:%d CONDITION: (%s) MESSAGE: %s\n", \
                    __func__, __LINE__, #condition, log_assert_buf);                     \
            exit(EXIT_FAILURE);                                                          \
        }                                                                                \
    } while (0)

// TODO doc
void su_Log_OpenGL_Debug_Message_Callback(sa_u32 source, sa_u32 type, sa_u32 id, sa_u32 severity, int length, const char* msg, const void* data);

#endif // SACI_UTILS_SU_DEBUG_H
