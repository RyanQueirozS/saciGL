// TODO uniform_struct_block needs to have an option to be freed, but also need
// to evaludate if it will free the user's uniform_struct_block. Perhaps a copy
// would be nice
#include <glad/glad.h>

#include "saci-backend/sc-gl.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

/* === OpenGL === */

SA_API void sc_GL_Uniform_Set_Value(const sa_s32 location, sa_dataType type, const void* value) {
    switch (type) {
    case SA_TYPE_U8:
    case SA_TYPE_U16:
    case SA_TYPE_U32:
    case SA_TYPE_U64:
    case SA_TYPE_SHADERID:
    case SA_TYPE_TEXTUREID:
    case SA_TYPE_BUFFERID:
        glUniform1ui(location, *(const GLuint*)value);
        break;

    case SA_TYPE_BOOL:
    case SA_TYPE_S8:
    case SA_TYPE_S16:
    case SA_TYPE_S32:
    case SA_TYPE_S64:
        glUniform1i(location, *(const GLint*)value);
        break;

    case SA_TYPE_UV:
    case SA_TYPE_VEC2:
        glUniform2f(location, ((const GLfloat*)value)[0], ((const GLfloat*)value)[1]);
        break;

    case SA_TYPE_VEC3:
        glUniform3f(location,
                    ((const float*)value)[0],
                    ((const float*)value)[1],
                    ((const float*)value)[2]);
        break;

    case SA_TYPE_VEC4:
    case SA_TYPE_COLOR:
        glUniform4f(location,
                    ((const float*)value)[0],
                    ((const float*)value)[1],
                    ((const float*)value)[2],
                    ((const float*)value)[3]);
        break;

    case SA_TYPE_MAT2:
        glUniformMatrix2fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT3:
        glUniformMatrix3fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT4:
        glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT2X3:
        glUniformMatrix2x3fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT2X4:
        glUniformMatrix2x4fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT3X2:
        glUniformMatrix3x2fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT3X4:
        glUniformMatrix3x4fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT4X2:
        glUniformMatrix4x2fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT4X3:
        glUniformMatrix4x3fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    default:
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", type);
        break;
    }
}

SA_API void sc_GL_Resize_Vertex_Buffer(sa_u32 vao_id, sa_u32 vbo_id, sa_u64 new_size) {
    glBindVertexArray(vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sa_Scast_To_m(sa_s64)(new_size), NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SA_API sa_u32 sc_GL_Create_Index_Buffer_Dynamic(sa_u32* indices, sa_u64 indice_amount) {
    sa_u32 ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sa_Scast_To_m(sa_s64)(indice_amount * sizeof(sa_u32)), &indices[0],
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API sa_u32 sc_GL_Create_Index_Buffer_Static(sa_u32* indices, sa_u64 indice_amount) {
    sa_u32 ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sa_Scast_To_m(sa_s64)(indice_amount * sizeof(sa_u32)), &indices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API void sc_GL_Create_Vertex_Array(sa_u64 amount, sa_u32* arrays) {
    glGenVertexArrays(sa_Scast_To_m(int)(amount), arrays);
}

SA_API void sc_GL_Bind_Vertex_Array(sa_u32 array) {
    glBindVertexArray(array);
}

SA_API void sc_GL_Bind_Vertex_Buffer(sa_u32 vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

SA_API sa_u32 sc_GL_Create_Vertex_Buffer(sa_u64 size, const void* data, sa_u32 usage) {
    sa_u32 vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sa_Scast_To_m(long int)(size), data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

SA_API void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32 index, int size, sa_u32 type, sa_bool normalized, sa_u64 stride, void* ptr) {
    glVertexAttribPointer(index, size, type, normalized, sa_Scast_To_m(int)(stride), ptr);
}

void sc_GL_Enable_Vertex_Attrib_Array(sa_u32 id) {
    glEnableVertexAttribArray(id);
}

/* === GL Implementation === */

SA_INTERNAL sa_u32 s_Shader_Compile(const char* shader_source, sa_u32 shader_type);

sa_u32 sc_Shader_Compile_Shader_Vert(const char* source) {
    return s_Shader_Compile(source, GL_VERTEX_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Frag(const char* source) {
    return s_Shader_Compile(source, GL_FRAGMENT_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Geom(const char* source) {
    return s_Shader_Compile(source, GL_GEOMETRY_SHADER);
}

sa_u32 sc_Shader_Create_Shader_Program(sa_u32 vshader, sa_u32 fshader) {
    sa_u32 program_id = glCreateProgram();
    glAttachShader(program_id, vshader);
    glAttachShader(program_id, fshader);
    glLinkProgram(program_id);

    sa_s32 success = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        glGetProgramInfoLog(program_id, 2048, &size_returned, gl_err_message);
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    glDetachShader(program_id, vshader);
    glDetachShader(program_id, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    sa_Log_InfoF_Print_m(sa_LOG_CONTEXT_OPENGL,
                         "Shader program %d loaded successfully", program_id);
    return program_id;
}

sa_u32 sc_Shader_Create_Shader_Program_Geom(sa_u32 vshader, sa_u32 fshader, sa_u32 gshader) {
    sa_u32 program_id = glCreateProgram();
    glAttachShader(program_id, vshader);
    glAttachShader(program_id, fshader);
    glAttachShader(program_id, gshader);
    glLinkProgram(program_id);

    sa_s32 success = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        glGetProgramInfoLog(program_id, 2048, &size_returned, gl_err_message);
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    glDetachShader(program_id, vshader);
    glDetachShader(program_id, fshader);
    glDetachShader(program_id, gshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteShader(gshader);
    sa_Log_InfoF_Print_m(sa_LOG_CONTEXT_OPENGL,
                         "Shader program %d loaded successfully", program_id);

    return program_id;
}

SA_API sa_s32 sc_GL_Uniform_Location(sa_shaderId program_id, const char* const name) {
    return glGetUniformLocation(program_id, name);
}

/* === GL Helper ===  */

SA_INTERNAL sa_u32 s_Shader_Compile(const char* shader_source, sa_u32 shader_type) {
    sa_u32 shader_id = glCreateShader(shader_type);

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
            if (shader_type == GL_GEOMETRY_SHADER) {
                log_message = "Geometry shader couldn't be loaded";
            }
            sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, log_message);
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
        if (shader_type == GL_GEOMETRY_SHADER) {
            log_message = "Geometry shader loaded succesfully";
        }
        sa_Log_Info_Print_m(sa_LOG_CONTEXT_OPENGL, log_message);
    }

    return shader_id;
}
