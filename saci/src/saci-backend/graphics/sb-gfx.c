#include "./sb-gfx.h"
#include "../resources/sb-dependency-internal.h"

#include "./sb-gl.h"

#ifndef __EMSCRIPTEN__
#  include "saci-utils/config/su-config-manager.h"
#else
#  include "./sb-emsdk.h"
#endif

#include "saci-utils/math/su-math-types.h"
#include "saci-utils/memory/su-darray.h"
#include "saci-utils/memory/su-memory.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-log.h"
#include "saci-utils/su-types-common.h"

#include <stdio.h>
#include <string.h>

/* === Internal helper Declarations === */

#ifndef __EMSCRIPTEN__
SA_STATIC struct sb_RenderApiLoaderFuncs sb__render_loader_funcs;
SA_STATIC enum su_RenderApi sb__render_api;
#else
SA_STATIC enum su_RenderApi sb__render_api = su_RENDERER_API_OPENGL;
#endif

SA_STATIC struct su_MemPool* sb__instance_draw_data_pool = NULL; // Used in sb__gfx_join_instance_data

// Init
SA_INTERNAL void sb__gfx_gl_init_info(union sb_GFXInfo* info_out, const struct su_RendererConfig cfg);

// Dependency
SA_INTERNAL su_ShaderId sb__gfx_emsdk_shader_create_program(const char* f, const char* v);

// Draw
SA_INTERNAL void sb__gfx_join_instance_data(const struct sb_GFXDrawData* draw_data, void** instance_data_array_out, su_U64* instance_data_array_size_out);
SA_INTERNAL void sb__gfx_gl_draw(const union sb_GFXInfo* gfx_info, const struct sb_GFXDrawData* data);
SA_INTERNAL void sb__gfx_gl_set_uniform_from_uniform_data(const struct sb_GFXUniformData uniform_data);
SA_INTERNAL su_Bool sb__has_texture(union sb_Texture* texture_array, su_U32 array_length);

/* === Header Impl === */

void sb_gfx_load(void)
{
#ifndef __EMSCRIPTEN__
    sb__render_loader_funcs = sb_dependencies_get_render_loader_api_funcs();
    sb__render_api = su_cfg_manager_get_renderer_api();
    sb__render_api = su_RENDERER_API_OPENGL;
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb_gl_load();
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
#endif
}

#ifndef __EMSCRIPTEN__
void sb_gfx_load_proc(sb_GfxProcAddress addrs)
{
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb__render_loader_funcs.gl.load_opengl(addrs);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}
#endif

void sb_gfx_init_shader(union sb_GFXInfo* info_out, const struct su_RendererConfig cfg)
{
    su_LOG_ASSERT_M(cfg.shaders.vert, su_LOG_CONTEXT_GFX, "Vertex shader is empty or NULL");
    su_LOG_ASSERT_M(cfg.shaders.frag, su_LOG_CONTEXT_GFX, "Frag shader is empty or NULL");
#ifndef __EMSCRIPTEN__
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        {
            info_out->gl_data.shader_program = sb_gl_shader_create_shader_program_source(
                cfg.shaders.vert, cfg.shaders.frag, cfg.shaders.geom);
            break;
        }
    case su_RENDERER_API_VULKAN:
        break;
    }
#else
    su_LOG_ASSERT_M(!cfg.shaders.geom, su_LOG_CONTEXT_GFX, "Opengl ES3 does not accept geometry shaders");
    info_out->gl_data.shader_program = sb_emsdk_shader_create_program_code(cfg.shaders.frag, cfg.shaders.vert);
#endif
}

void sb_gfx_create(union sb_GFXInfo* info_out, const struct su_RendererConfig cfg)
{
    sb__instance_draw_data_pool = su_mem_create_pool(su_MEM_CONTEXT_GFX, sizeof(su_Mat4) * 10000); // TODO remove the magic numbers and perhaps redo the whole chunk stuff
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb__gfx_gl_init_info(info_out, cfg);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}

void sb_gfx_clear_color(const su_Color color)
{
#ifndef __EMSCRIPTEN__
    sb_gl_clear_color(color);
#else
    sb_emsdk_clear_color(color);
#endif
}

void sb_gfx_clear_depth_buffer(void)
{
    sb_gl_clear_depth_buffer();
}

void sb_gfx_draw(const union sb_GFXInfo* gfx_info, const struct sb_GFXDrawData* data)
{
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb__gfx_gl_draw(gfx_info, data);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}

su_S32 sb_gfx_get_uniform_loc(const union sb_GFXInfo* info, const su_String* name)
{
    return sb_gfx_get_uniform_loc_cstr(info, su_string_data(name));
}

su_S32 sb_gfx_get_uniform_loc_cstr(const union sb_GFXInfo* info, const char* name)
{
    su_S32 location = 0;
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        location = sb_gl_uniform_location(info->gl_data.shader_program, name);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
    return location;
}

union sb_Texture sb_gfx_gen_texture(void)
{
    union sb_Texture id;
    sb_gl_generate_textures(1, &id.gl_texture.texture);
    return id;
}

void sb_gfx_upload_texture_2d(union sb_Texture texture,
                              su_S32 format,
                              su_S32 width, su_S32 height,
                              const void* data)
{
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb_gl_upload_texture_2d(texture.gl_texture.texture,
                                format, width, height, data);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}

void sb_gfx_get_texture_size_2d(union sb_Texture texture, su_S32* width_out, su_S32* height_out)
{
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb_gl_get_texture_size_2d(texture.gl_texture.texture,
                                  width_out, height_out);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}

void sb_gfx_generate_mipmap_2d(union sb_Texture texture)
{
    switch (sb__render_api) {

    case su_RENDERER_API_OPENGL:
        sb_gl_generate_mipmap_2d(texture.gl_texture.texture);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}

void sb_gfx_delete_texture(union sb_Texture texture)
{
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb_gl_delete_texture(1, &texture.gl_texture.texture);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}

void sb_gfx_initialize_renderer_debugger(void* debug_func)
{
    switch (sb__render_api) {
    case su_RENDERER_API_OPENGL:
        sb_gl_initialized_debugger(debug_func);
        break;
    case su_RENDERER_API_VULKAN:
        break;
    }
}

/* === Internal helper Implementation === */

SA_INTERNAL void sb__gfx_gl_create_main_buffers(
    union sb_GFXInfo* info_out,
    const struct su_RendererConfig cfg)
{
    info_out->gl_data.vbo = sb_gl_create_vertex_buffer_dynamic(
        cfg.batch.vertex_cfg.capacity * cfg.vertex_data.element_size_internal,
        NULL);

    info_out->gl_data.ibo = sb_gl_create_index_buffer_dynamic(
        cfg.batch.index_cfg.capacity * cfg.index_data.element_size_internal,
        NULL);

    sb_gl_create_vertex_array(1, &info_out->gl_data.vao);
}

SA_INTERNAL void sb__gfx_gl_setup_vertex_attributes(
    const struct su_RendererConfig cfg,
    su_BufferId vao,
    su_BufferId vbo)
{
    sb_gl_bind_vertex_array(vao);
    sb_gl_bind_vertex_buffer(vbo);

    for (su_U64 i = 0; i < cfg.vertex_data.layout_array_length; ++i) {
        const struct su_RendererCfgVertexLayout layout = cfg.vertex_data.layout_array[i];

        sb_gl_set_vertex_attrib_pointer(
            layout.location,
            su_CAST_M(su_S32)(su_SIZE_OF_TYPE[layout.type]),
            sb_gl_type_to_gl(layout.type),
            su_FALSE,
            cfg.vertex_data.element_size_internal,
            su_CAST_M(void*)(layout.offset));
        sb_gl_enable_vertex_attrib_array(
            su_CAST_M(su_U64)(layout.location));
    }
}

SA_INTERNAL void sb__gfx_gl_setup_instance_buffers(
    union sb_GFXInfo* info_out,
    const struct su_RendererConfig cfg)
{
    if (!cfg.instance_data.buffer_array_length)
        return;

    for (su_U64 buf_i = 0; buf_i < cfg.instance_data.buffer_array_length; ++buf_i) {
        const struct su_RendererCfgInstanceBuffer buffer_cfg = cfg.instance_data.buffer_array[buf_i];

        info_out->gl_data.instance_buffer = sb_gl_create_vertex_buffer_dynamic(buffer_cfg.size_byte_internal * cfg.batch.instance_cfg.capacity, NULL);
        sb_gl_bind_vertex_buffer(info_out->gl_data.instance_buffer);

        for (su_U64 attrib_i = 0; attrib_i < buffer_cfg.layout_array_length; ++attrib_i) {
            const struct su_RendererCfgInstanceBufferLayout attrib = buffer_cfg.layout_array[attrib_i];

            if (attrib.type == su_TYPE_MAT4) {
                for (su_U32 col = 0; col < 4; ++col) {
                    sb_gl_set_vertex_attrib_pointer(
                        attrib.location + col, sizeof(su_Vec4),
                        sb_GL_FLOAT, su_FALSE, (buffer_cfg.size_byte_internal),
                        (void*)(attrib.offset + sizeof(su_Vec4) * col));
                    sb_gl_enable_vertex_attrib_array(attrib.location + col);
                    sb_gl_vertex_attrib_divisor(attrib.location + col, 1);
                }
            } else {
                sb_gl_set_vertex_attrib_pointer(
                    attrib.location, su_CAST_M(su_S32)(su_SIZE_OF_TYPE[attrib.type]),
                    sb_gl_type_to_gl(attrib.type),
                    su_FALSE,
                    buffer_cfg.size_byte_internal,
                    (void*)(attrib.offset));
                sb_gl_enable_vertex_attrib_array(attrib.location);
                sb_gl_vertex_attrib_divisor(attrib.location, 1);
            }
        }
    }
    sb_gl_bind_vertex_buffer(0);
}

SA_INTERNAL void sb__gfx_gl_init_info(
    union sb_GFXInfo* info_out,
    const struct su_RendererConfig cfg)
{
    su_LOG_ASSERT_M(cfg.shaders.vert, su_LOG_CONTEXT_GFX, "GL vert shader is empty");
    su_LOG_ASSERT_M(cfg.shaders.frag, su_LOG_CONTEXT_GFX, "GL frag shader is empty");
    su_LOG_ASSERT_M(cfg.batch.index_cfg.capacity, su_LOG_CONTEXT_GFX, "Index capacity is not set");
    su_LOG_ASSERT_M(cfg.batch.vertex_cfg.capacity, su_LOG_CONTEXT_GFX, "Vertex capacity is not set");

    sb__gfx_gl_create_main_buffers(info_out, cfg);
    sb__gfx_gl_setup_vertex_attributes(cfg, info_out->gl_data.vao, info_out->gl_data.vbo);
    sb__gfx_gl_setup_instance_buffers(info_out, cfg);
}

SA_INTERNAL su_ShaderId sb__gfx_emsdk_shader_create_program(const char* f, const char* v)
{
    su_TODO_M;
}

SA_INTERNAL void sb__gfx_join_instance_data(const struct sb_GFXDrawData* draw_data, void** instance_data_array_out, su_U64* instance_data_array_size_out)
{
    *instance_data_array_size_out = 0;
    for (su_U64 i = 0; i < su_darray_length(draw_data->instance_data_array); ++i) {
        const struct sb_GFXInstanceData* data = su_darray_get(draw_data->instance_data_array, i);
        *instance_data_array_size_out += data->data_size;
    }

    if (*instance_data_array_size_out == 0) {
        *instance_data_array_out = NULL;
        return;
    }

    *instance_data_array_out = su_mem_pool_alloc(sb__instance_draw_data_pool, *instance_data_array_size_out);
    if (*instance_data_array_out == NULL) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                       su_LOG_CONTEXT_GFX, "Failed to allocate memory for instance data");
        return;
    }

    su_U8* current_ptr = (su_U8*)*instance_data_array_out;
    for (su_U64 i = 0; i < su_darray_length(draw_data->instance_data_array); ++i) {
        const struct sb_GFXInstanceData* data = su_darray_get(draw_data->instance_data_array, i);

        // Simple memcpy instead of safe_copy for testing
        memcpy(current_ptr, data->instance_data_structure, data->data_size);
        current_ptr += data->data_size;
    }
}

SA_INTERNAL void sb__gfx_gl_draw(const union sb_GFXInfo* gfx_info, const struct sb_GFXDrawData* data)
{
    sb_gl_enable(sb_GL_DEPTH_TEST); // TODO remove from here

    su_U64 instance_count = 0;
    static struct {
        union sb_Texture texture_array[SACI_MAX_TEXTURES];
    } previous_draw_call;

    sb_gl_use_program(gfx_info->gl_data.shader_program);
    sb_gl_bind_vertex_array(gfx_info->gl_data.vao);
    sb_gl_bind_vertex_buffer(gfx_info->gl_data.vbo);
    sb_gl_set_vertex_buffer_subdata(0, su_CAST_M(su_U32)(data->vertex_struct_size * su_darray_length(data->vertex_array)),
                                    su_darray_get_ptr(data->vertex_array, 0));
    sb_gl_bind_index_buffer(gfx_info->gl_data.ibo);
    sb_gl_set_index_buffer_subdata(0, su_CAST_M(su_U32)(data->index_struct_size * su_darray_length(data->index_array)),
                                   su_darray_get_ptr(data->index_array, 0));

    if (!su_darray_is_empty(data->instance_data_array)) {
        instance_count = su_darray_length(data->instance_data_array); // used later in this function

        void* instance_data = NULL;
        su_U64 instance_data_size = 0;
        sb__gfx_join_instance_data(data, &instance_data, &instance_data_size);

        sb_gl_bind_vertex_buffer(gfx_info->gl_data.instance_buffer);
        sb_gl_set_vertex_buffer_subdata(0, instance_data_size,
                                        instance_data);
    }

    { // Uniforms
        if (!su_darray_is_empty(data->uniform_data_array)) {
            for (su_U64 i = 0; i < su_darray_length(data->uniform_data_array); ++i) {
                const struct sb_GFXUniformData* uniform_data = su_darray_get(data->uniform_data_array, i);
                sb__gfx_gl_set_uniform_from_uniform_data(*uniform_data);
            }
        }
    }

    if (sb__has_texture(su_CAST_M(union sb_Texture*)(data->texture_array), SACI_MAX_TEXTURES)) {
        for (su_U32 i = 0; i < SACI_MAX_TEXTURES; ++i) {
            if (data->texture_array[i].gl_texture.texture != previous_draw_call.texture_array[i].gl_texture.texture && !data->texture_array[i].gl_texture.is_empty) {
                sb_gl_bind_texture_2d(sb_GL_TEXTURE0, data->texture_array[i].gl_texture.texture);
                previous_draw_call.texture_array[i] = data->texture_array[i];
            }
        }
    }

    sb_gl_draw_elements(sb_GL_TRIANGLES,
                        su_darray_length(data->index_array),
                        sb_GL_UNSIGNED_INT,
                        NULL,
                        instance_count);
}

SA_INTERNAL su_Bool sb__has_texture(union sb_Texture* texture_array, su_U32 array_length)
{
    for (su_U32 i = 0; i < array_length; ++i) {
        if (texture_array[i].gl_texture.texture != 0) {
            return su_TRUE;
        }
    }
    return su_FALSE;
}

SA_INTERNAL void sb__gfx_gl_set_uniform_from_uniform_data(const struct sb_GFXUniformData uniform_data)
{
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
