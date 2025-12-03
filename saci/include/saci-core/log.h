#ifndef SACI_UTILS_SU_LOG_H
#define SACI_UTILS_SU_LOG_H

#include "./types.h"
#include "./defines.h"

#include <stdio.h>

enum SaciLogType {
    SACI_LOG_TYPE_DEV = -1, // used ONLY for developers of the SACI lib
    SACI_LOG_TYPE_USER = 0, // used for the users of the SACI lib
    SACI_LOG_TYPE_PROD = 1, // used for products using saci
};

enum SaciLogErrorSeverity {
    SACI_LOG_ERROR_SEVERITY_LOW = 1,
    SACI_LOG_ERROR_SEVERITY_MEDIUM = 2,
    SACI_LOG_ERROR_SEVERITY_HIGH = 3,
    SACI_LOG_ERROR_SEVERITY_CRASH = 4,
};

enum SaciLogWarnSeverity {
    SACI_LOG_WARN_SEVERITY_LOW = 1,
    SACI_LOG_WARN_SEVERITY_MEDIUM = 2,
    SACI_LOG_WARN_SEVERITY_HIGH = 3,
};

enum SaciLogContext {
    // Core Systems (0x00-0x1F)
    SACI_LOG_CONTEXT_CORE_INIT = 0x00,
    SACI_LOG_CONTEXT_CORE_SHUTDOWN = 0x01,
    SACI_LOG_CONTEXT_CORE_MAINLOOP = 0x02,
    SACI_LOG_CONTEXT_CORE_EVENTS = 0x03,
    SACI_LOG_CONTEXT_CORE_CONFIG = 0x04,
    SACI_LOG_CONTEXT_CORE_DARRAY = 0x06,
    SACI_LOG_CONTEXT_CORE_MEMORY = 0x07,
    SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER = 0x08,

    // Rendering System (0x20-0x3F)
    SACI_LOG_CONTEXT_RENDERER = 0x20,
    SACI_LOG_CONTEXT_RENDERER_STATIC = 0x21,
    SACI_LOG_CONTEXT_RENDERER_INSTANCE = 0x22,
    SACI_LOG_CONTEXT_RENDERER_SHADERS = 0x23,
    SACI_LOG_CONTEXT_RENDERER_TEXTURES = 0x24,
    SACI_LOG_CONTEXT_RENDERER_BUFFERS = 0x25,
    SACI_LOG_CONTEXT_RENDERER_LIGHTS = 0x26,
    SACI_LOG_CONTEXT_RENDERER_CAMERA = 0x27,

    // Asset Management (0x40-0x5F)
    SACI_LOG_CONTEXT_ASSET_MANAGER = 0x40,
    SACI_LOG_CONTEXT_MODEL_LOADING = 0x41,
    SACI_LOG_CONTEXT_TEXTURE_LOADING = 0x42,
    SACI_LOG_CONTEXT_SHADER_LOADING = 0x43,
    SACI_LOG_CONTEXT_AUDIO_LOADING = 0x44,
    SACI_LOG_CONTEXT_SCENE_LOADING = 0x45,

    // (0x60-0x7F)
    SACI_LOG_CONTEXT_WINDOWING = 0x60,

    // (0x80-0x9F)

    // Input System (0xA0-0xBF)
    SACI_LOG_CONTEXT_INPUT = 0xA0,
    SACI_LOG_CONTEXT_INPUT_KEYBOARD = 0xA1,
    SACI_LOG_CONTEXT_INPUT_MOUSE = 0xA2,
    SACI_LOG_CONTEXT_INPUT_GAMEPAD = 0xA3,

    // (0xC0-0xDF)

    // External Libraries (0xE0-0xFF)
    SACI_LOG_CONTEXT_LIB_OPENGL = 0xE0,
    SACI_LOG_CONTEXT_LIB_STBI = 0xE1,
    SACI_LOG_CONTEXT_LIB_GLFW = 0xE2,

    // GFX (0x100-0x11F)
    SACI_LOG_CONTEXT_GFX = 0x100,

    // Main (0x120-13F)
    SACI_LOG_CONTEXT_MAIN_SHAPES_INIT = 0x120,
    SACI_LOG_CONTEXT_MAIN_SHAPES_DRAW = 0x121,
};

#define SACI_LOG_MESSAGE_CHAR_COUNT 2048
#define SACI_LOG_FILE_CHAR_COUNT 256

typedef struct {
    enum SaciLogType type;
    enum SaciLogErrorSeverity severity;
    enum SaciLogContext context;
    char message[SACI_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[SACI_LOG_FILE_CHAR_COUNT];
} SaciLogError;

typedef struct {
    enum SaciLogType type;
    enum SaciLogWarnSeverity severity;
    enum SaciLogContext context;
    char message[SACI_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[SACI_LOG_FILE_CHAR_COUNT];
} SaciLogWarning;

typedef struct {
    enum SaciLogType type;
    enum SaciLogContext context;
    char message[SACI_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[SACI_LOG_FILE_CHAR_COUNT];
} SaciLogInfo;

typedef struct {
    SaciBool passed;
    enum SaciLogContext context;
    char message[SACI_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[SACI_LOG_FILE_CHAR_COUNT];
} SaciLogAssertion;

typedef struct {
    SaciBool passed;
    enum SaciLogContext context;
    char message[SACI_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[SACI_LOG_FILE_CHAR_COUNT];
} SaciLogDummyCheck;

SA_API const char* saci_log_severity_as_str(const enum SaciLogErrorSeverity severity);
SA_API const char* saci_log_context_as_str(const enum SaciLogContext context);
SA_API enum SaciLogType saci_log_get_current_type(void);
SA_API void saci_log_set_type(enum SaciLogType type);

SA_API const SaciLogError* saci_log_get_last_error(void);

#define SACI_LOG_ERROR_MAX_COUNT 128
#define SACI_LOG_WARNING_MAX_COUNT 128
#define SACI_LOG_INFO_MAX_COUNT 128
#define SACI_LOG_ASSERTION_MAX_COUNT 128
#define SACI_LOG_DUMMY_CHECK_MAX_COUNT 128

typedef struct SaciLogBuffer SaciLogBuffer; // Contains all of the above in the given counts

typedef enum {
    SACI_LOG_EVENT_ERROR = 0,
    SACI_LOG_EVENT_WARNING = 1,
    SACI_LOG_EVENT_INFO = 2,
    SACI_LOG_EVENT_ASSERTION = 3,
    SACI_LOG_EVENT_DUMMY_CHECK = 4,
} SaciLogEventType;

struct SaciLogEvent {
    union {
        const SaciLogError* error;
        const SaciLogWarning* warning;
        const SaciLogInfo* info;
        const SaciLogAssertion* assertion;
        const SaciLogDummyCheck* dummy_check;
    } data;
    const SaciLogEventType EVENT_TYPE;
};

typedef void (*SaciLogCrashCallback)(const SaciLogError*);
typedef void (*SaciLogAssertionFailCallback)(const SaciLogAssertion*);
typedef void (*SaciLogDummyCheckFailCallback)(const SaciLogDummyCheck*);
typedef void (*SaciLogEventCallback)(const struct SaciLogEvent* log_event);

SA_API void saci_log_set_event_callback(SaciLogEventCallback event_callback);
SA_API void saci_log_set_crash_callback(SaciLogCrashCallback crash_callback);
SA_API void saci_log_set_assertion_fail_callback(SaciLogAssertionFailCallback assertion_callback);
SA_API void saci_log_set_dummy_check_fail_callback(SaciLogDummyCheckFailCallback dummy_check_callback);

SA_API void saci_log_error(const enum SaciLogType type, const enum SaciLogErrorSeverity severity, const enum SaciLogContext context, const char* message, const char* file, const int line);

SA_API void saci_log_warn(const enum SaciLogType type, const enum SaciLogWarnSeverity severity, const enum SaciLogContext context, const char* message, const char* file, const int line);

SA_API void saci_log_info(const enum SaciLogType type, const enum SaciLogContext context, const char* message, const char* file, const int line);

SA_API void saci_log_assert(const SaciBool condition, const enum SaciLogContext context, const char* message, const char* file, const int line);

SA_API void saci_log_dummy_check(const SaciBool condition, const enum SaciLogContext context, const char* message, const char* file, const int line);

// use when a condition fails or something that should happen happend
#define SACI_LOG_ERROR_M(type, severity, context, message) \
    su_log_error(type, severity, context, message, __FILE__, __LINE__)

// The do while loop crashes if the severity is "su_LOG_ERROR_SEVERITY_CRASH"
#define SACI_LOG_WARN_M(type, severity, context, message) \
    su_log_warn(type, severity, context, message, __FILE__, __LINE__);

#define SACI_LOG_INFO_M(type, context, message) \
    su_log_info(type, context, message, __FILE__, __LINE__)

// use to make sure simple stuff make sense (index < length etc). Think of it as asserting that the logic is working not that there is a expecific value and what not, that is done through checks and SACI_LOG_ERROR_M
#define SACI_LOG_ASSERT_M(condition, context, message) \
    su_log_assert(condition, context, message, __FILE__, __LINE__)

#define SACI_LOG_DUMMY_CHECK_M(condition, context, message) \
    su_log_dummy_check(condition, context, message, __FILE__, __LINE__)

#define SACI_LOG_ERRORF_M(type, severity, context, ...)                            \
    do {                                                                           \
        char su_log_message[2048];                                                 \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);             \
        su_log_error(type, severity, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define SACI_LOG_WARNF_M(type, severity, context, ...)                            \
    do {                                                                          \
        char su_log_message[2048];                                                \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);            \
        su_log_warn(type, severity, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define SACI_LOG_INFOF_M(type, context, ...)                            \
    do {                                                                \
        char su_log_message[2048];                                      \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);  \
        su_log_info(type, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define SACI_LOG_ASSERTF_M(condition, context, ...)                            \
    do {                                                                       \
        char su_log_message[2048];                                             \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);         \
        su_log_assert(condition, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define SACI_LOG_DUMMY_CHECKF_M(condition, context, ...)                            \
    do {                                                                            \
        char su_log_message[2048];                                                  \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);              \
        su_log_dummy_check(condition, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#endif // SACI_UTILS_SU_LOG_H
