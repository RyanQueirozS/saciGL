// TODO uniform_struct_block needs to have an option to be freed, but also need
// to evaludate if it will free the user's uniform_struct_block. Perhaps a copy
// would be nice
#include "saci-backend/graphics/sb-gl.h"

#include "saci-backend/resources/sb-dependency-internal.h"
#include <stdio.h>

#define GL_LINK_STATUS 0x8B82

/* === Helper === */

void sb__opengl_debug_message_callback(su_U32 source, su_U32 type, su_U32 id, su_U32 severity,
                                       int length, const char* msg, const void* data)
{
    (void)length, (void)data;
    char* _source;
    char* _type;
    char* _severity;

    switch (source) {
    case sb_GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case sb_GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case sb_GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case sb_GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case sb_GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case sb_GL_DEBUG_SOURCE_OTHER:
    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case sb_GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case sb_GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case sb_GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case sb_GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case sb_GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case sb_GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case sb_GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case sb_GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case sb_GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case sb_GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case sb_GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n", id, _type, _severity, _source, msg);
}

/* === OpenGL === */

SA_INTERNAL struct sb_RenderApiFuncs gl_funcs;

SA_API su_U32 sb_gl_type_to_gl(const su_DataType data_type)
{
    switch (data_type) {
    case su_TYPE_U8:
        return sb_GL_UNSIGNED_BYTE;
    case su_TYPE_U16:
        return sb_GL_UNSIGNED_SHORT;
    case su_TYPE_U32:
    case su_TYPE_U64:
        return sb_GL_UNSIGNED_INT;

    case su_TYPE_S8:
        return sb_GL_BYTE;
    case su_TYPE_S16:
        return sb_GL_SHORT;
    case su_TYPE_S32:
    case su_TYPE_S64:
        return sb_GL_INT;

    case su_TYPE_BOOL:
        return sb_GL_BOOL;

    case su_TYPE_SHADERID:
    case su_TYPE_TEXTUREID:
    case su_TYPE_BUFFERID:
        return sb_GL_UNSIGNED_INT;

    case su_TYPE_UV:
    case su_TYPE_VEC2:
    case su_TYPE_VEC3:
    case su_TYPE_VEC4:
    case su_TYPE_COLOR:
    case su_TYPE_MAT2:
    case su_TYPE_MAT3:
    case su_TYPE_MAT4:
    case su_TYPE_MAT2X3:
    case su_TYPE_MAT2X4:
    case su_TYPE_MAT3X2:
    case su_TYPE_MAT3X4:
    case su_TYPE_MAT4X2:
    case su_TYPE_MAT4X3:
        return sb_GL_FLOAT;

    case su_TYPE_SAMPLER2D:
        return sb_GL_SAMPLER_2D;

    default:
        return 0;
    }
}

void sb_gl_load(void)
{
    gl_funcs = sb_dependencies_get_render_api_funcs();
}

SA_API void sb_gl_uniform_set_value(const su_S32 location, su_DataType type, const void* value)
{
    switch (type) {
    case su_TYPE_U8:
    case su_TYPE_U16:
    case su_TYPE_U32:
    case su_TYPE_U64:
    case su_TYPE_SHADERID:
    case su_TYPE_TEXTUREID:
    case su_TYPE_BUFFERID:
        gl_funcs.gl.uniform1ui(location, *(const su_U32*)value);
        break;

    case su_TYPE_BOOL:
    case su_TYPE_S8:
    case su_TYPE_S16:
    case su_TYPE_S32:
    case su_TYPE_S64:
        gl_funcs.gl.uniform1i(location, *(const su_S32*)value);
        break;

    case su_TYPE_UV:
    case su_TYPE_VEC2:
        gl_funcs.gl.uniform2f(location, ((const float*)value)[0], ((const float*)value)[1]);
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
        gl_funcs.gl.uniform_matrix_2fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT3:
        gl_funcs.gl.uniform_matrix_3fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT4:
        gl_funcs.gl.uniform_matrix_4fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT2X3:
        gl_funcs.gl.uniform_matrix_2x3fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT2X4:
        gl_funcs.gl.uniform_matrix_2x4fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT3X2:
        gl_funcs.gl.uniform_matrix_3x2fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT3X4:
        gl_funcs.gl.uniform_matrix_3x4fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT4X2:
        gl_funcs.gl.uniform_matrix_4x2fv(location, 1, su_FALSE, (const float*)value);
        break;

    case su_TYPE_MAT4X3:
        gl_funcs.gl.uniform_matrix_4x3fv(location, 1, su_FALSE, (const float*)value);
        break;

    default:
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_LIB_OPENGL,
                        "Invalid type %d for uniform", type);
        break;
    }
}

SA_API void sb_gl_resize_vertex_buffer(su_U32 vao_id, su_U32 vbo_id, su_U64 new_size)
{
    gl_funcs.gl.bind_vertex_array(vao_id);

    gl_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, vbo_id);
    gl_funcs.gl.buffer_data(sb_GL_ARRAY_BUFFER, su_CAST_M(su_S64)(new_size), NULL, sb_GL_DYNAMIC_DRAW);

    gl_funcs.gl.bind_vertex_array(0);
    gl_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, 0);
}

SA_API su_U32 sb_gl_create_index_buffer_dynamic(su_U64 indice_amount, su_U32* indices)
{
    su_U32 ibo;
    gl_funcs.gl.gen_buffers(1, &ibo);
    gl_funcs.gl.bind_buffer(sb_GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_funcs.gl.buffer_data(sb_GL_ELEMENT_ARRAY_BUFFER, su_CAST_M(su_S64)(indice_amount * sizeof(su_U32)), &indices[0],
                            sb_GL_DYNAMIC_DRAW);
    gl_funcs.gl.bind_buffer(sb_GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API su_U32 sb_gl_create_index_buffer_static(su_U64 indice_amount, su_U32* indices)
{
    su_U32 ibo;
    gl_funcs.gl.gen_buffers(1, &ibo);
    gl_funcs.gl.bind_buffer(sb_GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_funcs.gl.buffer_data(sb_GL_ELEMENT_ARRAY_BUFFER, su_CAST_M(su_S64)(indice_amount * sizeof(su_U32)), &indices[0],
                            sb_GL_STATIC_DRAW);
    gl_funcs.gl.bind_buffer(sb_GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API void sb_gl_create_vertex_array(su_U64 amount, su_U32* arrays)
{
    gl_funcs.gl.gen_vertex_arrays(su_CAST_M(int)(amount), arrays);
}

SA_API void sb_gl_bind_vertex_array(su_U32 array)
{
    gl_funcs.gl.bind_vertex_array(array);
}

SA_API void sb_gl_bind_vertex_buffer(su_U32 vbo)
{
    gl_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, vbo);
}

SA_API su_U32 sb_gl_create_vertex_buffer_dynamic(su_U64 size, const void* data)
{
    su_U32 vbo = 0;
    gl_funcs.gl.gen_buffers(1, &vbo);
    gl_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, vbo);
    gl_funcs.gl.buffer_data(sb_GL_ARRAY_BUFFER, su_CAST_M(long int)(size), data, sb_GL_DYNAMIC_DRAW);
    gl_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, 0);
    return vbo;
}

SA_API su_U32 sb_gl_create_vertex_buffer_static(su_U64 size, const void* data)
{
    su_U32 vbo = 0;
    gl_funcs.gl.gen_buffers(1, &vbo);
    gl_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, vbo);
    gl_funcs.gl.buffer_data(sb_GL_ARRAY_BUFFER, su_CAST_M(long int)(size), data, sb_GL_STATIC_DRAW);
    gl_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, 0);
    return vbo;
}

SA_API void sb_gl_set_vertex_attrib_pointer(su_U32 index, int size, su_U32 type, su_Bool normalized, su_U64 stride, void* ptr)
{
    gl_funcs.gl.vertex_attrib_pointer(index, size, type, normalized, su_CAST_M(int)(stride), ptr);
}

void sb_gl_vertex_attrib_divisor(su_U32 id, su_U32 div)
{
    gl_funcs.gl.vertex_attrib_divisor(id, div);
}

void sb_gl_enable_vertex_attrib_array(su_U32 id)
{
    gl_funcs.gl.enable_vertex_attrib_array(id);
}

/* === GL Implementation === */

SA_INTERNAL su_ShaderId sb__shader_compile(const char* shader_source, su_U32 shader_type);

su_ShaderId sb_gl_shader_compile_shader_vert(const char* source)
{
    return sb__shader_compile(source, sb_GL_VERTEX_SHADER);
}

su_ShaderId sb_gl_shader_compile_shader_frag(const char* source)
{
    return sb__shader_compile(source, sb_GL_FRAGMENT_SHADER);
}

su_ShaderId sb_gl_shader_compile_shader_geom(const char* source)
{
    return sb__shader_compile(source, sb_GL_GEOMETRY_SHADER);
}

su_ShaderId sb_gl_shader_create_shader_program(su_ShaderId vshader, su_ShaderId fshader)
{
    su_ShaderId program_id = gl_funcs.gl.create_program();
    gl_funcs.gl.attach_shader(program_id, vshader);
    gl_funcs.gl.attach_shader(program_id, fshader);
    gl_funcs.gl.link_program(program_id);

    su_S32 success = su_FALSE;
    gl_funcs.gl.get_program_iv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        gl_funcs.gl.get_program_info_log(program_id, 2048, &size_returned, gl_err_message);
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_LIB_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    gl_funcs.gl.detach_shader(program_id, vshader);
    gl_funcs.gl.detach_shader(program_id, fshader);
    gl_funcs.gl.delete_shader(vshader);
    gl_funcs.gl.delete_shader(fshader);

    su_LOG_INFOF_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_LIB_OPENGL,
                   "Shader program %d loaded successfully", program_id);
    return program_id;
}

su_ShaderId sb_gl_shader_create_shader_program_geom(su_ShaderId vshader, su_ShaderId fshader, su_ShaderId gshader)
{
    su_ShaderId program_id = gl_funcs.gl.create_program();
    gl_funcs.gl.attach_shader(program_id, vshader);
    gl_funcs.gl.attach_shader(program_id, fshader);
    gl_funcs.gl.attach_shader(program_id, gshader);
    gl_funcs.gl.link_program(program_id);

    su_S32 success = su_FALSE;
    gl_funcs.gl.get_program_iv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        gl_funcs.gl.get_program_info_log(program_id, 2048, &size_returned, gl_err_message);
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_LIB_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    gl_funcs.gl.detach_shader(program_id, vshader);
    gl_funcs.gl.detach_shader(program_id, fshader);
    gl_funcs.gl.detach_shader(program_id, gshader);
    gl_funcs.gl.delete_shader(vshader);
    gl_funcs.gl.delete_shader(fshader);
    gl_funcs.gl.delete_shader(gshader);
    su_LOG_INFOF_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_LIB_OPENGL,
                   "Shader program %d loaded successfully", program_id);

    return program_id;
}

SA_API su_S32 sb_gl_uniform_location(su_ShaderId program_id, const char* const name)
{
    return gl_funcs.gl.get_uniform_location(program_id, name);
}

void sb_gl_clear_color(const su_Color color)
{
    gl_funcs.gl.clear_color(color.r, color.g, color.b, color.a);
    gl_funcs.gl.clear(sb_GL_COLOR_BUFFER_BIT);
}

void sb_gl_clear_depth_buffer(void)
{
    gl_funcs.gl.clear(sb_GL_DEPTH_BUFFER_BIT);
}

void sb_gl_initialized_debugger(void)
{
    gl_funcs.gl.enable(sb_GL_DEBUG_OUTPUT);
    gl_funcs.gl.enable(sb_GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl_funcs.gl.enable(sb_GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl_funcs.gl.debug_message_callback((void*)sb__opengl_debug_message_callback, NULL);
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_LIB_OPENGL, "Loaded opengl debug message callback");
}

/* === GL Helper ===  */

SA_INTERNAL su_ShaderId sb__shader_compile(const char* shader_source, su_U32 shader_type)
{
    su_ShaderId shader_id = gl_funcs.gl.create_shader(shader_type);

    gl_funcs.gl.shader_source(shader_id, 1, &shader_source, NULL);
    gl_funcs.gl.compile_shader(shader_id);

    int success;
    gl_funcs.gl.get_shaderiv(shader_id, sb_GL_COMPILE_STATUS, &success);
    if (!success) {
        char err_message[2048];
        int size_returned = 0;
        gl_funcs.gl.get_shader_info_log(shader_id, 2048, &size_returned, &err_message[0]);

        gl_funcs.gl.delete_shader(shader_id);
        { // Logging
            // TODO redo
            char* log_message = "";
            if (shader_type == sb_GL_VERTEX_SHADER) {
                log_message = "Vertex shader couldn't be loaded";
            }
            if (shader_type == sb_GL_FRAGMENT_SHADER) {
                log_message = "Fragment shader couldn't be loaded";
            }
            if (shader_type == sb_GL_GEOMETRY_SHADER) {
                log_message = "Geometry shader couldn't be loaded";
            }
            su_LOG_ERROR_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_LIB_OPENGL, log_message);
        }
        return 0;
    }
    {
        // TODO redo
        char* log_message = "";
        if (shader_type == sb_GL_VERTEX_SHADER) {
            log_message = "Vertex shader loaded succesfully";
        }
        if (shader_type == sb_GL_FRAGMENT_SHADER) {
            log_message = "Fragment shader loaded succesfully";
        }
        if (shader_type == sb_GL_GEOMETRY_SHADER) {
            log_message = "Geometry shader loaded succesfully";
        }
        su_LOG_INFO_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_LIB_OPENGL, log_message);
    }

    return shader_id;
}
