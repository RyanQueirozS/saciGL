#include "../internal/gles3.h"

#include <GLES3/gl3.h>

#include "saci_util/internal/log.h"
#include "saci_util/log.h"

// Internal
SACI_INTERNAL SaciShaderId psaci__shader_compile(const char* shader_source, SaciU32 shader_type);

// Header IMPL

// Draw

SACI_API void psaci_emsdk_clear_color(const SaciColor color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

// Shader

SaciShaderId psaci_emsdk_shader_compile_shader_vert(const char* source)
{
    return psaci__shader_compile(source, GL_VERTEX_SHADER);
}

SaciShaderId psaci_emsdk_shader_compile_shader_frag(const char* source)
{
    return psaci__shader_compile(source, GL_FRAGMENT_SHADER);
}

SaciShaderId psaci_emsdk_shader_create_shader_program(SaciShaderId vshader, SaciShaderId fshader)
{
    SaciShaderId program_id = glCreateProgram();
    glAttachShader(program_id, vshader);
    glAttachShader(program_id, fshader);
    glLinkProgram(program_id);

    SaciS32 success = SACI_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        glGetProgramInfoLog(program_id, 2048, &size_returned, gl_err_message);
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_LIB_OPENGLES3, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    glDetachShader(program_id, vshader);
    glDetachShader(program_id, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    SACI_LOG_INFOF_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_LIB_OPENGLES3,
                     "Shader program %d loaded successfully", program_id);
    return program_id;
}

SaciShaderId psaci_emsdk_shader_create_program_code(const char* v, const char* f)
{
    SaciShaderId program = 0;
    SaciShaderId f_shader = psaci_emsdk_shader_compile_shader_frag(f);
    SaciShaderId v_shader = psaci_emsdk_shader_compile_shader_vert(v);
    program = psaci_emsdk_shader_create_shader_program(v_shader, f_shader);
    return program;
}

// Internal
SaciShaderId psaci__shader_compile(const char* shader_source, SaciU32 shader_type)
{
    SaciShaderId shader_id = glCreateShader(shader_type);

    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);

    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char err_message[2048];
        int size_returned = 0;
        glGetShaderInfoLog(shader_id, 2048, &size_returned, &err_message[0]);

        glDeleteShader(shader_id);
        { // Logging
            // TODO redo
            char* log_message = "";
            if (shader_type == GL_VERTEX_SHADER) {
                log_message = "Vertex shader couldn't be loaded";
            }
            if (shader_type == GL_FRAGMENT_SHADER) {
                log_message = "Fragment shader couldn't be loaded";
            }
            SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_LIB_OPENGLES3, log_message);
        }
        return 0;
    }
    {
        // TODO redo
        char* log_message = "";
        if (shader_type == GL_VERTEX_SHADER) {
            log_message = "Vertex shader loaded succesfully";
        }
        if (shader_type == GL_FRAGMENT_SHADER) {
            log_message = "Fragment shader loaded succesfully";
        }
        SACI_LOG_INFO_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_LIB_OPENGLES3, log_message);
    }

    return shader_id;
}
