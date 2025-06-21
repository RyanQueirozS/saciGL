#include "saci-utils/su-debug.h"

#include <glad/glad.h>
#include "saci-utils/su-types.h"

#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* === Static vars=== */

static enum su_logSeverity sa_logging_severity_s = su_LOG_SEVERITY_LOW;

static sa_bool sa_should_log_source_s = sa_FALSE;

/* === Helpers === */

static const char* sa_Log_Severity_To_String_s(enum su_logSeverity severity) {
    switch (severity) {
    case su_LOG_SEVERITY_LOW:
        return "LOW";
    case su_LOG_SEVERITY_MEDIUM:
        return "MEDIUM";
    case su_LOG_SEVERITY_HIGH:
        return "HIGH";
    default:
        return "UNKOWN";
    }
}

static const char* sa_Log_Context_To_String_s(enum su_logContext context) {
    switch (context) {
    case su_LOG_CONTEXT_OPENGL:
        return "OpenGL";
    case su_LOG_CONTEXT_MODEL_LOADING:
        return "MODEL_LOADING";
    case su_LOG_CONTEXT_RENDERER:
        return "Renderer";
    case su_LOG_CONTEXT_STBI:
        return "STBI";
    case su_LOG_CONTEXT_MEMORY:
        return "MEMORY_ALLOC";
    case su_LOG_CONTEXT_CONFIG:
        return "CONFIG";
    }
    // There isn't a "default" case because clang sees that
    // and doesn't recomend adding 'new' cases, where as that
    // should generate a warning
    return "UNKNOWN";
}

/* === Implementations=== */

void su_Log_Should_Print_Origin(sa_bool enable) {
    sa_should_log_source_s = enable;
}

void su_Log_Info(enum su_logContext context,
                 const char* message, const char* file, int line) {
    if (sa_should_log_source_s) {
        printf("INFO: [%s]: %s: [FILE:%s][LINE:%d]\n",
               sa_Log_Context_To_String_s(context),
               message, file, line);
        return;
    }
    printf("INFO: [%s]: %s\n",
           sa_Log_Context_To_String_s(context),
           message);
}

void su_Log_Debug(enum su_logDebugType type, enum su_logContext context,
                  const char* message, const char* file, int line) {
    char type_str[32];
    switch (type) {
    case su_LOG_DEBUG_TYPE_WINDOWING:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_WINDOWING)
            strcpy(type_str, "WINDOWING");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] of %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] of %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);
#endif
            break;
        }
    case su_LOG_DEBUG_TYPE_TEXTURE:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_TEXTURE)
            strcpy(type_str, "TEXTURE");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] of %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] of %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);
#endif
            break;
        }
    case su_LOG_DEBUG_TYPE_MODEL:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_MODEL)
            strcpy(type_str, "MODEL");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] of %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] of %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);
#endif
            break;
        }
    case su_LOG_DEBUG_TYPE_OPENGL:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_OPENGL)
            strcpy(type_str, "OPENGL");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] of %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] of %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);

#endif
            break;
        }
    case su_LOG_DEBUG_TYPE_RENDERER:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_RENDERER)
            strcpy(type_str, "RENDERER");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);

#endif
            break;
        }
    case su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_RENDERER_FUNCTIONS)
            strcpy(type_str, "RENDERER_FUNCTIONS");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);

#endif
            break;
        }
    case su_LOG_DEBUG_TYPE_RENDERER_BATCH:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_RENDERER_BATCH)
            strcpy(type_str, "RENDERER_BATCH");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);

#endif
            break;
        }
    case su_LOG_DEBUG_TYPE_RENDERER_CALL:
        {
#if defined(SACI_DEBUG_MODE_ALL) || defined(SACI_DEBUG_MODE_RENDERER_CALL)
            strcpy(type_str, "RENDERER_CALL");
            if (sa_should_log_source_s) {
                printf("DEBUG %s: [%s] %s: [FILE:%s][LINE:%d]\n",
                       type_str, sa_Log_Context_To_String_s(context),
                       message, file, line);
                return;
            }
            printf("DEBUG %s: [%s] %s\n",
                   type_str, sa_Log_Context_To_String_s(context),
                   message);

#endif
            break;
        }
    default:
        {
            printf("INVALID DEBUG MESSAGE AT: %s : %d", __FILE__, __LINE__);
            exit(1);
        }
    }
}

void su_Log_Warn(enum su_logSeverity severity,
                 enum su_logContext context,
                 const char* message, const char* file, int line) {
    if (severity < sa_logging_severity_s) {
        return;
    }

    if (sa_should_log_source_s) {
        printf("ERROR: [%s] %s severity: %s: [FILE:%s][LINE:%d]\n",
               sa_Log_Context_To_String_s(context),
               sa_Log_Severity_To_String_s(severity),
               message, file, line);
        if (severity >= su_LOG_SEVERITY_HIGH) {
            exit(EXIT_FAILURE);
        }
        return;
    }
    printf("ERROR: [%s] of %s severity: %s\n",
           sa_Log_Context_To_String_s(context),
           sa_Log_Severity_To_String_s(severity),
           message);
    if (severity >= su_LOG_SEVERITY_HIGH) {
        exit(EXIT_FAILURE);
    }
}

void su_Log_Error(enum su_logSeverity severity,
                  enum su_logContext context,
                  const char* message, const char* file, int line) {
    if (severity < sa_logging_severity_s) {
        return;
    }

    if (sa_should_log_source_s) {
        printf("ERROR: [%s] of %s severity: %s: [FILE:%s][LINE:%d]\n",
               sa_Log_Context_To_String_s(context),
               sa_Log_Severity_To_String_s(severity),
               message, file, line);
        if (severity >= su_LOG_SEVERITY_HIGH) {
            exit(EXIT_FAILURE);
        }
        return;
    }
    printf("ERROR: [%s] of %s severity: %s\n",
           sa_Log_Context_To_String_s(context),
           sa_Log_Severity_To_String_s(severity),
           message);
    if (severity >= su_LOG_SEVERITY_HIGH) {
        exit(EXIT_FAILURE);
    }
}

void sa_Log_OpenGL_Debug_Message_Callback(sa_u32 source, sa_u32 type, sa_u32 id, sa_u32 severity,
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
