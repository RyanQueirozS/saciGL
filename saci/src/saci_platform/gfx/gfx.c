#include "./internal/gl4.h"

#include "./internal/gfx.h"

#include "saci_platform/dependencies/dependency.h"
#include "saci_platform/dependencies/internal/dependency.h"
#include <stdbool.h>

#include "saci_util/memory.h"
#include "saci_util/internal/general.h"
#include "saci_util/log.h"
#include "saci_util/internal/log.h"
#include "saci_util/types.h"

#include <stdio.h>
#include <string.h>

/* === Internal helper Declarations === */
SACI_STATIC struct {
    SaciBool is_loaded;
    enum PSaciRenderApi render_api;
    struct PSaciRenderApiLoaderFuncs render_loader_funcs;
    struct SaciMemPool* instance_draw_data_pool; // Used in sb__gfx_join_instance_data
} psaci_g_gfx_context = {0};

// Init
SACI_INTERNAL void psaci__gfx_gl_init_info(
    union PSaciGFXInfo* info_out,
    struct PSaciGFXInfoConstructionData construction_data);

// Draw
SACI_INTERNAL void psaci__gfx_join_instance_data(const struct PSaciGFXDrawData* draw_data, void** instance_data_array_out, SaciU64* instance_data_array_size_out);
SACI_INTERNAL void psaci__gfx_gl_draw(const union PSaciGFXInfo* gfx_info, const struct PSaciGFXDrawData* data);
SACI_INTERNAL void psaci__gfx_gl_set_uniform_from_uniform_data(const struct PSaciGFXUniformData uniform_data);
SACI_INTERNAL SaciBool psaci__has_texture(union PSaciTexture* texture_array, SaciU32 array_length);

/* === Header Impl === */

SaciBool psaci_gfx_load(void)
{
    if (psaci_g_gfx_context.is_loaded) {
        return SACI_TRUE;
    }
    psaci_g_gfx_context.is_loaded = SACI_TRUE;
    psaci_g_gfx_context.instance_draw_data_pool = saci_mem_create_pool(SACI_MEM_CONTEXT_GFX, sizeof(SaciMat4) * 10000); // TODO remove the magic numbers and perhaps redo the whole chunk stuff

#ifndef __EMSCRIPTEN__
    psaci_g_gfx_context.render_loader_funcs = psaci_dependencies_get_render_loader_api_funcs();
    psaci_g_gfx_context.render_api = psaci_dependencies_get_render_api();

    switch (psaci_g_gfx_context.render_api) {
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

void psaci_gfx_load_proc(PSaciGfxProcAddress addrs)
{
#ifndef __EMSCRIPTEN__
    switch (psaci_g_gfx_context.render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_g_gfx_context.render_loader_funcs.glad.load_opengl(addrs);
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // Not handles since EMSCRIPTEN doesn't need to load any proc
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
#endif
}

void psaci_gfx_init_shader(
    union PSaciGFXInfo* info_out,
    const char* vshader_code,
    const char* fshader_code,
    const char* gshader_code)
{
    SACI_LOG_ASSERT_M(vshader_code, SACI_LOG_CONTEXT_GFX,
                      "Vertex shader is empty or NULL");
    SACI_LOG_ASSERT_M(fshader_code, SACI_LOG_CONTEXT_GFX,
                      "Frag shader is empty or NULL");
#ifndef __EMSCRIPTEN__
    switch (psaci_g_gfx_context.render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        {
            info_out->gl_data.shader_program =
                psaci_gl_shader_create_shader_program_source(vshader_code,
                                                             fshader_code,
                                                             gshader_code);
            break;
        }
    case PSACI_RENDERER_API_OPENGLES3:
        // TODO
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
#else
    SACI_LOG_ASSERT_M(!gshader_code, SACI_LOG_CONTEXT_GFX,
                      "Opengl ES3 does not accept geometry shaders");
    info_out->gl_data.shader_program =
        psaci_emsdk_shader_create_program_code(vshader_code, fshader_code, );
#endif
}

void psaci_gfx_fill_info(union PSaciGFXInfo* info_out, const struct PSaciGFXInfoConstructionData construction_data)
{
    switch (psaci_g_gfx_context.render_api) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci__gfx_gl_init_info(info_out, construction_data);
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
    switch (psaci_g_gfx_context.render_api) {
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
    switch (psaci_g_gfx_context.render_api) {
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
    switch (psaci_g_gfx_context.render_api) {
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
    switch (psaci_g_gfx_context.render_api) {
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
    switch (psaci_g_gfx_context.render_api) {
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
    switch (psaci_g_gfx_context.render_api) {
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
    switch (psaci_g_gfx_context.render_api) {
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
    SaciU64 idx_capacity, SaciU64 idx_element_size,
    SaciU64 vtx_capacity, SaciU64 vtx_element_size)
{
    info_out->gl_data.vbo = psaci_gl_create_vertex_buffer_dynamic(
        vtx_capacity * vtx_element_size, NULL);

    info_out->gl_data.ibo = psaci_gl_create_index_buffer_dynamic(
        idx_capacity * idx_element_size, NULL);

    psaci_gl_create_vertex_array(1, &info_out->gl_data.vao);
}

SACI_INTERNAL void psaci__gfx_gl_setup_vertex_attributes(
    union PSaciGFXInfo* info_out,
    struct PSaciGFXInfoConstructionData construction_data)
{
    psaci_gl_bind_vertex_array(info_out->gl_data.vao);
    psaci_gl_bind_vertex_buffer(info_out->gl_data.vbo);

    for (SaciU64 i = 0; i < construction_data.vertex_attribute.element_array_length; ++i) {
        const struct PSaciGFXInfoVertexAttributeElementData element =
            construction_data.vertex_attribute.element_array[i];

        psaci_gl_set_vertex_attrib_pointer(
            element.location,
            SACI_CAST_M(SaciU32)(SACI_G_TYPE_SIZE_TABLE[element.type]),
            psaci_gl_type_to_gl(element.type),
            SACI_FALSE,
            construction_data.vertex_attribute.total_size_bytes,
            SACI_CAST_M(void*)(&element.offset));
        psaci_gl_enable_vertex_attrib_array(
            SACI_CAST_M(SaciU64)(element.location));
    }
}

SACI_INTERNAL void psaci__gfx_gl_setup_instance_buffer_attribute(
    struct PSaciGFXInfoInstanceBufferLayoutData layout,
    SaciU64 buffer_total_size_bytes)
{
    if (layout.type == SACI_TYPE_MAT4) {
        for (SaciU32 col = 0; col < 4; ++col) {
            psaci_gl_set_vertex_attrib_pointer(
                layout.location + col, sizeof(SaciVec4),
                PSACI_GL_FLOAT, SACI_FALSE, (buffer_total_size_bytes),
                (void*)(layout.offset + sizeof(SaciVec4) * col));
            psaci_gl_enable_vertex_attrib_array(layout.location + col);
            psaci_gl_vertex_attrib_divisor(layout.location + col, 1);
        }
    } else {
        psaci_gl_set_vertex_attrib_pointer(
            layout.location, SACI_CAST_M(SaciS32)(SACI_G_TYPE_SIZE_TABLE[layout.type]),
            psaci_gl_type_to_gl(layout.type),
            SACI_FALSE,
            buffer_total_size_bytes,
            (void*)(layout.offset));
        psaci_gl_enable_vertex_attrib_array(layout.location);
        psaci_gl_vertex_attrib_divisor(layout.location, 1);
    }
}

SACI_INTERNAL void psaci__gfx_gl_setup_instance_buffer(
    union PSaciGFXInfo* info_out,
    const struct PSaciGFXInfoInstanceBufferData buffer_data, SaciU8 idx)
{
    info_out->gl_data.instance_buffer[idx] = psaci_gl_create_vertex_buffer_dynamic(buffer_data.total_size_byte * buffer_data.capacity, NULL);
    psaci_gl_bind_vertex_buffer(info_out->gl_data.instance_buffer[idx]);

    for (SaciU64 attrib_i = 0; attrib_i < buffer_data.layout_array_length; ++attrib_i) {
        const struct PSaciGFXInfoInstanceBufferLayoutData layout = buffer_data.layout_array[attrib_i];
        psaci__gfx_gl_setup_instance_buffer_attribute(layout, buffer_data.total_size_byte);
    }
}

SACI_INTERNAL void psaci__gfx_gl_setup_instance_buffer_array(
    union PSaciGFXInfo* info_out,
    struct PSaciGFXInfoConstructionData construction_data)
{
    if (!construction_data.instance_buffer_array_length) {
        return;
    }

    for (SaciU8 buf_i = 0; buf_i < construction_data.instance_buffer_array_length; ++buf_i) {
        const struct PSaciGFXInfoInstanceBufferData buffer_data = construction_data.instance_buffer_array[buf_i];
        psaci__gfx_gl_setup_instance_buffer(info_out, buffer_data, buf_i);
    }
    psaci_gl_bind_vertex_buffer(0);
}

SACI_INTERNAL void psaci__gfx_gl_init_info(
    union PSaciGFXInfo* info_out,
    struct PSaciGFXInfoConstructionData construction_data)
{
    SACI_LOG_ASSERT_M(construction_data.idx_capacity,
                      SACI_LOG_CONTEXT_GFX, "Index capacity is not set");
    SACI_LOG_ASSERT_M(construction_data.vtx_capacity,
                      SACI_LOG_CONTEXT_GFX, "Vertex capacity is not set");
    SACI_LOG_ASSERT_M(construction_data.idx_element_size,
                      SACI_LOG_CONTEXT_GFX, "Index element size is not set");
    SACI_LOG_ASSERT_M(construction_data.vtx_element_size,
                      SACI_LOG_CONTEXT_GFX, "Vertex element size is not set");

    psaci__gfx_gl_create_main_buffers(
        info_out,
        construction_data.idx_capacity, construction_data.idx_element_size,
        construction_data.vtx_capacity, construction_data.vtx_element_size);

    psaci__gfx_gl_setup_vertex_attributes(info_out, construction_data);

    psaci__gfx_gl_setup_instance_buffer_array(info_out, construction_data);
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

    *instance_data_array_out = saci_mem_pool_alloc(psaci_g_gfx_context.instance_draw_data_pool, *instance_data_array_size_out);
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

        psaci_gl_bind_vertex_buffer(data->instance_buffer_id);
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
