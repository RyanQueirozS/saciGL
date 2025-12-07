#ifndef SACI_CORE_LOG_LOG_INTERNAL_H
#define SACI_CORE_LOG_LOG_INTERNAL_H

#include "saci_util/log.h"

#include "saci_util/defines.h"

#include <stdio.h>

typedef struct SaciLogBuffer SaciLogBuffer; // Contains all of the above in the given counts

SACI_EXTERN void saci_log_error(const enum SaciLogType type, const enum SaciLogErrorSeverity severity, const enum SaciLogContext context, const char* message, const char* file, const int line);

SACI_EXTERN void saci_log_warn(const enum SaciLogType type, const enum SaciLogWarnSeverity severity, const enum SaciLogContext context, const char* message, const char* file, const int line);

SACI_EXTERN void saci_log_info(const enum SaciLogType type, const enum SaciLogContext context, const char* message, const char* file, const int line);

SACI_EXTERN void saci_log_assert(const SaciBool condition, const enum SaciLogContext context, const char* message, const char* file, const int line);

SACI_EXTERN void saci_log_dummy_check(const SaciBool condition, const enum SaciLogContext context, const char* message, const char* file, const int line);

// use when a condition fails or something that should happen happend
#define SACI_LOG_ERROR_M(type, severity, context, message) \
    saci_log_error(type, severity, context, message, __FILE__, __LINE__)

#define SACI_LOG_WARN_M(type, severity, context, message) \
    saci_log_warn(type, severity, context, message, __FILE__, __LINE__);

#define SACI_LOG_INFO_M(type, context, message) \
    saci_log_info(type, context, message, __FILE__, __LINE__)

// Use this to make sure simple stuff make sense (index < length etc). Think of
// it as asserting that the logic is working not that there is a expecific value
// and what not, that is done through checks and other LOG functions
#define SACI_LOG_ASSERT_M(condition, context, message) \
    saci_log_assert(condition, context, message, __FILE__, __LINE__)

#define SACI_LOG_DUMMY_CHECK_M(condition, context, message) \
    saci_log_dummy_check(condition, context, message, __FILE__, __LINE__)

#define SACI_LOG_ERRORF_M(type, severity, context, ...)                                \
    do {                                                                               \
        char saci_log_message[2048];                                                   \
        if (snprintf(saci_log_message, sizeof(saci_log_message), __VA_ARGS__) < 0) {   \
            abort();                                                                   \
        };                                                                             \
        saci_log_error(type, severity, context, saci_log_message, __FILE__, __LINE__); \
    } while (0)

#define SACI_LOG_WARNF_M(type, severity, context, ...)                                \
    do {                                                                              \
        char saci_log_message[2048];                                                  \
        if (snprintf(saci_log_message, sizeof(saci_log_message), __VA_ARGS__) < 0) {  \
            abort();                                                                  \
        };                                                                            \
        saci_log_warn(type, severity, context, saci_log_message, __FILE__, __LINE__); \
    } while (0)

#define SACI_LOG_INFOF_M(type, context, ...)                                         \
    do {                                                                             \
        char saci_log_message[2048];                                                 \
        if (snprintf(saci_log_message, sizeof(saci_log_message), __VA_ARGS__) < 0) { \
            abort();                                                                 \
        };                                                                           \
        saci_log_info(type, context, saci_log_message, __FILE__, __LINE__);          \
    } while (0)

#define SACI_LOG_ASSERTF_M(condition, context, ...)                                  \
    do {                                                                             \
        char saci_log_message[2048];                                                 \
        if (snprintf(saci_log_message, sizeof(saci_log_message), __VA_ARGS__) < 0) { \
            abort();                                                                 \
        };                                                                           \
        saci_log_assert(condition, context, saci_log_message, __FILE__, __LINE__);   \
    } while (0)

#define SACI_LOG_DUMMY_CHECKF_M(condition, context, ...)                                \
    do {                                                                                \
        char saci_log_message[2048];                                                    \
        if (snprintf(saci_log_message, sizeof(saci_log_message), __VA_ARGS__) < 0) {    \
            abort();                                                                    \
        };                                                                              \
        saci_log_dummy_check(condition, context, saci_log_message, __FILE__, __LINE__); \
    } while (0)

#endif // SACI_CORE_LOG_LOG_INTERNAL_H
