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
 * @enum su_LogSeverity
 * @brief The severity of log information
 */
enum su_LogSeverity {
    su_LOG_SEVERITY_LOW = 0,
    su_LOG_SEVERITY_MEDIUM,
    su_LOG_SEVERITY_HIGH,
};

/**
 * @enum su_LogContext
 * @brief The context of log information
 */
enum su_LogContext {
    su_LOG_CONTEXT_OPENGL = 0,
    su_LOG_CONTEXT_INIT,
    su_LOG_CONTEXT_RENDERER,
    su_LOG_CONTEXT_MODEL_LOADING,
    su_LOG_CONTEXT_STBI,
    su_LOG_CONTEXT_MEMORY,
    su_LOG_CONTEXT_CONFIG,
    su_LOG_CONTEXT_SACI_MAIN_SHAPES,
};

enum su_LogDebugType {
    su_LOG_DEBUG_TYPE_WINDOWING,
    su_LOG_DEBUG_TYPE_TEXTURE,
    su_LOG_DEBUG_TYPE_MODEL,
    su_LOG_DEBUG_TYPE_OPENGL,
    su_LOG_DEBUG_TYPE_RENDERER,
    su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
    su_LOG_DEBUG_TYPE_RENDERER_BATCH,
    su_LOG_DEBUG_TYPE_RENDERER_CALL,
    su_LOG_DEBUG_TYPE_SACI_MAIN_MEM,
};

/**
 * @brief su_log_should_print_origin Enables or disables origin printing,
 * default is false, unless in debug mode
 *
 * @param[in] enable The option to enable or disable
 */
void su_log_should_print_origin(su_Bool enable);

/**
 * @fn su_log_error
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
void su_log_error(enum su_LogSeverity severity,
                  enum su_LogContext context,
                  const char* message, const char* file, int line);

/**
 * @fn su_log_warn
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
void su_log_warn(enum su_LogSeverity severity,
                 enum su_LogContext context,
                 const char* message, const char* file, int line);

/**
 * @fn su_log_info
 * @brief Logs an information using saci's print template
 * @note DO NOT USE THIS. There is already a provided macro that reduces param
 * amount
 *
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 * @param[in] file The file the info comes from
 * @param[in] line The line the info comes from
 */
void su_log_info(enum su_LogContext context,
                 const char* message, const char* file, int line);

/**
 * @fn su_log_debug
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
void su_log_debug(enum su_LogDebugType type, enum su_LogContext context,
                  const char* message, const char* file, int line);

/**
 * @define su_LOG_INFO_PRINT_M
 * @brief A macro that prints info, using @ref su_log_info
 *
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#define su_LOG_INFO_PRINT_M(context, message)              \
    do {                                                   \
        su_log_info(context, message, __FILE__, __LINE__); \
    } while (0)

#define su_LOG_INFOF_PRINT_M(context, fmt, ...)                                 \
    do {                                                                        \
        char su_log_info_buf[2048];                                             \
        snprintf(su_log_info_buf, sizeof(su_log_info_buf), fmt, ##__VA_ARGS__); \
        su_LOG_INFO_PRINT_M(context, su_log_info_buf);                          \
    } while (0)

/**
 * @define su_LOG_DEBUG_PRINT_M
 * @brief A macro that prints debug info, using @ref su_log_debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#ifdef SACI_DEBUG_MODE_ENABLED
#  define su_LOG_DEBUG_PRINT_M(debug_type, context, message)              \
      do {                                                                \
          su_log_debug(debug_type, context, message, __FILE__, __LINE__); \
      } while (0)

#else
#  define su_LOG_DEBUG_PRINT_M(debug_type, context, message)
#endif

/**
 * @define su_LOG_DEBUG_PRINT_M
 * @brief A macro that prints debug info if a condition is met, using @ref su_log_debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#ifdef SACI_DEBUG_MODE_ENABLED
#  define su_LOG_DEBUG_CONDITION_PRINT_M(condition, debug_type, context, message) \
      do {                                                                        \
          if (condition) {                                                        \
              su_log_debug(debug_type, context, message, __FILE__, __LINE__);     \
          }                                                                       \
      } while (0)

#else
#  define su_LOG_DEBUG_CONDITION_PRINT_M(condition, debug_type, context, message) \
      (void)(condition)
#endif

/**
 * @define su_LOG_DEBUGF_PRINT_M
 * @brief A macro that prints formatted debug info, using @ref su_log_debug
 *
 * @param[in] type The type of the info
 * @param[in] context The context of the info
 * @param[in] message The message explaining the info
 */
#ifdef SACI_DEBUG_MODE_ENABLED
#  define su_LOG_DEBUGF_PRINT_M(debug_type, context, fmt, ...)                  \
      do {                                                                      \
          char su_log_dbg_buf[2048];                                            \
          snprintf(su_log_dbg_buf, sizeof(su_log_dbg_buf), fmt, ##__VA_ARGS__); \
          su_LOG_DEBUG_PRINT_M(debug_type, context, su_log_dbg_buf);            \
      } while (0)
#else
#  define su_LOG_DEBUGF_PRINT_M(debug_type, context, fmt, ...)
#endif

/**
 * @define su_LOG_WARN_PRINT_M(
 * @brief A macro that prints warnings, using @ref su_log_info
 *
 * @param[in] context The context of the warning
 * @param[in] message The message explaining the warning
 */
#define su_LOG_WARN_PRINT_M(severity, context, message)              \
    do {                                                             \
        su_log_warn(severity, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @define su_LOG_WARNF_PRINT_M
 * @brief A macro that prints formatted warnings, using @ref su_log_info
 *
 * @param[in] context The context of the warning
 * @param[in] message The message explaining the warning
 */
#define su_LOG_WARNF_PRINT_M(severity, context, fmt, ...)                       \
    do {                                                                        \
        char su_log_warn_buf[2048];                                             \
        snprintf(su_log_warn_buf, sizeof(su_log_warn_buf), fmt, ##__VA_ARGS__); \
        su_LOG_WARN_PRINT_M(severity, context, su_log_warn_buf);                \
    } while (0)

/**
 * @define su_LOG_ERROR_PRINT_M
 * @brief A macro that prints errors, using @ref su_log_error
 *
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 */
#define su_LOG_ERROR_PRINT_M(severity, context, message)              \
    do {                                                              \
        su_log_error(severity, context, message, __FILE__, __LINE__); \
    } while (0)

/**
 * @define su_LOG_ERRORF_PRINT_M
 * @brief A macro that prints formatted errors, using @ref su_log_error
 *
 * @param[in] context The context of the error
 * @param[in] message The message explaining the error
 */
#define su_LOG_ERRORF_PRINT_M(severity, context, fmt, ...)                    \
    do {                                                                      \
        char su_log_err_buf[2048];                                            \
        snprintf(su_log_err_buf, sizeof(su_log_err_buf), fmt, ##__VA_ARGS__); \
        su_LOG_ERROR_PRINT_M(severity, context, su_log_err_buf);              \
    } while (0)

/**
 * @define su_LOG_ASSERT_MESSAGE_M
 * @brief A macro that asserts conditions
 *
 * @param[in] condition The condition
 * @param[in] message The message explaining the error
 */
#define su_LOG_ASSERT_MESSAGE_M(condition, message)                                              \
    do {                                                                                         \
        if (!(condition)) {                                                                      \
            fprintf(stderr, "[ASSERTION FAILED]: LOCATION %s:%d: CONDITION: (%s) MESSAGE: %s\n", \
                    __func__, __LINE__, #condition, message);                                    \
            exit(EXIT_FAILURE);                                                                  \
        }                                                                                        \
    } while (0)

#define su_LOG_ASSERTF_MESSAGE_M(condition, fmt, ...)                                    \
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

/**
 * @define su_DUMMY_CHECK_M
 * @brief A dummy check that asserts a condition is true only in debug builds.
 *
 * @details
 * This is intended for sanity checks on conditions that should *never* fail
 * during normal operation. It is compiled out entirely if
 * `SACI_DUMMY_CHECK_DISABLE` is defined, to avoid any runtime cost in release builds.
 *
 * @param[in] condition The condition that must be true.
 * @param[in] message   Message printed if the condition fails.
 */
#ifndef SACI_DUMMY_CHECK_DISABLE
#  define su_DUMMY_CHECK_M(condition, message)                                               \
      do {                                                                                   \
          if (!(condition)) {                                                                \
              fprintf(stderr,                                                                \
                      "[DUMMY CHECK FAILED]: LOCATION %s:%d: CONDITION: (%s) MESSAGE: %s\n", \
                      __func__, __LINE__, #condition, message);                              \
              exit(EXIT_FAILURE);                                                            \
          }                                                                                  \
      } while (0)

#  define su_DUMMY_CHECKF_M(condition, fmt, ...)                                             \
      do {                                                                                   \
          if (!(condition)) {                                                                \
              char log_dummy_buf[1024];                                                      \
              snprintf(log_dummy_buf, sizeof(log_dummy_buf), fmt, ##__VA_ARGS__);            \
              fprintf(stderr,                                                                \
                      "[DUMMY CHECK FAILED]: LOCATION %s:%d: CONDITION: (%s) MESSAGE: %s\n", \
                      __func__, __LINE__, #condition, log_dummy_buf);                        \
              exit(EXIT_FAILURE);                                                            \
          }                                                                                  \
      } while (0)
#else
#  define su_DUMMY_CHECK_M(condition, message) \
      do {                                     \
      } while (0)
#  define su_DUMMY_CHECKF_M(condition, fmt, ...) \
      do {                                       \
      } while (0)
#endif

// TODO doc
void su_log_opengl_debug_message_callback(su_U32 source, su_U32 type, su_U32 id, su_U32 severity, int length, const char* msg, const void* data);

#endif // SACI_UTILS_SU_DEBUG_H
