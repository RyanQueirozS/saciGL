#include "saci-backend/sb-gfx.h"

#include "saci-utils/su-general.h"
#include "saci-backend/sb-config-manager.h"
#include "saci-utils/su-log.h"
#include <saci-backend/sb-gl.h>
#include <saci-utils/su-types.h>
#include <stdio.h>

/* === Internal helper Declarations === */

enum sb_GLConstants {
    sb_GL_ARRAY_BUFFER = 0x8892,
    sb_GL_ELEMENT_ARRAY_BUFFER = 0x8893,
    sb_GL_DEPTH_TEST = 0x0B71,
    sb_GL_TEXTURE0 = 0x84C0,
    sb_GL_TEXTURE_2D = 0x0DE1,
    sb_GL_TEXTURE_WIDTH = 0x1000,
    sb_GL_TEXTURE_HEIGHT = 0x1001,
    sb_GL_TRIANGLES = 0x0004,
    sb_GL_UNSIGNED_INT = 0x1405,
    sb_GL_UNSIGNED_BYTE = 0x1401,
    sb_GL_UNIFORM_BUFFER = 0x8A11,
    sb_GL_STATIC_DRAW = 0x88E4,
    sb_GL_DYNAMIC_DRAW = 0x88E8,
    sb_GL_TEXTURE_BINDING_2D = 0x8069,
};

SA_STATIC struct sb_RenderApiFuncs render_funcs;
SA_STATIC enum sb_RendererApi render_api;

// Init
SA_INTERNAL void sb__gfx_gl_init_info(union sb_GFXInfo* info_out, const struct sb_RendererConfig cfg);

// Draw
SA_INTERNAL void sb__gfx_gl_draw(const union sb_GFXInfo* gfx_info, const struct sb_GFXDrawData* data);
SA_INTERNAL void sb__gfx_gl_set_uniform_from_uniform_data(const struct sb_GFXUniformData uniform_data);
SA_INTERNAL su_Bool sb__has_texture(union sb_GFXTexture* texture_array, su_U32 array_length);

/* === Header Impl === */

void sb_gfx_load(void) {
    render_funcs = sb_cfg_manager_get_render_funcs();
    render_api = sb_cfg_manager_get_renderer_api();
}

void sb_gfx_init_shader(union sb_GFXInfo* info_out, const struct sb_RendererConfig cfg) {
    su_LOG_ASSERT_M(cfg.shaders.vert, su_LOG_CONTEXT_GFX, "Vertex shader is empty or NULL");
    su_LOG_ASSERT_M(cfg.shaders.frag, su_LOG_CONTEXT_GFX, "Frag shader is empty or NULL");
    switch (render_api) {
    case sb_RENDERER_API_OPENGL:
        {
            su_ShaderId v_shader = sb_gl_shader_compile_shader_vert(su_string_data(cfg.shaders.vert));
            su_ShaderId f_shader = sb_gl_shader_compile_shader_frag(su_string_data(cfg.shaders.frag));
            if (cfg.shaders.geom) {
                su_ShaderId g_shader = sb_gl_shader_compile_shader_geom(su_string_data(cfg.shaders.geom));
                info_out->gl_data.shader_program = sb_gl_shader_create_shader_program_geom(v_shader,
                                                                                           f_shader,
                                                                                           g_shader);
                break;
            }
            info_out->gl_data.shader_program = sb_gl_shader_create_shader_program(v_shader, f_shader);
            break;
        }
    case sb_RENDERER_API_VULKAN:
        break;
    }
}

void sb_gfx_create(union sb_GFXInfo* info_out, const struct sb_RendererConfig cfg) {
    switch (render_api) {
    case sb_RENDERER_API_OPENGL:
        sb__gfx_gl_init_info(info_out, cfg);
        break;
    case sb_RENDERER_API_VULKAN:
        break;
    }
}

void sb_gfx_draw(const union sb_GFXInfo* gfx_info, const struct sb_GFXDrawData* data) {
    switch (render_api) {
    case sb_RENDERER_API_OPENGL:
        sb__gfx_gl_draw(gfx_info, data);
        break;
    case sb_RENDERER_API_VULKAN:
        break;
    }
}

su_S32 sb_gfx_get_uniform_loc(const union sb_GFXInfo* info, const su_String* name) {
    return sb_gfx_get_uniform_loc_cstr(info, su_string_data(name));
}

su_S32 sb_gfx_get_uniform_loc_cstr(const union sb_GFXInfo* info, const char* name) {
    su_S32 location = 0;
    switch (render_api) {
    case sb_RENDERER_API_OPENGL:
        location = sb_gl_uniform_location(info->gl_data.shader_program, name);
        break;
    case sb_RENDERER_API_VULKAN:
        break;
    }
    return location;
}

union sb_GFXTexture sb_gfx_gen_texture(void) {
    union sb_GFXTexture id;
    render_funcs.gl.gen_textures(1, &id.gl_texture.texture);
    return id;
}

void sb_gfx_upload_texture_2d(union sb_GFXTexture texture,
                              su_S32 format,
                              int width, int height,
                              const void* data) {
    render_funcs.gl.bind_texture(sb_GL_TEXTURE_2D, texture.gl_texture.texture);
    render_funcs.gl.tex_image_2d(sb_GL_TEXTURE_2D, 0,
                                 format, width, height, 0,
                                 (su_U32)format, sb_GL_UNSIGNED_BYTE, data);
}

void sb_gfx_get_texture_size(union sb_GFXTexture texture, int* width_out, int* height_out) {
    su_S32 prev_tex;
    render_funcs.gl.get_integer_v(sb_GL_TEXTURE_BINDING_2D, &prev_tex);

    render_funcs.gl.bind_texture(sb_GL_TEXTURE_2D, texture.gl_texture.texture);

    render_funcs.gl.get_texlevel_parameter_iv(sb_GL_TEXTURE_2D, 0, sb_GL_TEXTURE_WIDTH, width_out);
    render_funcs.gl.get_texlevel_parameter_iv(sb_GL_TEXTURE_2D, 0, sb_GL_TEXTURE_HEIGHT, height_out);

    render_funcs.gl.bind_texture(sb_GL_TEXTURE_2D, (su_U32)prev_tex);
}

void sb_gfx_generate_mipmap(union sb_GFXTexture texture) {
    su_S32 prev_tex;
    render_funcs.gl.get_integer_v(sb_GL_TEXTURE_BINDING_2D, &prev_tex);

    render_funcs.gl.bind_texture(sb_GL_TEXTURE_2D, texture.gl_texture.texture);

    render_funcs.gl.generate_mipmap(sb_GL_TEXTURE_2D);

    render_funcs.gl.bind_texture(sb_GL_TEXTURE_2D, (su_U32)prev_tex);
}

void sb_gfx_delete_texture(union sb_GFXTexture texture) {
    render_funcs.gl.delete_textures(1, &texture.gl_texture.texture);
}

/* === Internal helper Implementation === */

SA_INTERNAL void sb__gfx_gl_create_main_buffers(
    union sb_GFXInfo* info_out,
    const struct sb_RendererConfig cfg) {
    info_out->gl_data.vbo = sb_gl_create_vertex_buffer_dynamic(
        cfg.batch.vertex_cfg.capacity * cfg.vertex_data.element_size_internal,
        NULL);

    info_out->gl_data.ibo = sb_gl_create_index_buffer_dynamic(
        cfg.batch.index_cfg.capacity * cfg.index_data.element_size_internal,
        NULL);

    sb_gl_create_vertex_array(1, &info_out->gl_data.vao);
}

SA_INTERNAL void sb__gfx_gl_setup_vertex_attributes(
    const struct sb_RendererConfig cfg,
    su_BufferId vao,
    su_BufferId vbo) {
    sb_gl_bind_vertex_array(vao);
    sb_gl_bind_vertex_buffer(vbo);

    for (su_U64 i = 0; i < su_darray_length(cfg.vertex_data.layout_array); ++i) {
        struct sb_RendererCfgVertexLayout layout = *(struct sb_RendererCfgVertexLayout*)su_darray_get(cfg.vertex_data.layout_array, i);

        sb_gl_set_vertex_attrib_pointer(
            layout.location,
            su_SCAST_TO_M(su_U64)(su_SIZE_OF_TYPE[layout.type]),
            sb_gl_type_to_gl(layout.type),
            GL_FALSE,
            cfg.vertex_data.element_size_internal,
            su_SCAST_TO_M(void*)(layout.offset));
        sb_gl_enable_vertex_attrib_array(
            su_SCAST_TO_M(su_U64)(layout.location));
    }
}

SA_INTERNAL void sb__gfx_gl_setup_instance_buffers(
    const struct sb_RendererConfig cfg) {
    if (su_darray_is_empty(cfg.instance_data.buffer_array))
        return;

    for (su_U64 buf_i = 0; buf_i < su_darray_length(cfg.instance_data.buffer_array); ++buf_i) {
        struct sb_RendererCfgInstanceBuffer buffer_cfg = *(struct sb_RendererCfgInstanceBuffer*)su_darray_get(cfg.instance_data.buffer_array, buf_i);

        su_BufferId vbo = 0;
        render_funcs.gl.gen_buffers(1, &vbo);
        render_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, vbo);
        render_funcs.gl.buffer_data(
            sb_GL_ARRAY_BUFFER,
            buffer_cfg.size_byte_internal,
            NULL,
            GL_DYNAMIC_DRAW);

        for (su_U64 attrib_i = 0; attrib_i < su_darray_length(buffer_cfg.layout_array); ++attrib_i) {
            struct sb_RendererCfgInstanceBufferLayout attrib = *(struct sb_RendererCfgInstanceBufferLayout*)su_darray_get(buffer_cfg.layout_array, attrib_i);

            if (attrib.type == su_TYPE_MAT4) {
                for (su_U32 col = 0; col < 4; ++col) {
                    render_funcs.gl.vertex_attrib_pointer(
                        attrib.location + col,
                        4,
                        GL_FLOAT,
                        GL_FALSE,
                        (su_S32)buffer_cfg.size_byte_internal,
                        (void*)(attrib.offset + sizeof(float) * 4 * col));
                    render_funcs.gl.enable_vertex_attrib_array(attrib.location + col);
                    render_funcs.gl.vertex_attrib_divisor(attrib.location + col, 1);
                }
            } else {
                render_funcs.gl.vertex_attrib_pointer(
                    attrib.location,
                    su_SCAST_TO_M(su_U64)(su_SIZE_OF_TYPE[attrib.type]),
                    sb_gl_type_to_gl(attrib.type),
                    GL_FALSE,
                    (su_S32)buffer_cfg.size_byte_internal,
                    (void*)(attrib.offset));
                render_funcs.gl.enable_vertex_attrib_array(attrib.location);
                render_funcs.gl.vertex_attrib_divisor(attrib.location, 1);
            }
        }
    }
}

SA_INTERNAL void sb__gfx_gl_init_info(
    union sb_GFXInfo* info_out,
    const struct sb_RendererConfig cfg) {
    su_LOG_ASSERT_M(cfg.shaders.vert, su_LOG_CONTEXT_GFX, "GL vert shader is empty");
    su_LOG_ASSERT_M(cfg.shaders.frag, su_LOG_CONTEXT_GFX, "GL frag shader is empty");
    su_LOG_ASSERT_M(cfg.batch.index_cfg.capacity, su_LOG_CONTEXT_GFX, "Index capacity is not set");
    su_LOG_ASSERT_M(cfg.batch.vertex_cfg.capacity, su_LOG_CONTEXT_GFX, "Vertex capacity is not set");

    sb__gfx_gl_create_main_buffers(info_out, cfg);
    sb__gfx_gl_setup_vertex_attributes(cfg, info_out->gl_data.vao, info_out->gl_data.vbo);
    sb__gfx_gl_setup_instance_buffers(cfg);
}

SA_INTERNAL void sb__gfx_gl_draw(const union sb_GFXInfo* gfx_info, const struct sb_GFXDrawData* data) {
    // su_DUMMY_CHECK_M(su_darray_length(data->instance_array_array) ==
    //                      su_darray_length(data->instance_location_array),
    //                  "There needs to be the same amount of instance and instance locations per draw call");

    su_U64 instance_count = 0;
    static struct {
        union sb_GFXTexture texture_array[SACI_MAX_TEXTURES];
    } previous_draw_call;

    render_funcs.gl.use_program(gfx_info->gl_data.shader_program);
    render_funcs.gl.bind_vertex_array(gfx_info->gl_data.vao);
    render_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, gfx_info->gl_data.vbo);
    render_funcs.gl.buffer_subdata(sb_GL_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(long int)(data->vertex_struct_size * su_darray_length(data->vertex_array)),
                                   su_darray_get_ptr(data->vertex_array, 0));
    render_funcs.gl.bind_buffer(sb_GL_ELEMENT_ARRAY_BUFFER, gfx_info->gl_data.ibo);
    render_funcs.gl.buffer_subdata(sb_GL_ELEMENT_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(unsigned int)(data->index_struct_size * su_darray_length(data->index_array)),
                                   su_darray_get_ptr(data->index_array, 0));

    if (!su_darray_is_empty(data->instance_buffer_array)) {
        instance_count = su_darray_length(data->instance_buffer_array);
        for (su_U64 i = 0; i < instance_count; ++i) {
            struct sb_GFXInstanceData instance_buffer = *(struct sb_GFXInstanceData*)su_darray_get(data->instance_buffer_array, i);
            render_funcs.gl.bind_buffer(sb_GL_ARRAY_BUFFER, instance_buffer.location);
            render_funcs.gl.buffer_subdata(sb_GL_ARRAY_BUFFER, 0,
                                           instance_buffer.data_size,
                                           instance_buffer.instance_data_structure);
        }
    }

    { // Uniforms
        if (!su_darray_is_empty(data->uniform_data_array)) {
            for (su_U64 i = 0; i < su_darray_length(data->uniform_data_array); ++i) {
                struct sb_GFXUniformData uniform_data = *(struct sb_GFXUniformData*)su_darray_get(data->uniform_data_array, i);
                sb__gfx_gl_set_uniform_from_uniform_data(uniform_data);
            }
        }
    }

    if (sb__has_texture(su_SCAST_TO_M(union sb_GFXTexture*)(data->texture_array), SACI_MAX_TEXTURES)) {
        for (su_U32 i = 0; i < SACI_MAX_TEXTURES; ++i) {
            if (data->texture_array[i].gl_texture.texture != previous_draw_call.texture_array[i].gl_texture.texture && data->texture_array[i].gl_texture.texture != sb_TEXTURE_INVALID) {
                render_funcs.gl.active_texture(sb_GL_TEXTURE0);
                render_funcs.gl.bind_texture(sb_GL_TEXTURE_2D, data->texture_array[i].gl_texture.texture);
                previous_draw_call.texture_array[i] = data->texture_array[i];
            }
        }
    }

    if (instance_count) {
        render_funcs.gl.draw_elements_instanced(sb_GL_TRIANGLES,
                                                su_SCAST_TO_M(int)(su_darray_length(data->index_array)),
                                                sb_GL_UNSIGNED_INT,
                                                su_darray_get_ptr(data->index_array, 0),
                                                0);
    } else {
        render_funcs.gl.draw_elements(sb_GL_TRIANGLES,
                                      su_SCAST_TO_M(int)(su_darray_length(data->index_array)),
                                      sb_GL_UNSIGNED_INT,
                                      0);
    };
    render_funcs.gl.bind_buffer(sb_GL_UNIFORM_BUFFER, 0);
}

SA_INTERNAL su_Bool sb__has_texture(union sb_GFXTexture* texture_array, su_U32 array_length) {
    for (su_U32 i = 0; i < array_length; ++i) {
        if (texture_array[i].gl_texture.texture != 0) {
            return su_TRUE;
        }
    }
    return su_FALSE;
}

SA_INTERNAL void sb__gfx_gl_set_uniform_from_uniform_data(const struct sb_GFXUniformData uniform_data) {
    const void* value_ptr = NULL;

    switch (uniform_data.type) {
    case su_TYPE_U8:
        value_ptr = &uniform_data.value.u8;
        break;
    case su_TYPE_U16:
        value_ptr = &uniform_data.value.u16;
        break;
    case su_TYPE_U32:
    case su_TYPE_SHADERID:
    case su_TYPE_TEXTUREID:
    case su_TYPE_BUFFERID:
        value_ptr = &uniform_data.value.u32;
        break;
    case su_TYPE_U64:
        value_ptr = &uniform_data.value.u64;
        break;

    case su_TYPE_S8:
        value_ptr = &uniform_data.value.s8;
        break;
    case su_TYPE_S16:
        value_ptr = &uniform_data.value.s16;
        break;
    case su_TYPE_S32:
        value_ptr = &uniform_data.value.s32;
        break;
    case su_TYPE_S64:
        value_ptr = &uniform_data.value.s64;
        break;

    case su_TYPE_BOOL:
        value_ptr = &uniform_data.value.boolean;
        break;

    case su_TYPE_UV:
        value_ptr = &uniform_data.value.uv;
        break;
    case su_TYPE_VEC2:
        value_ptr = &uniform_data.value.vec2;
        break;
    case su_TYPE_VEC3:
        value_ptr = &uniform_data.value.vec3;
        break;
    case su_TYPE_VEC4:
        value_ptr = &uniform_data.value.vec4;
        break;

    case su_TYPE_COLOR:
        value_ptr = &uniform_data.value.color;
        break;

    case su_TYPE_MAT4:
        value_ptr = &uniform_data.value.mat4;
        break;

    // TODO NEED TO BE ADDED
    case su_TYPE_MAT2:
    case su_TYPE_MAT3:
    case su_TYPE_MAT2X3:
    case su_TYPE_MAT2X4:
    case su_TYPE_MAT3X2:
    case su_TYPE_MAT3X4:
    case su_TYPE_MAT4X2:
    case su_TYPE_MAT4X3:
        break;

    default:
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_GFX,
                        "Invalid type %d for uniform", uniform_data.type);
        return;
    }

    sb_gl_uniform_set_value(uniform_data.location, uniform_data.type, value_ptr);
}
