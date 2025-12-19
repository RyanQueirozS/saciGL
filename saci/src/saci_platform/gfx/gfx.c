#include "./internal/gl4.h"

#include "./internal/gfx.h"

#include "saci_platform/dependencies/dependency.h"
#include "saci_platform/dependencies/internal/dependency.h"

#ifndef __EMSCRIPTEN__
#  include "saci_platform/config/config.h"
#endif

#include "saci_util/memory.h"
#include "saci_util/internal/general.h"
#include "saci_util/log.h"
#include "saci_util/internal/log.h"
#include "saci_util/types.h"

#include <stdio.h>
#include <string.h>

/* === Internal helper Declarations === */

#ifndef __EMSCRIPTEN__
SACI_STATIC struct PSaciRenderApiLoaderFuncs psaci_g_render_loader_funcs;
SACI_STATIC enum PSaciRenderApi psaci_g_render_api;
#else
SACI_STATIC enum PSaciRenderApi sb__render_api = PSACI_RENDERER_API_OPENGL;
#endif

SACI_STATIC struct SaciMemPool* psaci_g_instance_draw_data_pool = NULL; // Used in sb__gfx_join_instance_data

// Init
SACI_INTERNAL void psaci__gfx_gl_init_info(union PSaciGFXInfo* info_out, const struct PSaciRendererConfig cfg);

// Draw
SACI_INTERNAL void psaci__gfx_join_instance_data(const struct PSaciGFXDrawData* draw_data, void** instance_data_array_out, SaciU64* instance_data_array_size_out);
SACI_INTERNAL void psaci__gfx_gl_draw(const union PSaciGFXInfo* gfx_info, const struct PSaciGFXDrawData* data);
SACI_INTERNAL void psaci__gfx_gl_set_uniform_from_uniform_data(const struct PSaciGFXUniformData uniform_data);
SACI_INTERNAL SaciBool psaci__has_texture(union PSaciTexture* texture_array, SaciU32 array_length);

/* === Header Impl === */

SaciBool psaci_gfx_load(void)
{
#ifndef __EMSCRIPTEN__
    psaci_g_render_loader_funcs = psaci_dependencies_get_render_loader_api_funcs();
    psaci_g_render_api = psaci_dependencies_get_render_api();

    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        return psaci_gl_load();
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
#endif
    return SACI_FALSE;
}

#ifndef __EMSCRIPTEN__
void psaci_gfx_load_proc(PSaciGfxProcAddress addrs)
{
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_g_render_loader_funcs.glad.load_opengl(addrs);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}
#endif

void psaci_gfx_init_shader(union PSaciGFXInfo* info_out, const struct PSaciRendererConfig cfg)
{
    SACI_LOG_ASSERT_M(cfg.shaders.vert, SACI_LOG_CONTEXT_GFX, "Vertex shader is empty or NULL");
    SACI_LOG_ASSERT_M(cfg.shaders.frag, SACI_LOG_CONTEXT_GFX, "Frag shader is empty or NULL");
#ifndef __EMSCRIPTEN__
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        {
            info_out->gl_data.shader_program = psaci_gl_shader_create_shader_program_source(
                cfg.shaders.vert, cfg.shaders.frag, cfg.shaders.geom);
            break;
        }
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
#else
    SACI_LOG_ASSERT_M(!cfg.shaders.geom, SACI_LOG_CONTEXT_GFX, "Opengl ES3 does not accept geometry shaders");
    info_out->gl_data.shader_program = sb_emsdk_shader_create_program_code(cfg.shaders.frag, cfg.shaders.vert);
#endif
}

void psaci_gfx_create(union PSaciGFXInfo* info_out, const struct PSaciRendererConfig cfg)
{
    psaci_g_instance_draw_data_pool = saci_mem_create_pool(SACI_MEM_CONTEXT_GFX, sizeof(SaciMat4) * 10000); // TODO remove the magic numbers and perhaps redo the whole chunk stuff
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci__gfx_gl_init_info(info_out, cfg);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}

void psaci_gfx_clear_color(const SaciColor color)
{
#ifndef __EMSCRIPTEN__
    psaci_gl_clear_color(color);
#else
    sb_emsdk_clear_color(color);
#endif
}

void psaci_gfx_clear_depth_buffer(void)
{
    psaci_gl_clear_depth_buffer();
}

void psaci_gfx_draw(const union PSaciGFXInfo* gfx_info, const struct PSaciGFXDrawData* data)
{
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci__gfx_gl_draw(gfx_info, data);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}

SaciS32 psaci_gfx_get_uniform_loc(const union PSaciGFXInfo* info, const char* name)
{
    SaciS32 location = 0;
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        location = psaci_gl_uniform_location(info->gl_data.shader_program, name);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
    return location;
}

union PSaciTexture psaci_gfx_gen_texture(void)
{
    union PSaciTexture tex;
    psaci_gl_generate_textures(1, &tex.gl.texture);
    return tex;
}

void psaci_gfx_upload_texture_2d(union PSaciTexture texture,
                                 SaciS32 format,
                                 SaciS32 width, SaciS32 height,
                                 const void* data)
{
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_gl_upload_texture_2d(texture.gl.texture,
                                   format, width, height, data);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}

void psaci_gfx_get_texture_size_2d(union PSaciTexture texture, SaciS32* width_out, SaciS32* height_out)
{
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_gl_get_texture_size_2d(texture.gl.texture,
                                     width_out, height_out);
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}

void psaci_gfx_generate_mipmap_2d(union PSaciTexture texture)
{
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_gl_generate_mipmap_2d(texture.gl.texture);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}

void psaci_gfx_delete_texture(union PSaciTexture texture)
{
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_gl_delete_texture(1, &texture.gl.texture);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}

void psaci_gfx_initialize_renderer_debugger(void* debug_func)
{
    switch (psaci_g_render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_gl_initialized_debugger(debug_func);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
}

/* === Internal helper Implementation === */

SACI_INTERNAL void psaci__gfx_gl_create_main_buffers(
    union PSaciGFXInfo* info_out,
    const struct PSaciRendererConfig cfg)
{
    info_out->gl_data.vbo = psaci_gl_create_vertex_buffer_dynamic(
        cfg.batch.vertex_cfg.capacity * cfg.vertex_data.element_size_internal,
        NULL);

    info_out->gl_data.ibo = psaci_gl_create_index_buffer_dynamic(
        cfg.batch.index_cfg.capacity * cfg.index_data.element_size_internal,
        NULL);

    psaci_gl_create_vertex_array(1, &info_out->gl_data.vao);
}

SACI_INTERNAL void psaci__gfx_gl_setup_vertex_attributes(
    const struct PSaciRendererConfig cfg,
    SaciBufferId vao,
    SaciBufferId vbo)
{
    psaci_gl_bind_vertex_array(vao);
    psaci_gl_bind_vertex_buffer(vbo);

    for (SaciU64 i = 0; i < cfg.vertex_data.layout_array_length; ++i) {
        const struct PSaciRendererCfgVertexLayout layout = cfg.vertex_data.layout_array[i];

        psaci_gl_set_vertex_attrib_pointer(
            layout.location,
            SACI_CAST_M(SaciS32)(SACI_G_TYPE_SIZE_TABLE[layout.type]),
            psaci_gl_type_to_gl(layout.type),
            SACI_FALSE,
            cfg.vertex_data.element_size_internal,
            SACI_CAST_M(void*)(layout.offset));
        psaci_gl_enable_vertex_attrib_array(
            SACI_CAST_M(SaciU64)(layout.location));
    }
}

SACI_INTERNAL void psaci__gfx_gl_setup_instance_buffers(
    union PSaciGFXInfo* info_out,
    const struct PSaciRendererConfig cfg)
{
    if (!cfg.instance_data.buffer_array_length) {
        return;
    }

    for (SaciU64 buf_i = 0; buf_i < cfg.instance_data.buffer_array_length; ++buf_i) {
        const struct PSaciRendererCfgInstanceBuffer buffer_cfg = cfg.instance_data.buffer_array[buf_i];

        info_out->gl_data.instance_buffer = psaci_gl_create_vertex_buffer_dynamic(buffer_cfg.size_byte_internal * cfg.batch.instance_cfg.capacity, NULL);
        psaci_gl_bind_vertex_buffer(info_out->gl_data.instance_buffer);

        for (SaciU64 attrib_i = 0; attrib_i < buffer_cfg.layout_array_length; ++attrib_i) {
            const struct PSaciRendererCfgInstanceBufferLayout attrib = buffer_cfg.layout_array[attrib_i];

            if (attrib.type == SACI_TYPE_MAT4) {
                for (SaciU32 col = 0; col < 4; ++col) {
                    psaci_gl_set_vertex_attrib_pointer(
                        attrib.location + col, sizeof(SaciVec4),
                        PSACI_GL_FLOAT, SACI_FALSE, (buffer_cfg.size_byte_internal),
                        (void*)(attrib.offset + sizeof(SaciVec4) * col));
                    psaci_gl_enable_vertex_attrib_array(attrib.location + col);
                    psaci_gl_vertex_attrib_divisor(attrib.location + col, 1);
                }
            } else {
                psaci_gl_set_vertex_attrib_pointer(
                    attrib.location, SACI_CAST_M(SaciS32)(SACI_G_TYPE_SIZE_TABLE[attrib.type]),
                    psaci_gl_type_to_gl(attrib.type),
                    SACI_FALSE,
                    buffer_cfg.size_byte_internal,
                    (void*)(attrib.offset));
                psaci_gl_enable_vertex_attrib_array(attrib.location);
                psaci_gl_vertex_attrib_divisor(attrib.location, 1);
            }
        }
    }
    psaci_gl_bind_vertex_buffer(0);
}

SACI_INTERNAL void psaci__gfx_gl_init_info(
    union PSaciGFXInfo* info_out,
    const struct PSaciRendererConfig cfg)
{
    SACI_LOG_ASSERT_M(cfg.shaders.vert, SACI_LOG_CONTEXT_GFX, "GL vert shader is empty");
    SACI_LOG_ASSERT_M(cfg.shaders.frag, SACI_LOG_CONTEXT_GFX, "GL frag shader is empty");
    SACI_LOG_ASSERT_M(cfg.batch.index_cfg.capacity, SACI_LOG_CONTEXT_GFX, "Index capacity is not set");
    SACI_LOG_ASSERT_M(cfg.batch.vertex_cfg.capacity, SACI_LOG_CONTEXT_GFX, "Vertex capacity is not set");

    psaci__gfx_gl_create_main_buffers(info_out, cfg);
    psaci__gfx_gl_setup_vertex_attributes(cfg, info_out->gl_data.vao, info_out->gl_data.vbo);
    psaci__gfx_gl_setup_instance_buffers(info_out, cfg);
}

SACI_INTERNAL void psaci__gfx_join_instance_data(const struct PSaciGFXDrawData* draw_data, void** instance_data_array_out, SaciU64* instance_data_array_size_out)
{
    *instance_data_array_size_out = 0;
    for (SaciU64 i = 0; i < draw_data->instance_data_array_count; ++i) {
        const struct PSaciGFXInstanceData data = draw_data->instance_data_array[i];
        *instance_data_array_size_out += data.data_size;
    }

    if (*instance_data_array_size_out == 0) {
        *instance_data_array_out = NULL;
        return;
    }

    *instance_data_array_out = saci_mem_pool_alloc(psaci_g_instance_draw_data_pool, *instance_data_array_size_out);
    if (*instance_data_array_out == NULL) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_GFX, "Failed to allocate memory for instance data");
        return;
    }

    SaciU8* current_ptr = (SaciU8*)*instance_data_array_out;
    for (SaciU64 i = 0; i < draw_data->instance_data_array_count; ++i) {
        const struct PSaciGFXInstanceData data = draw_data->instance_data_array[i];

        // Simple memcpy instead of safe_copy for testing
        memcpy(current_ptr, data.instance_data_structure, data.data_size);
        current_ptr += data.data_size;
    }
}

SACI_INTERNAL void psaci__gfx_gl_draw(const union PSaciGFXInfo* gfx_info, const struct PSaciGFXDrawData* data)
{
    psaci_gl_enable(PSACI_GL_DEPTH_TEST); // TODO remove from here

    SaciU64 instance_count = data->index_data.count; // used later in this function;
    static struct {
        union PSaciTexture texture_array[SACI_MAX_TEXTURES];
    } psaci__previous_draw_call;

    psaci_gl_use_program(gfx_info->gl_data.shader_program);
    psaci_gl_bind_vertex_array(gfx_info->gl_data.vao);
    psaci_gl_bind_vertex_buffer(gfx_info->gl_data.vbo);
    psaci_gl_set_vertex_buffer_subdata(0, SACI_CAST_M(SaciU32)(data->vertex_data.struct_size * data->vertex_data.count),
                                       data->vertex_data.array);
    psaci_gl_bind_index_buffer(gfx_info->gl_data.ibo);
    psaci_gl_set_index_buffer_subdata(0, SACI_CAST_M(SaciU32)(data->index_data.struct_size * data->index_data.count),
                                      data->index_data.array);

    if (!data->instance_data_array) {

        void* instance_data = NULL;
        SaciU64 instance_data_size = 0;
        psaci__gfx_join_instance_data(data, &instance_data, &instance_data_size);

        psaci_gl_bind_vertex_buffer(gfx_info->gl_data.instance_buffer);
        psaci_gl_set_vertex_buffer_subdata(0, instance_data_size,
                                           instance_data);
    }

    { // Uniforms
        if (!data->uniform_data_array) {
            for (SaciU64 i = 0; i < data->uniform_data_array_count; ++i) {
                const struct PSaciGFXUniformData uniform_data = data->uniform_data_array[i];
                psaci__gfx_gl_set_uniform_from_uniform_data(uniform_data);
            }
        }
    }

    if (psaci__has_texture(SACI_CAST_M(union PSaciTexture*)(data->texture_array), SACI_MAX_TEXTURES)) {
        for (SaciU32 i = 0; i < SACI_MAX_TEXTURES; ++i) {
            if (data->texture_array[i].gl.texture != psaci__previous_draw_call.texture_array[i].gl.texture && !data->texture_array[i].gl.is_empty) {
                psaci_gl_bind_texture_2d(PSACI_GL_TEXTURE0, data->texture_array[i].gl.texture);
                psaci__previous_draw_call.texture_array[i] = data->texture_array[i];
            }
        }
    }

    psaci_gl_draw_elements(PSACI_GL_TRIANGLES,
                           data->index_data.count,
                           PSACI_GL_UNSIGNED_INT,
                           NULL,
                           instance_count);
}

SACI_INTERNAL SaciBool psaci__has_texture(union PSaciTexture* texture_array, SaciU32 array_length)
{
    for (SaciU32 i = 0; i < array_length; ++i) {
        if (texture_array[i].gl.texture != 0) {
            return SACI_TRUE;
        }
    }
    return SACI_FALSE;
}

SACI_INTERNAL void psaci__gfx_gl_set_uniform_from_uniform_data(const struct PSaciGFXUniformData uniform_data)
{
    const void* value_ptr = NULL;

    switch (uniform_data.type) {
    case SACI_TYPE_U8:
        value_ptr = &uniform_data.value.u8;
        break;
    case SACI_TYPE_U16:
        value_ptr = &uniform_data.value.u16;
        break;
    case SACI_TYPE_U32:
    case SACI_TYPE_SHADERID:
    case SACI_TYPE_TEXTUREID:
    case SACI_TYPE_BUFFERID:
        value_ptr = &uniform_data.value.u32;
        break;
    case SACI_TYPE_U64:
        value_ptr = &uniform_data.value.u64;
        break;

    case SACI_TYPE_S8:
        value_ptr = &uniform_data.value.s8;
        break;
    case SACI_TYPE_S16:
        value_ptr = &uniform_data.value.s16;
        break;
    case SACI_TYPE_S32:
        value_ptr = &uniform_data.value.s32;
        break;
    case SACI_TYPE_S64:
        value_ptr = &uniform_data.value.s64;
        break;

    case SACI_TYPE_BOOL:
        value_ptr = &uniform_data.value.boolean;
        break;

    case SACI_TYPE_UV:
        value_ptr = &uniform_data.value.uv;
        break;
    case SACI_TYPE_VEC2:
        value_ptr = &uniform_data.value.vec2;
        break;
    case SACI_TYPE_VEC3:
        value_ptr = &uniform_data.value.vec3;
        break;
    case SACI_TYPE_VEC4:
        value_ptr = &uniform_data.value.vec4;
        break;

    case SACI_TYPE_COLOR:
        value_ptr = &uniform_data.value.color;
        break;

    case SACI_TYPE_MAT4:
        value_ptr = &uniform_data.value.mat4;
        break;

    // TODO NEED TO BE ADDED
    case SACI_TYPE_MAT2:
    case SACI_TYPE_MAT3:
    case SACI_TYPE_MAT2X3:
    case SACI_TYPE_MAT2X4:
    case SACI_TYPE_MAT3X2:
    case SACI_TYPE_MAT3X4:
    case SACI_TYPE_MAT4X2:
    case SACI_TYPE_MAT4X3:
        break;

    default:
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_GFX,
                          "Invalid type %d for uniform", uniform_data.type);
        return;
    }

    psaci_gl_uniform_set_value(uniform_data.location, uniform_data.type, value_ptr);
}
