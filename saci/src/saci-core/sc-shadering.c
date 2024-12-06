#include <glad/glad.h>
#include <stdio.h>
#include "saci-core/sc-gl.h"
#include "saci-utils/su-debug.h"

/**
 * @brief Compiles a OpenGL shader.
 *
 * @param shaderSource The source code of the shader.
 * @param shaderType The shader type (SACI_SHADER_VERTEX, SACI_SHADER_FRAGMENT...).
 */
saci_u32 __sc_shader_compile(const char* shaderSource, saci_u32 shaderType);

saci_u32 sc_Shader_CompileShaderV(const char* source) {
    return __sc_shader_compile(source, GL_VERTEX_SHADER);
}

saci_u32 sc_Shader_CompileShaderF(const char* source) {
    return __sc_shader_compile(source, GL_FRAGMENT_SHADER);
}

saci_u32 sc_Shader_CompileShaderG(const char* source) {
    return __sc_shader_compile(source, GL_GEOMETRY_SHADER);
}

saci_u32 sc_Shader_GetShaderProgram(saci_u32 vshader, saci_u32 fshader) {
    saci_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glLinkProgram(programID);

    saci_s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char glErrMessage[1024];
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, glErrMessage);
        snprintf(errMessage, sizeof(errMessage), "Shader program couldn't be loaded: %s",
                 glErrMessage);
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL,
                        "Shader program loaded successfully");
    return programID;
}

saci_u32 sc_Shader_GetShaderProgramg(saci_u32 vshader, saci_u32 fshader, saci_u32 gshader) {
    saci_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glAttachShader(programID, gshader);
    glLinkProgram(programID);

    saci_s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char glErrMessage[1024];
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, glErrMessage);
        snprintf(errMessage, sizeof(errMessage), "Shader program couldn't be loaded: %s",
                 glErrMessage);
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDetachShader(programID, gshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteShader(gshader);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL,
                        "Shader program be loaded successfully");

    return programID;
}

// helper
saci_u32 __sc_shader_compile(const char* shaderSource, saci_u32 shaderType) {
    saci_u32 shaderID = glCreateShader(shaderType);

    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetShaderInfoLog(shaderID, 2048, &sizeReturned, &errMessage[0]);

        glDeleteShader(shaderID);
        { // Logging
            char* logMessage = "";
            if (shaderType == GL_VERTEX_SHADER) {
                logMessage = "Vertex shader couldn't be loaded";
            }
            if (shaderType == GL_FRAGMENT_SHADER) {
                logMessage = "Fragment shader couldn't be loaded";
            }
            if (shaderType == GL_GEOMETRY_SHADER) {
                logMessage = "Geometry shader couldn't be loaded";
            }
            sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
                                 sa_LOG_CONTEXT_OPENGL, logMessage);
        }
        return 0;
    }
    {
        char* logMessage = "";
        if (shaderType == GL_VERTEX_SHADER) {
            logMessage = "Vertex shader loaded succesfully";
        }
        if (shaderType == GL_FRAGMENT_SHADER) {
            logMessage = "Fragment shader loaded succesfully";
        }
        if (shaderType == GL_GEOMETRY_SHADER) {
            logMessage = "Geometry shader loaded succesfully";
        }
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO,
                            sa_LOG_CONTEXT_OPENGL, logMessage);
    }

    return shaderID;
}
