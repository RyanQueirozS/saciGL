#include "saci-utils/su-debug.h"

#include "glad/glad.h"
#include "saci-utils/su-types.h"

#include <stdio.h>

/* === Static vars=== */

static enum sa_Log_Severity_e __sa_logging_severity_s = sa_LOG_SEVERITY_LOW;
static sa_Bool_t __sa_should_log_source_s = sa_FALSE;

/* === Helpers === */

const char* __sa_Log_Type_To_String(enum sa_Log_Type_e type) {
    switch (type) {
    case sa_LOG_TYPE_ERROR:
        return "ERROR";
    case sa_LOG_TYPE_WARN:
        return "WARN";
    case sa_LOG_TYPE_INFO:
        return "INFO";
    case sa_LOG_TYPE_DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

const char* __sa_Log_Severity_To_String(enum sa_Log_Severity_e severity) {
    switch (severity) {
    case sa_LOG_SEVERITY_LOW:
        return "LOW";
    case sa_LOG_SEVERITY_MEDIUM:
        return "MEDIUM";
    case sa_LOG_SEVERITY_HIGH:
        return "HIGH";
    default:
        return "UNKOWN";
    }
}

const char* __sa_Log_Context_To_String(enum sa_Log_Context_e context) {
    switch (context) {
    case sa_LOG_CONTEXT_OPENGL:
        return "OpenGL";
    case sa_LOG_CONTEXT_OBJ_LOADING:
        return "OBJ_LOADING";
    case sa_LOG_CONTEXT_RENDERER:
        return "Renderer";
    case sa_LOG_CONTEXT_STBI:
        return "STBI";
    case sa_LOG_CONTEXT_MEMORY_ALLOCATION:
        return "MEMORY_ALLOC";
    default:
        return "UNKOWN";
    }
}

/* === Implementations=== */

void sa_Log_Should_Print_Origin(sa_Bool_t enable) {
    __sa_should_log_source_s = enable;
}

void sa_Log_Info(enum sa_Log_Type_e type, enum sa_Log_Context_e context,
                 const char* message, const char* file, int line) {
#if !(defined(SACI_DEBUG_MODE))
    if (type == sa_LOG_TYPE_DEBUG) {
        return;
    }
#endif

    if (__sa_should_log_source_s) {
        printf("[%s] %s: %s: [FILE:%s][LINE:%d]\n",
               __sa_Log_Context_To_String(context),
               __sa_Log_Type_To_String(type),
               message, file, line);
        return;
    }
    printf("[%s] %s: %s\n",
           __sa_Log_Context_To_String(context),
           __sa_Log_Type_To_String(type),
           message);
}

void sa_Log_Error(enum sa_Log_Type_e type,
                  enum sa_Log_Severity_e severity,
                  enum sa_Log_Context_e context,
                  const char* message, const char* file, int line) {
    if (severity < __sa_logging_severity_s)
        return;

    if (__sa_should_log_source_s) {
        printf("[%s] %s of %s severity: %s: [FILE:%s][LINE:%d]\n",
               __sa_Log_Context_To_String(context),
               __sa_Log_Type_To_String(type),
               __sa_Log_Severity_To_String(severity),
               message, file, line);
        return;
    }
    printf("[%s] %s of %s severity: %s\n",
           __sa_Log_Context_To_String(context),
           __sa_Log_Type_To_String(type),
           __sa_Log_Severity_To_String(severity),
           message);
}

void sa_OpenGL_Debug_Message_Callback(sa_U32_t source, sa_U32_t type, sa_U32_t id, sa_U32_t severity,
                                      int length, const char* msg, const void* data) {
    (void)length, (void)data;
    char* _source;
    char* _type;
    char* _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n", id, _type, _severity, _source, msg);
}
