#ifndef SACI_UTILS_SU_LOG_H
#define SACI_UTILS_SU_LOG_H

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

enum su_LogType {
    su_LOG_TYPE_DEV = -1,  // used ONLY for developers of the SACI lib
    su_LOG_TYPE_USER = -0, // used for the users of the SACI lib
    su_LOG_TYPE_PROD = 1,  // used for products using saci
};

enum su_LogErrorSeverity {
    su_LOG_ERROR_SEVERIY_LOW = 1,
    su_LOG_ERROR_SEVERIY_MEDIUM = 2,
    su_LOG_ERROR_SEVERIY_HIGH = 3,
    su_LOG_ERROR_SEVERIY_CRASH = 4,
};

enum su_LogContext {
    // Core Systems (0x00-0x1F)
    su_LOG_CONTEXT_CORE_INIT = 0x00,
    su_LOG_CONTEXT_CORE_SHUTDOWN = 0x01,
    su_LOG_CONTEXT_CORE_MAINLOOP = 0x02,
    su_LOG_CONTEXT_CORE_EVENTS = 0x03,
    su_LOG_CONTEXT_CORE_CONFIG = 0x04,
    su_LOG_CONTEXT_CORE_MEMORY = 0x05,
    su_LOG_CONTEXT_CORE_MEMORY_MANAGER = 0x05,

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

    // Error/Utility contexts
    su_LOG_CONTEXT_BAD_PARAMS = 0xFE,
    su_LOG_CONTEXT_UNCATEGORIZED = 0xFF
};

SA_API void su_log_error(const enum su_LogType type, const enum su_LogErrorSeverity severity, const enum su_LogContext context, const su_String* message, const char* file, const int line);

SA_API void su_log_warn(const enum su_LogType type, const enum su_LogErrorSeverity severity, const enum su_LogContext context, const su_String* message, const char* file, const int line);

SA_API void su_log_info(const enum su_LogType type, const enum su_LogContext context, const su_String* message, const char* file, const int line);

SA_API void su_log_assert(const su_Bool condition, const enum su_LogContext context, const su_String* message, const char* file, const int line);

SA_API void su_log_dummy_check(const su_Bool condition, const enum su_LogContext context, const su_String* message, const char* file, const int line);

#define su_LOG_ERROR_M(type, severity, context, message) \
    su_log_error(type, severity, context, message, __FILE__, __LINE__)

#define su_LOG_WARN_M(type, severity, context, message) \
    su_log_warn(type, severity, context, message, __FILE__, __LINE__)

#define su_LOG_INFO_M(type, context, message) \
    su_log_info(type, context, message, __FILE__, __LINE__)

#define su_LOG_ASSERT_M(condition, type, context, message) \
    su_log_assert(condition, type, context, message, __FILE__, __LINE__)

#define su_LOG_DUMMY_CHECK_M(condition, type, context, message) \
    su_log_dummy_check(condition, type, context, message, __FILE__, __LINE__)

#endif // SACI_UTILS_SU_LOG_H
