#include "saci-utils/su-log.h"
#include <saci-utils/su-types-common.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// INTERNAL

SA_INTERNAL enum su_LogType su__log_type = su_LOG_TYPE_DEV;

SA_INTERNAL void su__log_default_event_callback(const struct su_LogEvent* event);
SA_INTERNAL void su__log_default_crash_callback(const su_LogError* error);
SA_INTERNAL void su__log_default_assertion_fail_callback(const su_LogAssertion* assertion);
SA_INTERNAL void su__log_default_dummy_check_fail_callback(const su_LogDummyCheck* dummy_check);

SA_INTERNAL void su__log_copy_safe_str(char* dest_out, const char* src, su_U64 max, const char* fallback);
SA_INTERNAL void su__log_call_event_callback(const struct su_LogEvent* event);
SA_INTERNAL void su__log_push_error_event(su_LogError* err);
SA_INTERNAL void su__log_push_warning_event(su_LogWarning* war);
SA_INTERNAL void su__log_push_info_event(su_LogInfo* info);
SA_INTERNAL void su__log_push_assertion_event(su_LogAssertion* assertion);
SA_INTERNAL void su__log_push_dummy_check_event(su_LogDummyCheck* dummy_check);

SA_INTERNAL struct su_LogBuffer {
    su_LogError error_array[su_LOG_ERROR_MAX_COUNT];
    su_LogWarning warning_array[su_LOG_ERROR_MAX_COUNT];
    su_LogInfo info_array[su_LOG_ERROR_MAX_COUNT];
    su_LogAssertion assertion_array[su_LOG_ERROR_MAX_COUNT];
    su_LogDummyCheck dummy_check_array[su_LOG_ERROR_MAX_COUNT];
} su__log_buffer = {0};

SA_INTERNAL struct su_LogConfig {
    su_LogEventCallback event_callback;
    su_LogCrashCallback crash_callback;
    su_LogAssertionFailCallback assertion_fail_callback;
    su_LogDummyCheckFailCallback dummy_check_fail_callback;

    su_U64 error_count;
    su_U64 warning_count;
    su_U64 info_count;
    su_U64 assertion_count;
    su_U64 dummy_check_count;
} su__log_config = {
    .event_callback = su__log_default_event_callback,
    .crash_callback = su__log_default_crash_callback,
    .assertion_fail_callback = su__log_default_assertion_fail_callback,
    .dummy_check_fail_callback = su__log_default_dummy_check_fail_callback,
};

/* === Header implementation === */

enum su_LogType su_log_get_current_type(void)
{
    return su__log_type;
}

void su_log_set_type(enum su_LogType type)
{
    su__log_type = type;
}

const su_LogError* su_log_get_last_error(void)
{
    su_LogError* err = &su__log_buffer.error_array[su__log_config.error_count];
    if (su__log_config.error_count != 0) {
        --su__log_config.error_count;
    }
    return err;
}

void su_log_error(const enum su_LogType type, const enum su_LogErrorSeverity severity, const enum su_LogContext context, const char* message, const char* file, const int line)
{
    su_LogError err = (su_LogError){
        .type = type,
        .severity = severity,
        .context = context,
        .line = line,
    };
    su__log_copy_safe_str(err.file, file, su_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    su__log_copy_safe_str(err.message, message, su_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    su__log_push_error_event(&err);
}

void su_log_warn(const enum su_LogType type, const enum su_LogWarnSeverity severity, const enum su_LogContext context, const char* message, const char* file, const int line)
{
    su_LogWarning warn = (su_LogWarning){
        .type = type,
        .severity = severity,
        .context = context,
        .line = line,
    };
    su__log_copy_safe_str(warn.file, file, su_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    su__log_copy_safe_str(warn.message, message, su_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    su__log_push_warning_event(&warn);
}

void su_log_info(const enum su_LogType type, const enum su_LogContext context, const char* message, const char* file, const int line)
{
    su_LogInfo info = {
        .type = type,
        .context = context,
        .line = line,
    };

    su__log_copy_safe_str(info.file, file, su_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    su__log_copy_safe_str(info.message, message, su_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    su__log_push_info_event(&info);
}

void su_log_assert(const su_Bool condition, const enum su_LogContext context, const char* message, const char* file, const int line)
{
    su_LogAssertion assertion = {
        .passed = condition,
        .context = context,
        .line = line,
    };
    su__log_copy_safe_str(assertion.file, file, su_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    su__log_copy_safe_str(assertion.message, message, su_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    su__log_push_assertion_event(&assertion);
}

void su_log_dummy_check(const su_Bool condition, const enum su_LogContext context, const char* message, const char* file, const int line)
{
    su_LogDummyCheck dummy_check = {
        .passed = condition,
        .context = context,
        .line = line,
    };

    su__log_copy_safe_str(dummy_check.file, file, su_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    su__log_copy_safe_str(dummy_check.message, message, su_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    su__log_push_dummy_check_event(&dummy_check);
}

void su_log_set_event_callback(su_LogEventCallback event_callback)
{
    su__log_config.event_callback = event_callback;
}

void su_log_set_crash_callback(su_LogCrashCallback crash_callback)
{
    su__log_config.crash_callback = crash_callback;
}

void su_log_set_assertion_fail_callback(su_LogAssertionFailCallback assertion_fail_callback)
{
    su__log_config.assertion_fail_callback = assertion_fail_callback;
}

void su_log_set_dummy_check_fail_callback(su_LogDummyCheckFailCallback dummy_check_fail_callback)
{
    su__log_config.dummy_check_fail_callback = dummy_check_fail_callback;
}

const char* su_log_severity_as_str(const enum su_LogErrorSeverity severity)
{
    switch (severity) {
    case su_LOG_ERROR_SEVERITY_LOW:
        return "LOW";
    case su_LOG_ERROR_SEVERITY_MEDIUM:
        return "MEDIUM";
    case su_LOG_ERROR_SEVERITY_HIGH:
        return "HIGH";
    case su_LOG_ERROR_SEVERITY_CRASH:
        return "CRASH";
    }
    return "UNKOWN";
}

const char* su_log_context_as_str(const enum su_LogContext context)
{
    switch (context) {
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
    case su_LOG_CONTEXT_CORE_DARRAY:
        return "CORE_DARRAY";
    case su_LOG_CONTEXT_CORE_MEMORY_MANAGER:
        return "CORE_MEMORY_MANAGER";
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
    case su_LOG_CONTEXT_WINDOWING:
        return "WINDOWING";
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
    case su_LOG_CONTEXT_GFX:
        return "GFX";
    case su_LOG_CONTEXT_MAIN_SHAPES_INIT:
        return "MAIN_SHAPES_INIT";
    case su_LOG_CONTEXT_MAIN_SHAPES_DRAW:
        return "MAIN_SHAPES_DRAW";
        break;
    }

    // Fallback for unknown values
    return "UNKNOWN";
}

// INTERNAL

SA_INTERNAL void su__log_default_event_callback(const struct su_LogEvent* event)
{
    switch (event->EVENT_TYPE) {
    case su_LOG_EVENT_ERROR:
        {
            const su_LogError error = *event->data.error;
            if (error.type < su_log_get_current_type()) {
                break;
            }
            printf("ERROR [%s]: %s at %s:%d\n",
                   su_log_context_as_str(error.context), error.message,
                   error.file, error.line);
            break;
        }
    case su_LOG_EVENT_WARNING:
        {
            const su_LogWarning warning = *event->data.warning;
            if (warning.type < su_log_get_current_type()) {
                break;
            }
            printf("WARNING [%s]: %s at %s:%d\n",
                   su_log_context_as_str(warning.context), warning.message,
                   warning.file, warning.line);
            break;
        }
    case su_LOG_EVENT_INFO:
        {
            const su_LogInfo info = *event->data.info;
            if (info.type < su_log_get_current_type()) {
                break;
            }
            printf("INFO [%s]: %s\n",
                   su_log_context_as_str(info.context), info.message);
            break;
        }
    case su_LOG_EVENT_ASSERTION: // Normally don't print assertions, perhaps a debug build should have it
        break;
    case su_LOG_EVENT_DUMMY_CHECK:
        break;
    }
}

SA_INTERNAL void su__log_default_crash_callback(const su_LogError* error)
{
    if (su_log_get_current_type() == su_LOG_TYPE_PROD) {
        printf("A fatal error occoured, please contact the product owner\n");
        return;
    }
    printf("FATAL [%s] ERROR: %s at %s:%d\n", su_log_context_as_str(error->context), error->message, error->file, error->line);
}

SA_INTERNAL void su__log_default_assertion_fail_callback(const su_LogAssertion* assertion)
{
    if (su_log_get_current_type() == su_LOG_TYPE_PROD) {
        printf("An assertion failed, please contact the product owner\n");
        return;
    }
    printf("ASSERTION FAIL [%s]: %s at %s:%d\n", su_log_context_as_str(assertion->context), assertion->message, assertion->file, assertion->line);
}

SA_INTERNAL void su__log_default_dummy_check_fail_callback(const su_LogDummyCheck* dummy_check)
{
    if (su_log_get_current_type() == su_LOG_TYPE_PROD) {
        printf("A dummy check failed, please contact the product owner\n");
        return;
    }
    printf("DUMMY CHECK FAIL [%s]: %s at %s:%d\n", su_log_context_as_str(dummy_check->context), dummy_check->message, dummy_check->file, dummy_check->line);
}

SA_INTERNAL void su__log_copy_safe_str(char* dest_out, const char* src, su_U64 max, const char* fallback)
{
    if (!src) {
        su_U64 str_len = strnlen(fallback, max);
        if (str_len >= max) {
            str_len = max - 1;
        }
        strncpy(dest_out, fallback, str_len);
        dest_out[str_len] = '\0';
        return;
    }
    su_U64 str_len = strnlen(src, max);
    if (str_len >= max) {
        str_len = max - 1;
    }
    strncpy(dest_out, src, str_len);
    dest_out[str_len] = '\0';
}

SA_INTERNAL void su__log_call_event_callback(const struct su_LogEvent* event)
{
    if (su__log_config.event_callback) {
        su__log_config.event_callback(event);
    }
}

SA_INTERNAL void su__log_push_error_event(su_LogError* err)
{
    if (err->severity == su_LOG_ERROR_SEVERITY_CRASH) {
        if (su__log_config.crash_callback) {
            su__log_config.crash_callback(err);
        }
        abort();
    }
    if (su__log_config.error_count == su_LOG_ERROR_MAX_COUNT - 1) {
        su__log_config.error_count = 0;
    }
    su__log_buffer.error_array[su__log_config.error_count++] = *err;

    struct su_LogEvent event = {
        .data = {.error = err},
        .EVENT_TYPE = su_LOG_EVENT_ERROR,
    };
    su__log_call_event_callback(&event);
}

SA_INTERNAL void su__log_push_warning_event(su_LogWarning* war)
{
    if (su__log_config.warning_count == su_LOG_ERROR_MAX_COUNT - 1) {
        su__log_config.warning_count = 0;
    }
    su__log_buffer.warning_array[su__log_config.warning_count++] = *war;
    struct su_LogEvent event = {
        .data = {.warning = war},
        .EVENT_TYPE = su_LOG_EVENT_WARNING,
    };
    su__log_call_event_callback(&event);
}

SA_INTERNAL void su__log_push_info_event(su_LogInfo* info)
{
    if (su__log_config.info_count == su_LOG_ERROR_MAX_COUNT - 1) {
        su__log_config.info_count = 0;
    }
    su__log_buffer.info_array[su__log_config.info_count++] = *info;
    struct su_LogEvent event = {
        .data = {.info = info},
        .EVENT_TYPE = su_LOG_EVENT_INFO,
    };
    su__log_call_event_callback(&event);
}

SA_INTERNAL void su__log_push_assertion_event(su_LogAssertion* assertion)
{
    if (!assertion->passed) {
        if (su__log_config.assertion_fail_callback) {
            su__log_config.assertion_fail_callback(assertion);
        }
        abort();
    }
    if (su__log_config.assertion_count == su_LOG_ERROR_MAX_COUNT - 1) {
        su__log_config.assertion_count = 0;
    }
    su__log_buffer.assertion_array[su__log_config.assertion_count++] = *assertion;
    struct su_LogEvent event = {
        .data = {.assertion = assertion},
        .EVENT_TYPE = su_LOG_EVENT_ASSERTION,
    };
    su__log_call_event_callback(&event);
}

SA_INTERNAL void su__log_push_dummy_check_event(su_LogDummyCheck* dummy_check)
{
    if (!dummy_check->passed) {
        if (su__log_config.dummy_check_fail_callback) {
            su__log_config.dummy_check_fail_callback(dummy_check);
        }
        abort();
    }
    if (su__log_config.dummy_check_count == su_LOG_ERROR_MAX_COUNT - 1) {
        su__log_config.dummy_check_count = 0;
    }
    su__log_buffer.dummy_check_array[su__log_config.dummy_check_count++] = *dummy_check;
    struct su_LogEvent event = {
        .data = {.dummy_check = dummy_check},
        .EVENT_TYPE = su_LOG_EVENT_ASSERTION,
    };
    su__log_call_event_callback(&event);
}
