#ifndef SACI_UTILS_SU_LOG_H
#define SACI_UTILS_SU_LOG_H

#include "./su-types-common.h"
#include "./su-general.h"

enum su_LogType {
    su_LOG_TYPE_DEV = -1, // used ONLY for developers of the SACI lib
    su_LOG_TYPE_USER = 0, // used for the users of the SACI lib
    su_LOG_TYPE_PROD = 1, // used for products using saci
};

enum su_LogErrorSeverity {
    su_LOG_ERROR_SEVERITY_LOW = 1,
    su_LOG_ERROR_SEVERITY_MEDIUM = 2,
    su_LOG_ERROR_SEVERITY_HIGH = 3,
    su_LOG_ERROR_SEVERITY_CRASH = 4,
};

enum su_LogWarnSeverity {
    su_LOG_WARN_SEVERITY_LOW = 1,
    su_LOG_WARN_SEVERITY_MEDIUM = 2,
    su_LOG_WARN_SEVERITY_HIGH = 3,
};

enum su_LogContext {
    // Core Systems (0x00-0x1F)
    su_LOG_CONTEXT_CORE_INIT = 0x00,
    su_LOG_CONTEXT_CORE_SHUTDOWN = 0x01,
    su_LOG_CONTEXT_CORE_MAINLOOP = 0x02,
    su_LOG_CONTEXT_CORE_EVENTS = 0x03,
    su_LOG_CONTEXT_CORE_CONFIG = 0x04,
    su_LOG_CONTEXT_CORE_DARRAY = 0x06,
    su_LOG_CONTEXT_CORE_MEMORY = 0x07,
    su_LOG_CONTEXT_CORE_MEMORY_MANAGER = 0x08,

    // Rendering System (0x20-0x3F)
    su_LOG_CONTEXT_RENDERER = 0x20,
    su_LOG_CONTEXT_RENDERER_STATIC = 0x21,
    su_LOG_CONTEXT_RENDERER_INSTANCE = 0x22,
    su_LOG_CONTEXT_RENDERER_SHADERS = 0x23,
    su_LOG_CONTEXT_RENDERER_TEXTURES = 0x24,
    su_LOG_CONTEXT_RENDERER_BUFFERS = 0x25,
    su_LOG_CONTEXT_RENDERER_LIGHTS = 0x26,
    su_LOG_CONTEXT_RENDERER_CAMERA = 0x27,

    // Asset Management (0x40-0x5F)
    su_LOG_CONTEXT_ASSET_MANAGER = 0x40,
    su_LOG_CONTEXT_MODEL_LOADING = 0x41,
    su_LOG_CONTEXT_TEXTURE_LOADING = 0x42,
    su_LOG_CONTEXT_SHADER_LOADING = 0x43,
    su_LOG_CONTEXT_AUDIO_LOADING = 0x44,
    su_LOG_CONTEXT_SCENE_LOADING = 0x45,

    // (0x60-0x7F)

    // (0x80-0x9F)

    // Input System (0xA0-0xBF)
    su_LOG_CONTEXT_INPUT = 0xA0,
    su_LOG_CONTEXT_INPUT_KEYBOARD = 0xA1,
    su_LOG_CONTEXT_INPUT_MOUSE = 0xA2,
    su_LOG_CONTEXT_INPUT_GAMEPAD = 0xA3,

    // (0xC0-0xDF)

    // External Libraries (0xE0-0xFF)
    su_LOG_CONTEXT_LIB_OPENGL = 0xE0,
    su_LOG_CONTEXT_LIB_STBI = 0xE1,
    su_LOG_CONTEXT_LIB_GLFW = 0xE2,

    // GFX (0x100-0x11F)
    su_LOG_CONTEXT_GFX = 0x100,

    // Main (0x120-13F)
    su_LOG_CONTEXT_MAIN_SHAPES_INIT = 0x120,
    su_LOG_CONTEXT_MAIN_SHAPES_DRAW = 0x121,
};

#define su_LOG_MESSAGE_CHAR_COUNT 2048
#define su_LOG_FILE_CHAR_COUNT 256

typedef struct {
    enum su_LogType type;
    enum su_LogErrorSeverity severity;
    enum su_LogContext context;
    char message[su_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[su_LOG_FILE_CHAR_COUNT];
} su_LogError;

typedef struct {
    enum su_LogType type;
    enum su_LogWarnSeverity severity;
    enum su_LogContext context;
    char message[su_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[su_LOG_FILE_CHAR_COUNT];
} su_LogWarning;

typedef struct {
    enum su_LogType type;
    enum su_LogContext context;
    char message[su_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[su_LOG_FILE_CHAR_COUNT];
} su_LogInfo;

typedef struct {
    su_Bool passed;
    enum su_LogContext context;
    char message[su_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[su_LOG_FILE_CHAR_COUNT];
} su_LogAssertion;

typedef struct {
    su_Bool passed;
    enum su_LogContext context;
    char message[su_LOG_MESSAGE_CHAR_COUNT];
    int line;
    char file[su_LOG_FILE_CHAR_COUNT];
} su_LogDummyCheck;

SA_API const char* su_log_severity_as_str(const enum su_LogErrorSeverity severity);
SA_API const char* su_log_context_as_str(const enum su_LogContext context);
SA_API enum su_LogType su_log_get_current_type(void);
SA_API void su_log_set_type(enum su_LogType type);

SA_API const su_LogError* su_log_get_last_error(void);

#define su_LOG_ERROR_MAX_COUNT 128
#define su_LOG_WARNING_MAX_COUNT 128
#define su_LOG_INFO_MAX_COUNT 128
#define su_LOG_ASSERTION_MAX_COUNT 128
#define su_LOG_DUMMY_CHECK_MAX_COUNT 128

typedef struct su_LogBuffer su_LogBuffer; // Contains all of the above in the given counts

typedef enum {
    su_LOG_EVENT_ERROR = 0,
    su_LOG_EVENT_WARNING = 1,
    su_LOG_EVENT_INFO = 2,
    su_LOG_EVENT_ASSERTION = 3,
    su_LOG_EVENT_DUMMY_CHECK = 4,
} su_LogEventType;

struct su_LogEvent {
    union {
        const su_LogError* error;
        const su_LogWarning* warning;
        const su_LogInfo* info;
        const su_LogAssertion* assertion;
        const su_LogDummyCheck* dummy_check;
    } data;
    const su_LogEventType EVENT_TYPE;
};

typedef void (*su_LogCrashCallback)(const su_LogError*);
typedef void (*su_LogAssertionFailCallback)(const su_LogAssertion*);
typedef void (*su_LogDummyCheckFailCallback)(const su_LogDummyCheck*);
typedef void (*su_LogEventCallback)(const struct su_LogEvent* log_event);

SA_API void su_log_set_event_callback(su_LogEventCallback event_callback);
SA_API void su_log_set_crash_callback(su_LogCrashCallback crash_callback);
SA_API void su_log_set_assertion_fail_callback(su_LogAssertionFailCallback assertion_callback);
SA_API void su_log_set_dummy_check_fail_callback(su_LogDummyCheckFailCallback dummy_check_callback);

SA_API void su_log_error(const enum su_LogType type, const enum su_LogErrorSeverity severity, const enum su_LogContext context, const char* message, const char* file, const int line);

SA_API void su_log_warn(const enum su_LogType type, const enum su_LogWarnSeverity severity, const enum su_LogContext context, const char* message, const char* file, const int line);

SA_API void su_log_info(const enum su_LogType type, const enum su_LogContext context, const char* message, const char* file, const int line);

SA_API void su_log_assert(const su_Bool condition, const enum su_LogContext context, const char* message, const char* file, const int line);

SA_API void su_log_dummy_check(const su_Bool condition, const enum su_LogContext context, const char* message, const char* file, const int line);

// use when a condition fails or something that should happen happend
#define su_LOG_ERROR_M(type, severity, context, message) \
    su_log_error(type, severity, context, message, __FILE__, __LINE__)

// The do while loop crashes if the severity is "su_LOG_ERROR_SEVERITY_CRASH"
#define su_LOG_WARN_M(type, severity, context, message) \
    su_log_warn(type, severity, context, message, __FILE__, __LINE__);

#define su_LOG_INFO_M(type, context, message) \
    su_log_info(type, context, message, __FILE__, __LINE__)

// use to make sure simple stuff make sense (index < length etc). Think of it as asserting that the logic is working not that there is a expecific value and what not, that is done through checks and su_LOG_ERROR_M
#define su_LOG_ASSERT_M(condition, context, message) \
    su_log_assert(condition, context, message, __FILE__, __LINE__)

#define su_LOG_DUMMY_CHECK_M(condition, context, message) \
    su_log_dummy_check(condition, context, message, __FILE__, __LINE__)

#define su_LOG_ERRORF_M(type, severity, context, ...)                              \
    do {                                                                           \
        char su_log_message[2048];                                                 \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);             \
        su_log_error(type, severity, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define su_LOG_WARNF_M(type, severity, context, ...)                              \
    do {                                                                          \
        char su_log_message[2048];                                                \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);            \
        su_log_warn(type, severity, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define su_LOG_INFOF_M(type, context, ...)                              \
    do {                                                                \
        char su_log_message[2048];                                      \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);  \
        su_log_info(type, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define su_LOG_ASSERTF_M(condition, context, ...)                              \
    do {                                                                       \
        char su_log_message[2048];                                             \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);         \
        su_log_assert(condition, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#define su_LOG_DUMMY_CHECKF_M(condition, context, ...)                              \
    do {                                                                            \
        char su_log_message[2048];                                                  \
        snprintf(su_log_message, sizeof(su_log_message), __VA_ARGS__);              \
        su_log_dummy_check(condition, context, su_log_message, __FILE__, __LINE__); \
    } while (0)

#endif // SACI_UTILS_SU_LOG_H
