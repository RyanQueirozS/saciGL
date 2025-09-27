#include "saci-utils/su-log.h"
#include <saci-utils/su-types.h>
#include <stdio.h>

SA_INTERNAL enum su_LogType su__type = su_LOG_TYPE_PROD;

const char* su__log_severity_as_str(const enum su_LogErrorSeverity severity);
const char* su__log_context_as_str(const enum su_LogContext severity);

void su_log_error(const enum su_LogType type, const enum su_LogErrorSeverity severity, const enum su_LogContext context, const su_String* message, const char* file, const int line) {
    if ((int)su__type < type) {
        return;
    }
    const char* severity_str = su__log_severity_as_str(severity);
    const char* context_str = su__log_context_as_str(context);
    printf("ERROR of [%s] severity: [%s] (%s) at %s:%d\n", severity_str, context_str, su_string_data(message), file, line);
}

void su_log_warn(const enum su_LogType type, const enum su_LogErrorSeverity severity, const enum su_LogContext context, const su_String* message, const char* file, const int line) {
    if ((int)su__type < type) {
        return;
    }
    const char* severity_str = su__log_severity_as_str(severity);
    const char* context_str = su__log_context_as_str(context);
    printf("WARNING of [%s] severity: [%s] (%s) at %s:%d\n", severity_str, context_str, su_string_data(message), file, line);
}

void su_log_info(const enum su_LogType type, const enum su_LogContext context, const su_String* message, const char* file, const int line) {
    if (su__type < type) {
        return;
    }
    const char* context_str = su__log_context_as_str(context);
    printf("INFO: [%s] (%s) at %s:%d\n", context_str, su_string_data(message), file, line);
}

void su_log_assert(const su_Bool condition, const enum su_LogContext context, const su_String* message, const char* file, const int line) {
    if (!condition) {
        const char* context_str = su__log_context_as_str(context);
        printf("ASSERTION FAILED: [%s] (%s) at %s:%d\n", context_str, su_string_data(message), file, line);
        abort();
    }
}

void su_log_dummy_check(const su_Bool condition, const enum su_LogContext context, const su_String* message, const char* file, const int line) {
    if (su__type != su_LOG_TYPE_DEV && su__type != su_LOG_TYPE_USER) {
        printf("CRITICAL ERROR: A development safety check failed in production environment\n"
               "Please contact the development team immediately\n");
        abort();
    }

    if (!condition) {
        const char* context_str = su__log_context_as_str(context);
        printf("DUMMY CHECK FAILED: [%s] (%s) at %s:%d\n", context_str, su_string_data(message), file, line);
        abort();
    }
}

const char* su__log_severity_as_str(const enum su_LogErrorSeverity severity) {
    switch (severity) {
    case su_LOG_ERROR_SEVERIY_LOW:
        return "LOW";
    case su_LOG_ERROR_SEVERIY_MEDIUM:
        return "MEDIUM";
    case su_LOG_ERROR_SEVERIY_HIGH:
        return "HIGH";
    case su_LOG_ERROR_SEVERIY_CRASH:
        return "CRASH";
    }
    return "UNKOWN";
}

const char* su__log_context_as_str(const enum su_LogContext severity) {
    switch (severity) {
    case su_LOG_CONTEXT_CORE_INIT:
        return "CORE_INIT";
    case su_LOG_CONTEXT_CORE_SHUTDOWN:
        return "CORE_SHUTDOWN";
    case su_LOG_CONTEXT_CORE_MAINLOOP:
        return "CORE_MAINLOOP";
    case su_LOG_CONTEXT_CORE_EVENTS:
        return "CORE_EVENTS";
    case su_LOG_CONTEXT_CORE_CONFIG:
        return "CORE_CONFIG";
    case su_LOG_CONTEXT_CORE_MEMORY:
        return "CORE_MEMORY";
    case su_LOG_CONTEXT_RENDERER:
        return "RENDERER";
    case su_LOG_CONTEXT_RENDERER_STATIC:
        return "RENDERER_STATIC";
    case su_LOG_CONTEXT_RENDERER_INSTANCE:
        return "RENDERER_INSTANCE";
    case su_LOG_CONTEXT_RENDERER_SHADERS:
        return "RENDERER_SHADERS";
    case su_LOG_CONTEXT_RENDERER_TEXTURES:
        return "RENDERER_TEXTURES";
    case su_LOG_CONTEXT_RENDERER_BUFFERS:
        return "RENDERER_BUFFERS";
    case su_LOG_CONTEXT_RENDERER_LIGHTS:
        return "RENDERER_LIGHTS";
    case su_LOG_CONTEXT_RENDERER_CAMERA:
        return "RENDERER_CAMERA";
    case su_LOG_CONTEXT_ASSET_MANAGER:
        return "ASSET_MANAGER";
    case su_LOG_CONTEXT_MODEL_LOADING:
        return "MODEL_LOADING";
    case su_LOG_CONTEXT_TEXTURE_LOADING:
        return "TEXTURE_LOADING";
    case su_LOG_CONTEXT_SHADER_LOADING:
        return "SHADER_LOADING";
    case su_LOG_CONTEXT_AUDIO_LOADING:
        return "AUDIO_LOADING";
    case su_LOG_CONTEXT_SCENE_LOADING:
        return "SCENE_LOADING";
    case su_LOG_CONTEXT_INPUT:
        return "INPUT";
    case su_LOG_CONTEXT_INPUT_KEYBOARD:
        return "INPUT_KEYBOARD";
    case su_LOG_CONTEXT_INPUT_MOUSE:
        return "INPUT_MOUSE";
    case su_LOG_CONTEXT_INPUT_GAMEPAD:
        return "INPUT_GAMEPAD";
    case su_LOG_CONTEXT_LIB_OPENGL:
        return "LIB_OPENGL";
    case su_LOG_CONTEXT_LIB_STBI:
        return "LIB_STBI";
    case su_LOG_CONTEXT_LIB_GLFW:
        return "LIB_GLFW";
    case su_LOG_CONTEXT_BAD_PARAMS:
        return "BAD_PARAMS";
    case su_LOG_CONTEXT_UNCATEGORIZED:
        return "UNCATEGORIZED";
    }

    // Fallback for unknown values
    return "UNKNOWN";
}
