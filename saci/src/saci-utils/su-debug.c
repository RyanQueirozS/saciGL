#include "saci-utils/su-debug.h"

#include "glad/glad.h"
#include "saci-utils/su-types.h"

#include <stdio.h>

const char* __sa_Log_Level_To_String(enum sa_Log_Level level) {
    switch (level) {
    case sa_LOG_LEVEL_ERROR:
        return "ERROR";
    case sa_LOG_LEVEL_WARN:
        return "WARN";
    case sa_LOG_LEVEL_INFO:
        return "INFO";
    case sa_LOG_LEVEL_DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

const char* __sa_Log_Context_To_String(enum sa_Log_Context context) {
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

void sa_Log_Message(int level, int context, const char* message, const char* file, int line) {
    if (level == sa_LOG_LEVEL_WARN || level == sa_LOG_LEVEL_ERROR) {
        printf("%s: %s: %s: [FILE:%s][LINE:%d]\n", __sa_Log_Level_To_String(level),
               __sa_Log_Context_To_String(context), message, file, line);
        return;
    }
    printf("%s: %s: %s\n", __sa_Log_Level_To_String(level), __sa_Log_Context_To_String(context), message);
}

void sa_OpenGL_Debug_Message_Callback(saci_u32 source, saci_u32 type, saci_u32 id, saci_u32 severity,
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
