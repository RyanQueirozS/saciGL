#include "saci_util/log.h"
#include "saci_util/internal/log.h"

#include "saci_util/types.h"
#include "saci_util/defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SACI_LOG_ERROR_MAX_COUNT 128
#define SACI_LOG_WARNING_MAX_COUNT 128
#define SACI_LOG_INFO_MAX_COUNT 128
#define SACI_LOG_ASSERTION_MAX_COUNT 128
#define SACI_LOG_DUMMY_CHECK_MAX_COUNT 128

// INTERNAL

SACI_INTERNAL enum SaciLogType saci_g_saci_log_type = SACI_LOG_TYPE_DEV;

SACI_INTERNAL void saci__log_default_event_callback(const struct SaciLogEvent* event);
SACI_INTERNAL void saci__log_default_crash_callback(const SaciLogError* error);
SACI_INTERNAL void saci__log_default_assertion_fail_callback(const SaciLogAssertion* assertion);
SACI_INTERNAL void saci__log_default_dummy_check_fail_callback(const SaciLogDummyCheck* dummy_check);

SACI_INTERNAL void saci__log_copy_safe_str(char* dest_out, const char* src, SaciU64 max, const char* fallback);
SACI_INTERNAL void saci__log_call_event_callback(const struct SaciLogEvent* event);
SACI_INTERNAL void saci__log_push_error_event(SaciLogError* err);
SACI_INTERNAL void saci__log_push_warning_event(SaciLogWarning* war);
SACI_INTERNAL void saci__log_push_info_event(SaciLogInfo* info);
SACI_INTERNAL void saci__log_push_assertion_event(SaciLogAssertion* assertion);
SACI_INTERNAL void saci__log_push_dummy_check_event(SaciLogDummyCheck* dummy_check);

SACI_STATIC struct SaciLogBuffer {
    SaciLogError error_array[SACI_LOG_ERROR_MAX_COUNT];
    SaciLogWarning warning_array[SACI_LOG_ERROR_MAX_COUNT];
    SaciLogInfo info_array[SACI_LOG_ERROR_MAX_COUNT];
    SaciLogAssertion assertion_array[SACI_LOG_ERROR_MAX_COUNT];
    SaciLogDummyCheck dummy_check_array[SACI_LOG_ERROR_MAX_COUNT];
} saci_g_log_buffer = {0};

SACI_STATIC struct {
    SaciLogEventCallback event_callback;
    SaciLogCrashCallback crash_callback;
    SaciLogAssertionFailCallback assertion_fail_callback;
    SaciLogDummyCheckFailCallback dummy_check_fail_callback;

    SaciU64 error_count;
    SaciU64 warning_count;
    SaciU64 info_count;
    SaciU64 assertion_count;
    SaciU64 dummy_check_count;
} saci_g_log_config = {
    .event_callback = saci__log_default_event_callback,
    .crash_callback = saci__log_default_crash_callback,
    .assertion_fail_callback = saci__log_default_assertion_fail_callback,
    .dummy_check_fail_callback = saci__log_default_dummy_check_fail_callback,
};

/* === Header implementation === */

enum SaciLogType saci_log_get_current_type(void)
{
    return saci_g_saci_log_type;
}

void saci_log_set_type(enum SaciLogType type)
{
    saci_g_saci_log_type = type;
}

const SaciLogError* saci_log_get_last_error(void)
{
    SaciLogError* err = &saci_g_log_buffer.error_array[saci_g_log_config.error_count];
    if (saci_g_log_config.error_count != 0) {
        --saci_g_log_config.error_count;
    }
    return err;
}

void saci_log_error(const enum SaciLogType type, const enum SaciLogErrorSeverity severity, const enum SaciLogContext context, const char* message, const char* file, const int line)
{
    SaciLogError err = (SaciLogError){
        .type = type,
        .severity = severity,
        .context = context,
        .line = line,
    };
    saci__log_copy_safe_str(err.file, file, SACI_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    saci__log_copy_safe_str(err.message, message, SACI_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    saci__log_push_error_event(&err);
}

void saci_log_warn(const enum SaciLogType type, const enum SaciLogWarnSeverity severity, const enum SaciLogContext context, const char* message, const char* file, const int line)
{
    SaciLogWarning warn = (SaciLogWarning){
        .type = type,
        .severity = severity,
        .context = context,
        .line = line,
    };
    saci__log_copy_safe_str(warn.file, file, SACI_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    saci__log_copy_safe_str(warn.message, message, SACI_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    saci__log_push_warning_event(&warn);
}

void saci_log_info(const enum SaciLogType type, const enum SaciLogContext context, const char* message, const char* file, const int line)
{
    SaciLogInfo info = {
        .type = type,
        .context = context,
        .line = line,
    };

    saci__log_copy_safe_str(info.file, file, SACI_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    saci__log_copy_safe_str(info.message, message, SACI_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    saci__log_push_info_event(&info);
}

void saci_log_assert(const SaciBool condition, const enum SaciLogContext context, const char* message, const char* file, const int line)
{
    SaciLogAssertion assertion = {
        .passed = condition,
        .context = context,
        .line = line,
    };
    saci__log_copy_safe_str(assertion.file, file, SACI_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    saci__log_copy_safe_str(assertion.message, message, SACI_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    saci__log_push_assertion_event(&assertion);
}

void saci_log_dummy_check(const SaciBool condition, const enum SaciLogContext context, const char* message, const char* file, const int line)
{
    SaciLogDummyCheck dummy_check = {
        .passed = condition,
        .context = context,
        .line = line,
    };

    saci__log_copy_safe_str(dummy_check.file, file, SACI_LOG_FILE_CHAR_COUNT - 1, "UNKOWN FILE");
    saci__log_copy_safe_str(dummy_check.message, message, SACI_LOG_MESSAGE_CHAR_COUNT - 1, "NO MESSAGE");

    saci__log_push_dummy_check_event(&dummy_check);
}

void saci_log_set_event_callback(SaciLogEventCallback event_callback)
{
    saci_g_log_config.event_callback = event_callback;
}

void saci_log_set_crash_callback(SaciLogCrashCallback crash_callback)
{
    saci_g_log_config.crash_callback = crash_callback;
}

void saci_log_set_assertion_fail_callback(SaciLogAssertionFailCallback assertion_fail_callback)
{
    saci_g_log_config.assertion_fail_callback = assertion_fail_callback;
}

void saci_log_set_dummy_check_fail_callback(SaciLogDummyCheckFailCallback dummy_check_fail_callback)
{
    saci_g_log_config.dummy_check_fail_callback = dummy_check_fail_callback;
}

const char* saci_log_severity_as_str(const enum SaciLogErrorSeverity severity)
{
    switch (severity) {
    case SACI_LOG_ERROR_SEVERITY_LOW:
        return "LOW";
    case SACI_LOG_ERROR_SEVERITY_MEDIUM:
        return "MEDIUM";
    case SACI_LOG_ERROR_SEVERITY_HIGH:
        return "HIGH";
    case SACI_LOG_ERROR_SEVERITY_CRASH:
        return "CRASH";
    }
    return "UNKOWN";
}

const char* saci_log_context_as_str(const enum SaciLogContext context)
{
    switch (context) {
    case SACI_LOG_CONTEXT_CORE_INIT:
        return "CORE_INIT";
    case SACI_LOG_CONTEXT_CORE_SHUTDOWN:
        return "CORE_SHUTDOWN";
    case SACI_LOG_CONTEXT_CORE_MAINLOOP:
        return "CORE_MAINLOOP";
    case SACI_LOG_CONTEXT_CORE_EVENTS:
        return "CORE_EVENTS";
    case SACI_LOG_CONTEXT_CORE_CONFIG:
        return "CORE_CONFIG";
    case SACI_LOG_CONTEXT_CORE_MEMORY:
        return "CORE_MEMORY";
    case SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER:
        return "CORE_MEMORY_MANAGER";
    case SACI_LOG_CONTEXT_RENDERER:
        return "RENDERER";
    case SACI_LOG_CONTEXT_RENDERER_STATIC:
        return "RENDERER_STATIC";
    case SACI_LOG_CONTEXT_RENDERER_INSTANCE:
        return "RENDERER_INSTANCE";
    case SACI_LOG_CONTEXT_RENDERER_SHADERS:
        return "RENDERER_SHADERS";
    case SACI_LOG_CONTEXT_RENDERER_TEXTURES:
        return "RENDERER_TEXTURES";
    case SACI_LOG_CONTEXT_RENDERER_BUFFERS:
        return "RENDERER_BUFFERS";
    case SACI_LOG_CONTEXT_RENDERER_LIGHTS:
        return "RENDERER_LIGHTS";
    case SACI_LOG_CONTEXT_RENDERER_CAMERA:
        return "RENDERER_CAMERA";
    case SACI_LOG_CONTEXT_ASSET_MANAGER:
        return "ASSET_MANAGER";
    case SACI_LOG_CONTEXT_MODEL_LOADING:
        return "MODEL_LOADING";
    case SACI_LOG_CONTEXT_TEXTURE_LOADING:
        return "TEXTURE_LOADING";
    case SACI_LOG_CONTEXT_SHADER_LOADING:
        return "SHADER_LOADING";
    case SACI_LOG_CONTEXT_AUDIO_LOADING:
        return "AUDIO_LOADING";
    case SACI_LOG_CONTEXT_SCENE_LOADING:
        return "SCENE_LOADING";
    case SACI_LOG_CONTEXT_WINDOWING:
        return "WINDOWING";
    case SACI_LOG_CONTEXT_WINDOWING_RUNTIME:
        return "WINDOWING_RUNTIME";
    case SACI_LOG_CONTEXT_DEPENDENCIES:
        return "DEPENDENCIES";
    case SACI_LOG_CONTEXT_INPUT:
        return "INPUT";
    case SACI_LOG_CONTEXT_INPUT_KEYBOARD:
        return "INPUT_KEYBOARD";
    case SACI_LOG_CONTEXT_INPUT_MOUSE:
        return "INPUT_MOUSE";
    case SACI_LOG_CONTEXT_INPUT_GAMEPAD:
        return "INPUT_GAMEPAD";
    case SACI_LOG_CONTEXT_LIB_OPENGL4:
        return "LIB_OPENGL4";
    case SACI_LOG_CONTEXT_LIB_OPENGLES3:
        return "LIB_OPENGLES3";
    case SACI_LOG_CONTEXT_LIB_STBI:
        return "LIB_STBI";
    case SACI_LOG_CONTEXT_LIB_GLFW:
        return "LIB_GLFW";
    case SACI_LOG_CONTEXT_GFX:
        return "GFX";
    case SACI_LOG_CONTEXT_MAIN_SHAPES_INIT:
        return "MAIN_SHAPES_INIT";
    case SACI_LOG_CONTEXT_MAIN_SHAPES_DRAW:
        return "MAIN_SHAPES_DRAW";
        break;
    }

    // Fallback for unknown values
    return "UNKNOWN";
}

// INTERNAL

SACI_INTERNAL void saci__log_default_event_callback(const struct SaciLogEvent* event)
{
    switch (event->EVENT_TYPE) {
    case SACI_LOG_EVENT_ERROR:
        {
            const SaciLogError error = *event->data.error;
            if (error.type < saci_log_get_current_type()) {
                break;
            }
            printf("ERROR [%s]: %s at %s:%d\n",
                   saci_log_context_as_str(error.context), error.message,
                   error.file, error.line);
            break;
        }
    case SACI_LOG_EVENT_WARNING:
        {
            const SaciLogWarning warning = *event->data.warning;
            if (warning.type < saci_log_get_current_type()) {
                break;
            }
            printf("WARNING [%s]: %s at %s:%d\n",
                   saci_log_context_as_str(warning.context), warning.message,
                   warning.file, warning.line);
            break;
        }
    case SACI_LOG_EVENT_INFO:
        {
            const SaciLogInfo info = *event->data.info;
            if (info.type < saci_log_get_current_type()) {
                break;
            }
            printf("INFO [%s]: %s\n",
                   saci_log_context_as_str(info.context), info.message);
            break;
        }
    case SACI_LOG_EVENT_ASSERTION: // Normally don't print assertions, perhaps a debug build should have it
    case SACI_LOG_EVENT_DUMMY_CHECK:
        break;
    }
}

SACI_INTERNAL void saci__log_default_crash_callback(const SaciLogError* error)
{
    if (saci_log_get_current_type() == SACI_LOG_TYPE_PROD) {
        printf("A fatal error occoured, please contact the product owner\n");
        return;
    }
    printf("FATAL [%s] ERROR: %s at %s:%d\n", saci_log_context_as_str(error->context), error->message, error->file, error->line);
}

SACI_INTERNAL void saci__log_default_assertion_fail_callback(const SaciLogAssertion* assertion)
{
    if (saci_log_get_current_type() == SACI_LOG_TYPE_PROD) {
        printf("An assertion failed, please contact the product owner\n");
        return;
    }
    printf("ASSERTION FAIL [%s]: %s at %s:%d\n", saci_log_context_as_str(assertion->context), assertion->message, assertion->file, assertion->line);
}

SACI_INTERNAL void saci__log_default_dummy_check_fail_callback(const SaciLogDummyCheck* dummy_check)
{
    if (saci_log_get_current_type() == SACI_LOG_TYPE_PROD) {
        printf("A dummy check failed, please contact the product owner\n");
        return;
    }
    printf("DUMMY CHECK FAIL [%s]: %s at %s:%d\n", saci_log_context_as_str(dummy_check->context), dummy_check->message, dummy_check->file, dummy_check->line);
}

SACI_INTERNAL void saci__log_copy_safe_str(char* dest_out, const char* src, SaciU64 max, const char* fallback)
{
    if (!src) {
        SaciU64 str_len = strnlen(fallback, max);
        if (str_len >= max) {
            str_len = max - 1;
        }
        strncpy(dest_out, fallback, str_len);
        dest_out[str_len] = '\0';
        return;
    }
    SaciU64 str_len = strnlen(src, max);
    if (str_len >= max) {
        str_len = max - 1;
    }
    strncpy(dest_out, src, str_len);
    dest_out[str_len] = '\0';
}

SACI_INTERNAL void saci__log_call_event_callback(const struct SaciLogEvent* event)
{
    if (saci_g_log_config.event_callback) {
        saci_g_log_config.event_callback(event);
    }
}

SACI_INTERNAL void saci__log_push_error_event(SaciLogError* err)
{
    if (err->severity == SACI_LOG_ERROR_SEVERITY_CRASH) {
        if (saci_g_log_config.crash_callback) {
            saci_g_log_config.crash_callback(err);
        }
        abort();
    }
    if (saci_g_log_config.error_count == SACI_LOG_ERROR_MAX_COUNT - 1) {
        saci_g_log_config.error_count = 0;
    }
    saci_g_log_buffer.error_array[saci_g_log_config.error_count++] = *err;

    struct SaciLogEvent event = {
        .data = {.error = err},
        .EVENT_TYPE = SACI_LOG_EVENT_ERROR,
    };
    saci__log_call_event_callback(&event);
}

SACI_INTERNAL void saci__log_push_warning_event(SaciLogWarning* war)
{
    if (saci_g_log_config.warning_count == SACI_LOG_ERROR_MAX_COUNT - 1) {
        saci_g_log_config.warning_count = 0;
    }
    saci_g_log_buffer.warning_array[saci_g_log_config.warning_count++] = *war;
    struct SaciLogEvent event = {
        .data = {.warning = war},
        .EVENT_TYPE = SACI_LOG_EVENT_WARNING,
    };
    saci__log_call_event_callback(&event);
}

SACI_INTERNAL void saci__log_push_info_event(SaciLogInfo* info)
{
    if (saci_g_log_config.info_count == SACI_LOG_ERROR_MAX_COUNT - 1) {
        saci_g_log_config.info_count = 0;
    }
    saci_g_log_buffer.info_array[saci_g_log_config.info_count++] = *info;
    struct SaciLogEvent event = {
        .data = {.info = info},
        .EVENT_TYPE = SACI_LOG_EVENT_INFO,
    };
    saci__log_call_event_callback(&event);
}

SACI_INTERNAL void saci__log_push_assertion_event(SaciLogAssertion* assertion)
{
    if (!assertion->passed) {
        if (saci_g_log_config.assertion_fail_callback) {
            saci_g_log_config.assertion_fail_callback(assertion);
        }
        abort();
    }
    if (saci_g_log_config.assertion_count == SACI_LOG_ERROR_MAX_COUNT - 1) {
        saci_g_log_config.assertion_count = 0;
    }
    saci_g_log_buffer.assertion_array[saci_g_log_config.assertion_count++] = *assertion;
    struct SaciLogEvent event = {
        .data = {.assertion = assertion},
        .EVENT_TYPE = SACI_LOG_EVENT_ASSERTION,
    };
    saci__log_call_event_callback(&event);
}

SACI_INTERNAL void saci__log_push_dummy_check_event(SaciLogDummyCheck* dummy_check)
{
    if (!dummy_check->passed) {
        if (saci_g_log_config.dummy_check_fail_callback) {
            saci_g_log_config.dummy_check_fail_callback(dummy_check);
        }
        abort();
    }
    if (saci_g_log_config.dummy_check_count == SACI_LOG_ERROR_MAX_COUNT - 1) {
        saci_g_log_config.dummy_check_count = 0;
    }
    saci_g_log_buffer.dummy_check_array[saci_g_log_config.dummy_check_count++] = *dummy_check;
    struct SaciLogEvent event = {
        .data = {.dummy_check = dummy_check},
        .EVENT_TYPE = SACI_LOG_EVENT_ASSERTION,
    };
    saci__log_call_event_callback(&event);
}
