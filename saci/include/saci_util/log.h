#ifndef SACI_UTILS_SU_LOG_H
#define SACI_UTILS_SU_LOG_H

#include "./types.h"
#include "./defines.h"

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

// TODO Rename
enum SaciLogContext {
    // Core Systems (0x00-0x1F)
    SACI_LOG_CONTEXT_CORE_INIT = 0x00,
    SACI_LOG_CONTEXT_CORE_SHUTDOWN = 0x01,
    SACI_LOG_CONTEXT_CORE_MAINLOOP = 0x02,
    SACI_LOG_CONTEXT_CORE_EVENTS = 0x03,
    SACI_LOG_CONTEXT_CORE_CONFIG = 0x04,
    SACI_LOG_CONTEXT_CORE_MEMORY = 0x05,
    SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER = 0x06,

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
    SACI_LOG_CONTEXT_WINDOWING_RUNTIME = 0x61,
    SACI_LOG_CONTEXT_DEPENDENCIES = 0x62,

    // (0x80-0x9F)

    // Input System (0xA0-0xBF)
    SACI_LOG_CONTEXT_INPUT = 0xA0,
    SACI_LOG_CONTEXT_INPUT_KEYBOARD = 0xA1,
    SACI_LOG_CONTEXT_INPUT_MOUSE = 0xA2,
    SACI_LOG_CONTEXT_INPUT_GAMEPAD = 0xA3,

    // (0xC0-0xDF)

    // External Libraries (0xE0-0xFF)
    SACI_LOG_CONTEXT_LIB_OPENGL4 = 0xE0,
    SACI_LOG_CONTEXT_LIB_OPENGLES3 = 0xE1,
    SACI_LOG_CONTEXT_LIB_STBI = 0xF0,
    SACI_LOG_CONTEXT_LIB_GLFW = 0xF1,

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

SACI_API const char* saci_log_severity_as_str(const enum SaciLogErrorSeverity severity);
SACI_API const char* saci_log_context_as_str(const enum SaciLogContext context);
SACI_API enum SaciLogType saci_log_get_current_type(void);
SACI_API void saci_log_set_type(enum SaciLogType type);

SACI_API const SaciLogError* saci_log_get_last_error(void);

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

SACI_API void saci_log_set_event_callback(SaciLogEventCallback event_callback);
SACI_API void saci_log_set_crash_callback(SaciLogCrashCallback crash_callback);
SACI_API void saci_log_set_assertion_fail_callback(SaciLogAssertionFailCallback assertion_callback);
SACI_API void saci_log_set_dummy_check_fail_callback(SaciLogDummyCheckFailCallback dummy_check_callback);

#endif // SACI_UTILS_SU_LOG_H
