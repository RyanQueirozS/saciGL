// TODO uniform_struct_block needs to have an option to be freed, but also need
// to evaludate if it will free the user's uniform_struct_block. Perhaps a copy
// would be nice
#include "../internal/gl4.h"

#include "saci_platform/dependencies/internal/dependency.h"
#include "saci_util/internal/general.h"
#include "saci_util/internal/log.h"
#include "saci_util/log.h"

#include <stdio.h>
#include <stdlib.h>

#define GL_LINK_STATUS 0x8B82

/* === Helper === */

// TODO should use the LOG funcs in saci_util
SACI_STATIC void psaci__opengl_debug_message_callback(SaciU32 source, SaciU32 type, SaciU32 id, SaciU32 severity,
                                                      int length, const char* msg, const void* data)
{
    (void)length, (void)data;
    char* print_src;
    char* print_type;
    char* print_severity;

    switch (source) {
    case PSACI_GL_DEBUG_SOURCE_API:
        print_src = "API";
        break;

    case PSACI_GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        print_src = "WINDOW SYSTEM";
        break;

    case PSACI_GL_DEBUG_SOURCE_SHADER_COMPILER:
        print_src = "SHADER COMPILER";
        break;

    case PSACI_GL_DEBUG_SOURCE_THIRD_PARTY:
        print_src = "THIRD PARTY";
        break;

    case PSACI_GL_DEBUG_SOURCE_APPLICATION:
        print_src = "APPLICATION";
        break;

    case PSACI_GL_DEBUG_SOURCE_OTHER:
    default:
        print_src = "UNKNOWN";
        break;
    }

    switch (type) {
    case PSACI_GL_DEBUG_TYPE_ERROR:
        print_type = "ERROR";
        break;

    case PSACI_GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        print_type = "DEPRECATED BEHAVIOR";
        break;

    case PSACI_GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        print_type = "UDEFINED BEHAVIOR";
        break;

    case PSACI_GL_DEBUG_TYPE_PORTABILITY:
        print_type = "PORTABILITY";
        break;

    case PSACI_GL_DEBUG_TYPE_PERFORMANCE:
        print_type = "PERFORMANCE";
        break;

    case PSACI_GL_DEBUG_TYPE_OTHER:
        print_type = "OTHER";
        break;

    case PSACI_GL_DEBUG_TYPE_MARKER:
        print_type = "MARKER";
        break;

    default:
        print_type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case PSACI_GL_DEBUG_SEVERITY_HIGH:
        print_severity = "HIGH";
        break;

    case PSACI_GL_DEBUG_SEVERITY_MEDIUM:
        print_severity = "MEDIUM";
        break;

    case PSACI_GL_DEBUG_SEVERITY_LOW:
        print_severity = "LOW";
        break;

    case PSACI_GL_DEBUG_SEVERITY_NOTIFICATION:
        print_severity = "NOTIFICATION";
        break;

    default:
        print_severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n", id, print_type, print_severity, print_src, msg);
}

/* === OpenGL === */

SACI_INTERNAL struct PSaciRenderApiFuncs psaci_g_gl_funcs;

SaciU32 psaci_gl_type_to_gl(const SaciDataType data_type)
{
    switch (data_type) {
    case SACI_TYPE_U8:
        return PSACI_GL_UNSIGNED_BYTE;
    case SACI_TYPE_U16:
        return PSACI_GL_UNSIGNED_SHORT;
    case SACI_TYPE_U32:
    case SACI_TYPE_U64:
        return PSACI_GL_UNSIGNED_INT;

    case SACI_TYPE_S8:
        return PSACI_GL_BYTE;
    case SACI_TYPE_S16:
        return PSACI_GL_SHORT;
    case SACI_TYPE_S32:
    case SACI_TYPE_S64:
        return PSACI_GL_INT;

    case SACI_TYPE_BOOL:
        return PSACI_GL_BOOL;

    case SACI_TYPE_SHADERID:
    case SACI_TYPE_TEXTUREID:
    case SACI_TYPE_BUFFERID:
        return PSACI_GL_UNSIGNED_INT;

    case SACI_TYPE_UV:
    case SACI_TYPE_VEC2:
    case SACI_TYPE_VEC3:
    case SACI_TYPE_VEC4:
    case SACI_TYPE_COLOR:
    case SACI_TYPE_MAT2:
    case SACI_TYPE_MAT3:
    case SACI_TYPE_MAT4:
    case SACI_TYPE_MAT2X3:
    case SACI_TYPE_MAT2X4:
    case SACI_TYPE_MAT3X2:
    case SACI_TYPE_MAT3X4:
    case SACI_TYPE_MAT4X2:
    case SACI_TYPE_MAT4X3:
        return PSACI_GL_FLOAT;

    case SACI_TYPE_SAMPLER2D:
        return PSACI_GL_SAMPLER_2D;

    default:
        return 0;
    }
}

void psaci_gl_load(void)
{
    psaci_g_gl_funcs = psaci_dependencies_get_render_api_funcs();
}

void psaci_gl_enable(SaciU32 flag)
{
    psaci_g_gl_funcs.gl.enable(flag);
}

void psaci_gl_uniform_set_value(const SaciS32 location, SaciDataType type, const void* value)
{
    switch (type) {
    case SACI_TYPE_U8:
    case SACI_TYPE_U16:
    case SACI_TYPE_U32:
    case SACI_TYPE_U64:
    case SACI_TYPE_SHADERID:
    case SACI_TYPE_TEXTUREID:
    case SACI_TYPE_BUFFERID:
        psaci_g_gl_funcs.gl.uniform1ui(location, *(const SaciU32*)value);
        break;

    case SACI_TYPE_BOOL:
    case SACI_TYPE_S8:
    case SACI_TYPE_S16:
    case SACI_TYPE_S32:
    case SACI_TYPE_S64:
        psaci_g_gl_funcs.gl.uniform1i(location, *(const SaciS32*)value);
        break;

    case SACI_TYPE_UV:
    case SACI_TYPE_VEC2:
        psaci_g_gl_funcs.gl.uniform2f(location, ((const float*)value)[0], ((const float*)value)[1]);
        break;

    case SACI_TYPE_VEC3:
        psaci_g_gl_funcs.gl.uniform3f(location,
                                      ((const float*)value)[0],
                                      ((const float*)value)[1],
                                      ((const float*)value)[2]);
        break;

    case SACI_TYPE_VEC4:
    case SACI_TYPE_COLOR:
        psaci_g_gl_funcs.gl.uniform4f(location,
                                      ((const float*)value)[0],
                                      ((const float*)value)[1],
                                      ((const float*)value)[2],
                                      ((const float*)value)[3]);
        break;

    case SACI_TYPE_MAT2:
        psaci_g_gl_funcs.gl.uniform_matrix_2fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT3:
        psaci_g_gl_funcs.gl.uniform_matrix_3fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT4:
        psaci_g_gl_funcs.gl.uniform_matrix_4fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT2X3:
        psaci_g_gl_funcs.gl.uniform_matrix_2x3fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT2X4:
        psaci_g_gl_funcs.gl.uniform_matrix_2x4fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT3X2:
        psaci_g_gl_funcs.gl.uniform_matrix_3x2fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT3X4:
        psaci_g_gl_funcs.gl.uniform_matrix_3x4fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT4X2:
        psaci_g_gl_funcs.gl.uniform_matrix_4x2fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    case SACI_TYPE_MAT4X3:
        psaci_g_gl_funcs.gl.uniform_matrix_4x3fv(location, 1, SACI_G_FALSE, (const float*)value);
        break;

    default:
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH, SACI_LOG_CONTEXT_LIB_OPENGL4,
                          "Invalid type %d for uniform", type);
        break;
    }
}

void psaci_gl_resize_vertex_buffer(SaciU32 vao_id, SaciU32 vbo_id, SaciU64 new_size)
{
    psaci_g_gl_funcs.gl.bind_vertex_array(vao_id);

    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ARRAY_BUFFER, vbo_id);
    psaci_g_gl_funcs.gl.buffer_data(PSACI_GL_ARRAY_BUFFER, SACI_CAST_M(SaciS64)(new_size), NULL, PSACI_GL_DYNAMIC_DRAW);

    psaci_g_gl_funcs.gl.bind_vertex_array(0);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ARRAY_BUFFER, 0);
}

SaciU32 psaci_gl_create_index_buffer_dynamic(SaciU64 indice_amount, SaciU32* indices)
{
    SaciU32 ibo;
    psaci_g_gl_funcs.gl.gen_buffers(1, &ibo);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ELEMENT_ARRAY_BUFFER, ibo);
    psaci_g_gl_funcs.gl.buffer_data(PSACI_GL_ELEMENT_ARRAY_BUFFER, SACI_CAST_M(SaciS64)(indice_amount * sizeof(SaciU32)), &indices[0],
                                    PSACI_GL_DYNAMIC_DRAW);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SaciU32 psaci_gl_create_index_buffer_static(SaciU64 indice_amount, SaciU32* indices)
{
    SaciU32 ibo;
    psaci_g_gl_funcs.gl.gen_buffers(1, &ibo);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ELEMENT_ARRAY_BUFFER, ibo);
    psaci_g_gl_funcs.gl.buffer_data(PSACI_GL_ELEMENT_ARRAY_BUFFER, SACI_CAST_M(SaciS64)(indice_amount * sizeof(SaciU32)), &indices[0],
                                    PSACI_GL_STATIC_DRAW);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

void psaci_gl_create_vertex_array(SaciU64 amount, SaciU32* arrays)
{
    psaci_g_gl_funcs.gl.gen_vertex_arrays(SACI_CAST_M(int)(amount), arrays);
}

void psaci_gl_bind_vertex_array(SaciU32 array)
{
    psaci_g_gl_funcs.gl.bind_vertex_array(array);
}

void psaci_gl_bind_vertex_buffer(SaciU32 vbo)
{
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ARRAY_BUFFER, vbo);
}

void psaci_gl_bind_index_buffer(SaciU32 ibo)
{
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ELEMENT_ARRAY_BUFFER, ibo);
}

SaciU32 psaci_gl_create_vertex_buffer_dynamic(SaciU64 size, const void* data)
{
    SaciU32 vbo = 0;
    psaci_g_gl_funcs.gl.gen_buffers(1, &vbo);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ARRAY_BUFFER, vbo);
    psaci_g_gl_funcs.gl.buffer_data(PSACI_GL_ARRAY_BUFFER, SACI_CAST_M(long int)(size), data, PSACI_GL_DYNAMIC_DRAW);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ARRAY_BUFFER, 0);
    return vbo;
}

SaciU32 psaci_gl_create_vertex_buffer_static(SaciU64 size, const void* data)
{
    SaciU32 vbo = 0;
    psaci_g_gl_funcs.gl.gen_buffers(1, &vbo);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ARRAY_BUFFER, vbo);
    psaci_g_gl_funcs.gl.buffer_data(PSACI_GL_ARRAY_BUFFER, SACI_CAST_M(long int)(size), data, PSACI_GL_STATIC_DRAW);
    psaci_g_gl_funcs.gl.bind_buffer(PSACI_GL_ARRAY_BUFFER, 0);
    return vbo;
}

void psaci_gl_set_vertex_attrib_pointer(SaciU32 index, int size, SaciU32 type, SaciBool normalized, SaciU64 stride, void* ptr)
{
    psaci_g_gl_funcs.gl.vertex_attrib_pointer(index, (SaciS32)((float)size / sizeof(float)), type, normalized, SACI_CAST_M(int)(stride), ptr);
}

void psaci_gl_vertex_attrib_divisor(SaciU32 id, SaciU32 div)
{
    psaci_g_gl_funcs.gl.vertex_attrib_divisor(id, div);
}

void psaci_gl_enable_vertex_attrib_array(SaciU32 id)
{
    psaci_g_gl_funcs.gl.enable_vertex_attrib_array(id);
}

/* === GL Implementation === */

SACI_INTERNAL SaciShaderId psaci__shader_compile(const char* shader_source, SaciU32 shader_type);

SaciShaderId psaci_gl_shader_compile_shader_vert(const char* source)
{
    return psaci__shader_compile(source, PSACI_GL_VERTEX_SHADER);
}

SaciShaderId psaci_gl_shader_compile_shader_frag(const char* source)
{
    return psaci__shader_compile(source, PSACI_GL_FRAGMENT_SHADER);
}

SaciShaderId psaci_gl_shader_compile_shader_geom(const char* source)
{
    return psaci__shader_compile(source, PSACI_GL_GEOMETRY_SHADER);
}

SaciShaderId psaci_gl_shader_create_shader_program(SaciShaderId vshader, SaciShaderId fshader)
{
    SaciShaderId program_id = psaci_g_gl_funcs.gl.create_program();
    psaci_g_gl_funcs.gl.attach_shader(program_id, vshader);
    psaci_g_gl_funcs.gl.attach_shader(program_id, fshader);
    psaci_g_gl_funcs.gl.link_program(program_id);

    SaciS32 success = SACI_G_FALSE;
    psaci_g_gl_funcs.gl.get_program_iv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        psaci_g_gl_funcs.gl.get_program_info_log(program_id, 2048, &size_returned, gl_err_message);
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_LIB_OPENGL4, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    psaci_g_gl_funcs.gl.detach_shader(program_id, vshader);
    psaci_g_gl_funcs.gl.detach_shader(program_id, fshader);
    psaci_g_gl_funcs.gl.delete_shader(vshader);
    psaci_g_gl_funcs.gl.delete_shader(fshader);

    SACI_LOG_INFOF_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_LIB_OPENGL4,
                     "Shader program %d loaded successfully", program_id);
    return program_id;
}

SaciShaderId psaci_gl_shader_create_shader_program_geom(SaciShaderId vshader, SaciShaderId fshader, SaciShaderId gshader)
{
    SaciShaderId program_id = psaci_g_gl_funcs.gl.create_program();
    psaci_g_gl_funcs.gl.attach_shader(program_id, vshader);
    psaci_g_gl_funcs.gl.attach_shader(program_id, fshader);
    psaci_g_gl_funcs.gl.attach_shader(program_id, gshader);
    psaci_g_gl_funcs.gl.link_program(program_id);

    SaciS32 success = SACI_G_FALSE;
    psaci_g_gl_funcs.gl.get_program_iv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        psaci_g_gl_funcs.gl.get_program_info_log(program_id, 2048, &size_returned, gl_err_message);
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_LIB_OPENGL4, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    psaci_g_gl_funcs.gl.detach_shader(program_id, vshader);
    psaci_g_gl_funcs.gl.detach_shader(program_id, fshader);
    psaci_g_gl_funcs.gl.detach_shader(program_id, gshader);
    psaci_g_gl_funcs.gl.delete_shader(vshader);
    psaci_g_gl_funcs.gl.delete_shader(fshader);
    psaci_g_gl_funcs.gl.delete_shader(gshader);
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_LIB_OPENGL4,
                     "Shader program %d loaded successfully", program_id);

    return program_id;
}

SaciShaderId psaci_gl_shader_create_shader_program_source(const char* v, const char* f, const char* g)
{
    SaciShaderId program = 0;
    SaciShaderId f_shader = psaci_gl_shader_compile_shader_frag(f);
    SaciShaderId v_shader = psaci_gl_shader_compile_shader_vert(v);
    if (g) {
        SaciShaderId g_shader = psaci_gl_shader_compile_shader_geom(g);
        program = psaci_gl_shader_create_shader_program_geom(v_shader,
                                                             f_shader,
                                                             g_shader);
    }
    program = psaci_gl_shader_create_shader_program(v_shader, f_shader);
    return program;
}

SaciS32 psaci_gl_uniform_location(SaciShaderId program_id, const char* const name)
{
    return psaci_g_gl_funcs.gl.get_uniform_location(program_id, name);
}

void psaci_gl_clear_color(const SaciColor color)
{
    psaci_g_gl_funcs.gl.clear_color(color.r, color.g, color.b, color.a);
    psaci_g_gl_funcs.gl.clear(PSACI_GL_COLOR_BUFFER_BIT);
}

void psaci_gl_clear_depth_buffer(void)
{
    psaci_g_gl_funcs.gl.clear(PSACI_GL_DEPTH_BUFFER_BIT);
}

void psaci_gl_use_program(SaciU32 program)
{
    psaci_g_gl_funcs.gl.use_program(program);
}

void psaci_gl_set_vertex_buffer_subdata(SaciS64 offset, SaciU64 size,
                                        const void* data)
{
    psaci_g_gl_funcs.gl.buffer_subdata(PSACI_GL_ARRAY_BUFFER, offset,
                                       SACI_CAST_M(unsigned int)(size), data);
}

void psaci_gl_set_index_buffer_subdata(SaciS64 offset, SaciU64 size, const void* data)
{
    psaci_g_gl_funcs.gl.buffer_subdata(PSACI_GL_ELEMENT_ARRAY_BUFFER, offset,
                                       SACI_CAST_M(unsigned int)(size), data);
}

void psaci_gl_draw_elements(SaciU32 primitives, SaciU64 count, SaciU32 type,
                            void* data, SaciU64 instance_count)
{
    if (instance_count > 0) {
        psaci_g_gl_funcs.gl.draw_elements_instanced(
            primitives, (SaciS32)count, type, data, (SaciS32)instance_count);
        return;
    }
    psaci_g_gl_funcs.gl.draw_elements(primitives, (SaciS32)count, type,
                                      data);
}

void psaci_gl_generate_textures(int count, SaciU32* tex_array_out)
{
    psaci_g_gl_funcs.gl.gen_textures(count, tex_array_out);
}

void psaci_gl_generate_mipmap_2d(const SaciU32 tex)
{
    SaciS32 prev_tex;
    psaci_g_gl_funcs.gl.get_integer_v(PSACI_GL_TEXTURE_BINDING_2D, &prev_tex);

    psaci_g_gl_funcs.gl.bind_texture(PSACI_GL_TEXTURE_2D, tex);

    psaci_g_gl_funcs.gl.generate_mipmap(PSACI_GL_TEXTURE_2D);

    psaci_g_gl_funcs.gl.bind_texture(PSACI_GL_TEXTURE_2D, (SaciU32)prev_tex);
}

void psaci_gl_upload_texture_2d(const SaciU32 tex, SaciS32 format, int width, int height, const void* data)
{
    psaci_g_gl_funcs.gl.bind_texture(PSACI_GL_TEXTURE_2D, tex);
    psaci_g_gl_funcs.gl.tex_image_2d(PSACI_GL_TEXTURE_2D, 0,
                                     format, width, height, 0,
                                     (SaciU32)format, PSACI_GL_UNSIGNED_BYTE, data);
}

void psaci_gl_get_texture_size_2d(const SaciU32 tex, int* width_out, int* height_out)
{

    SaciS32 prev_tex;
    psaci_g_gl_funcs.gl.get_integer_v(PSACI_GL_TEXTURE_BINDING_2D, &prev_tex);

    psaci_g_gl_funcs.gl.bind_texture(PSACI_GL_TEXTURE_2D, tex);

    psaci_g_gl_funcs.gl.get_texlevel_parameter_iv(PSACI_GL_TEXTURE_2D, 0, PSACI_GL_TEXTURE_WIDTH, width_out);
    psaci_g_gl_funcs.gl.get_texlevel_parameter_iv(PSACI_GL_TEXTURE_2D, 0, PSACI_GL_TEXTURE_HEIGHT, height_out);

    psaci_g_gl_funcs.gl.bind_texture(PSACI_GL_TEXTURE_2D, (SaciU32)prev_tex);
}

void psaci_gl_delete_texture(int count, SaciU32* tex_array_out)
{
    psaci_g_gl_funcs.gl.delete_textures(count, tex_array_out);
}

void psaci_gl_bind_texture_2d(enum PSaciGLConstants texture_loc, const SaciU32 tex)
{
    psaci_g_gl_funcs.gl.active_texture(texture_loc);
    psaci_g_gl_funcs.gl.bind_texture(PSACI_GL_TEXTURE_2D, tex);
}

void psaci_gl_initialized_debugger(void* debug_func)
{
    psaci_g_gl_funcs.gl.enable(PSACI_GL_DEBUG_OUTPUT);
    psaci_g_gl_funcs.gl.enable(PSACI_GL_DEBUG_OUTPUT_SYNCHRONOUS);
    psaci_g_gl_funcs.gl.enable(PSACI_GL_DEBUG_OUTPUT_SYNCHRONOUS);
    if (debug_func) {
        psaci_g_gl_funcs.gl.debug_message_callback((void*)debug_func, NULL);
    } else {
        psaci_g_gl_funcs.gl.debug_message_callback((void*)psaci__opengl_debug_message_callback, NULL);
    }
    SACI_LOG_INFO_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_LIB_OPENGL4, "Loaded opengl debug message callback");
}

/* === GL Helper ===  */

SACI_INTERNAL SaciShaderId psaci__shader_compile(const char* shader_source, SaciU32 shader_type)
{
    SaciShaderId shader_id = psaci_g_gl_funcs.gl.create_shader(shader_type);

    psaci_g_gl_funcs.gl.shader_source(shader_id, 1, &shader_source, NULL);
    psaci_g_gl_funcs.gl.compile_shader(shader_id);

    int success;
    psaci_g_gl_funcs.gl.get_shaderiv(shader_id, PSACI_GL_COMPILE_STATUS, &success);
    if (!success) {
        char err_message[2048];
        int size_returned = 0;
        psaci_g_gl_funcs.gl.get_shader_info_log(shader_id, 2048, &size_returned, &err_message[0]);

        psaci_g_gl_funcs.gl.delete_shader(shader_id);
        { // Logging
            // TODO redo
            char* log_message = "";
            if (shader_type == PSACI_GL_VERTEX_SHADER) {
                log_message = "Vertex shader couldn't be loaded";
            }
            if (shader_type == PSACI_GL_FRAGMENT_SHADER) {
                log_message = "Fragment shader couldn't be loaded";
            }
            if (shader_type == PSACI_GL_GEOMETRY_SHADER) {
                log_message = "Geometry shader couldn't be loaded";
            }
            SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_LIB_OPENGL4, log_message);
        }
        return 0;
    }
    {
        // TODO redo
        char* log_message = "";
        if (shader_type == PSACI_GL_VERTEX_SHADER) {
            log_message = "Vertex shader loaded succesfully";
        }
        if (shader_type == PSACI_GL_FRAGMENT_SHADER) {
            log_message = "Fragment shader loaded succesfully";
        }
        if (shader_type == PSACI_GL_GEOMETRY_SHADER) {
            log_message = "Geometry shader loaded succesfully";
        }
        SACI_LOG_INFO_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_LIB_OPENGL4, log_message);
    }

    return shader_id;
}
