// TODO uniform_struct_block needs to have an option to be freed, but also need
// to evaludate if it will free the user's uniform_struct_block. Perhaps a copy
// would be nice
#include <saci-backend/sb-config-manager.h>
#include <stdio.h>

#include "saci-backend/sb-gl.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

/* === OpenGL === */

SA_INTERNAL struct sb_RenderApiFuncs gl_funcs;

void sb_gl_load(void) {
    gl_funcs = sb_cfg_manager_get_render_funcs();
}

SA_API void sb_gl_uniform_set_value(const su_S32 location, su_DataType type, const void* value) {
    switch (type) {
    case su_TYPE_U8:
    case su_TYPE_U16:
    case su_TYPE_U32:
    case su_TYPE_U64:
    case su_TYPE_SHADERID:
    case su_TYPE_TEXTUREID:
    case su_TYPE_BUFFERID:
        gl_funcs.gl.uniform1ui(location, *(const GLuint*)value);
        break;

    case su_TYPE_BOOL:
    case su_TYPE_S8:
    case su_TYPE_S16:
    case su_TYPE_S32:
    case su_TYPE_S64:
        gl_funcs.gl.uniform1i(location, *(const GLint*)value);
        break;

    case su_TYPE_UV:
    case su_TYPE_VEC2:
        gl_funcs.gl.uniform2f(location, ((const GLfloat*)value)[0], ((const GLfloat*)value)[1]);
        break;

    case su_TYPE_VEC3:
        gl_funcs.gl.uniform3f(location,
                              ((const float*)value)[0],
                              ((const float*)value)[1],
                              ((const float*)value)[2]);
        break;

    case su_TYPE_VEC4:
    case su_TYPE_COLOR:
        gl_funcs.gl.uniform4f(location,
                              ((const float*)value)[0],
                              ((const float*)value)[1],
                              ((const float*)value)[2],
                              ((const float*)value)[3]);
        break;

    case su_TYPE_MAT2:
        gl_funcs.gl.uniform_matrix_2fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT3:
        gl_funcs.gl.uniform_matrix_3fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT4:
        gl_funcs.gl.uniform_matrix_4fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT2X3:
        gl_funcs.gl.uniform_matrix_2x3fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT2X4:
        gl_funcs.gl.uniform_matrix_2x4fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT3X2:
        gl_funcs.gl.uniform_matrix_3x2fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT3X4:
        gl_funcs.gl.uniform_matrix_3x4fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT4X2:
        gl_funcs.gl.uniform_matrix_4x2fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case su_TYPE_MAT4X3:
        gl_funcs.gl.uniform_matrix_4x3fv(location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    default:
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", type);
        break;
    }
}

SA_API void sb_gl_resize_vertex_buffer(su_U32 vao_id, su_U32 vbo_id, su_U64 new_size) {
    gl_funcs.gl.bind_vertex_array(vao_id);

    gl_funcs.gl.bind_buffer(GL_ARRAY_BUFFER, vbo_id);
    gl_funcs.gl.buffer_data(GL_ARRAY_BUFFER, su_SCAST_TO_M(su_S64)(new_size), NULL, GL_DYNAMIC_DRAW);

    gl_funcs.gl.bind_vertex_array(0);
    gl_funcs.gl.bind_buffer(GL_ARRAY_BUFFER, 0);
}

SA_API su_U32 sb_gl_create_index_buffer_dynamic(su_U64 indice_amount, su_U32* indices) {
    su_U32 ibo;
    gl_funcs.gl.gen_buffers(1, &ibo);
    gl_funcs.gl.bind_buffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_funcs.gl.buffer_data(GL_ELEMENT_ARRAY_BUFFER, su_SCAST_TO_M(su_S64)(indice_amount * sizeof(su_U32)), &indices[0],
                            GL_DYNAMIC_DRAW);
    gl_funcs.gl.bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API su_U32 sb_gl_create_index_buffer_static(su_U64 indice_amount, su_U32* indices) {
    su_U32 ibo;
    gl_funcs.gl.gen_buffers(1, &ibo);
    gl_funcs.gl.bind_buffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_funcs.gl.buffer_data(GL_ELEMENT_ARRAY_BUFFER, su_SCAST_TO_M(su_S64)(indice_amount * sizeof(su_U32)), &indices[0],
                            GL_STATIC_DRAW);
    gl_funcs.gl.bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API void sb_gl_create_vertex_array(su_U64 amount, su_U32* arrays) {
    gl_funcs.gl.gen_vertex_arrays(su_SCAST_TO_M(int)(amount), arrays);
}

SA_API void sb_gl_bind_vertex_array(su_U32 array) {
    gl_funcs.gl.bind_vertex_array(array);
}

SA_API void sb_gl_bind_vertex_buffer(su_U32 vbo) {
    gl_funcs.gl.bind_buffer(GL_ARRAY_BUFFER, vbo);
}

SA_API su_U32 sb_gl_create_vertex_buffer_dynamic(su_U64 size, const void* data) {
    su_U32 vbo = 0;
    gl_funcs.gl.gen_buffers(1, &vbo);
    gl_funcs.gl.bind_buffer(GL_ARRAY_BUFFER, vbo);
    gl_funcs.gl.buffer_data(GL_ARRAY_BUFFER, su_SCAST_TO_M(long int)(size), data, GL_DYNAMIC_DRAW);
    gl_funcs.gl.bind_buffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

SA_API su_U32 sb_gl_create_vertex_buffer_static(su_U64 size, const void* data) {
    su_U32 vbo = 0;
    gl_funcs.gl.gen_buffers(1, &vbo);
    gl_funcs.gl.bind_buffer(GL_ARRAY_BUFFER, vbo);
    gl_funcs.gl.buffer_data(GL_ARRAY_BUFFER, su_SCAST_TO_M(long int)(size), data, GL_STATIC_DRAW);
    gl_funcs.gl.bind_buffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

SA_API void sb_gl_set_vertex_attrib_pointer(su_U32 index, int size, su_U32 type, su_Bool normalized, su_U64 stride, void* ptr) {
    gl_funcs.gl.vertex_attrib_pointer(index, size, type, normalized, su_SCAST_TO_M(int)(stride), ptr);
}

void sb_gl_vertex_attrib_divisor(su_U32 id, su_U32 div) {
    gl_funcs.gl.vertex_attrib_divisor(id, div);
}

void sb_gl_enable_vertex_attrib_array(su_U32 id) {
    gl_funcs.gl.enable_vertex_attrib_array(id);
}

/* === GL Implementation === */

SA_INTERNAL su_U32 sb__shader_compile(const char* shader_source, su_U32 shader_type);

su_U32 sb_shader_compile_shader_vert(const char* source) {
    return sb__shader_compile(source, GL_VERTEX_SHADER);
}

su_U32 sb_shader_compile_shader_frag(const char* source) {
    return sb__shader_compile(source, GL_FRAGMENT_SHADER);
}

su_U32 sb_shader_compile_shader_geom(const char* source) {
    return sb__shader_compile(source, GL_GEOMETRY_SHADER);
}

su_U32 sb_shader_create_shader_program(su_U32 vshader, su_U32 fshader) {
    su_U32 program_id = gl_funcs.gl.create_program();
    gl_funcs.gl.attach_shader(program_id, vshader);
    gl_funcs.gl.attach_shader(program_id, fshader);
    gl_funcs.gl.link_program(program_id);

    su_S32 success = GL_FALSE;
    gl_funcs.gl.get_programiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        gl_funcs.gl.get_program_info_log(program_id, 2048, &size_returned, gl_err_message);
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    gl_funcs.gl.detach_shader(program_id, vshader);
    gl_funcs.gl.detach_shader(program_id, fshader);
    gl_funcs.gl.delete_shader(vshader);
    gl_funcs.gl.delete_shader(fshader);

    su_LOG_INFOF_PRINT_M(su_LOG_CONTEXT_OPENGL,
                         "Shader program %d loaded successfully", program_id);
    return program_id;
}

su_U32 sb_shader_create_shader_program_geom(su_U32 vshader, su_U32 fshader, su_U32 gshader) {
    su_U32 program_id = gl_funcs.gl.create_program();
    gl_funcs.gl.attach_shader(program_id, vshader);
    gl_funcs.gl.attach_shader(program_id, fshader);
    gl_funcs.gl.attach_shader(program_id, gshader);
    gl_funcs.gl.link_program(program_id);

    su_S32 success = GL_FALSE;
    gl_funcs.gl.get_programiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        gl_funcs.gl.get_program_info_log(program_id, 2048, &size_returned, gl_err_message);
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    gl_funcs.gl.detach_shader(program_id, vshader);
    gl_funcs.gl.detach_shader(program_id, fshader);
    gl_funcs.gl.detach_shader(program_id, gshader);
    gl_funcs.gl.delete_shader(vshader);
    gl_funcs.gl.delete_shader(fshader);
    gl_funcs.gl.delete_shader(gshader);
    su_LOG_INFOF_PRINT_M(su_LOG_CONTEXT_OPENGL,
                         "Shader program %d loaded successfully", program_id);

    return program_id;
}

SA_API su_S32 sb_gl_uniform_location(su_ShaderId program_id, const char* const name) {
    return gl_funcs.gl.get_uniform_location(program_id, name);
}

/* === GL Helper ===  */

SA_INTERNAL su_U32 sb__shader_compile(const char* shader_source, su_U32 shader_type) {
    su_U32 shader_id = gl_funcs.gl.create_shader(shader_type);

    gl_funcs.gl.shader_source(shader_id, 1, &shader_source, NULL);
    gl_funcs.gl.compile_shader(shader_id);

    int success;
    gl_funcs.gl.get_shaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char err_message[2048];
        int size_returned = 0;
        gl_funcs.gl.get_shader_info_log(shader_id, 2048, &size_returned, &err_message[0]);

        gl_funcs.gl.delete_shader(shader_id);
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
            su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_OPENGL, log_message);
        }
        return 0;
    }
    printf("4\n");
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
        su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, log_message);
    }

    return shader_id;
}
