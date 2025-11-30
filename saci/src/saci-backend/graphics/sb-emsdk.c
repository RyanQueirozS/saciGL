#include "./sb-emsdk.h"

#include <GLES3/gl3.h>

#include "saci-utils/su-log.h"

// Internal
SA_INTERNAL su_ShaderId sb__shader_compile(const char* shader_source, su_U32 shader_type);

// Header IMPL

su_ShaderId sb_emsdk_shader_compile_shader_vert(const char* source)
{
    return sb__shader_compile(source, GL_VERTEX_SHADER);
}

su_ShaderId sb_emsdk_shader_compile_shader_frag(const char* source)
{
    return sb__shader_compile(source, GL_FRAGMENT_SHADER);
}

su_ShaderId sb_emsdk_shader_create_shader_program(su_ShaderId vshader, su_ShaderId fshader)
{
    su_ShaderId program_id = glCreateProgram();
    glAttachShader(program_id, vshader);
    glAttachShader(program_id, fshader);
    glLinkProgram(program_id);

    su_S32 success = su_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        glGetProgramInfoLog(program_id, 2048, &size_returned, gl_err_message);
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_LIB_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    glDetachShader(program_id, vshader);
    glDetachShader(program_id, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    su_LOG_INFOF_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_LIB_OPENGL,
                   "Shader program %d loaded successfully", program_id);
    return program_id;
}

su_ShaderId sb_emsdk_shader_create_program_code(const char* v, const char* f)
{
    su_ShaderId program = 0;
    su_ShaderId f_shader = sb_emsdk_shader_compile_shader_frag(f);
    su_ShaderId v_shader = sb_emsdk_shader_compile_shader_vert(v);
    program = sb_emsdk_shader_create_shader_program(v_shader, f_shader);
    return program;
}

// Internal
su_ShaderId sb__shader_compile(const char* shader_source, su_U32 shader_type)
{
    su_ShaderId shader_id = glCreateShader(shader_type);

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
            su_LOG_ERROR_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_LIB_OPENGL, log_message);
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
        su_LOG_INFO_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_LIB_OPENGL, log_message);
    }

    return shader_id;
}
