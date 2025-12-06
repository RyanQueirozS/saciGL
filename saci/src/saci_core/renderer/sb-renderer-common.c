#include "./sb-renderer-common.h"

#include "saci-utils/su-log.h"

#ifndef __EMSCRIPTEN__
#  include "saci-utils/config/su-config-manager.h"
#endif

#include <stdio.h>

// Internal
SA_INTERNAL void sb__init_instance_buffers(struct su_RendererConfig* cfg_out);

SA_INTERNAL void sb__renderer_instance_fill_default(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info);

SA_INTERNAL_CONST struct su_RendererConfig sb__instance_config_default = {
    .name = "instance",
    .vertex_data = {
        .layout_array = (struct su_RendererCfgVertexLayout[3]){
            {.name = "position", .type = su_TYPE_VEC3, .offset = 0, .location = 0},
            {.name = "color", .type = su_TYPE_COLOR, .offset = 12, .location = 1},
            {.name = "uv", .type = su_TYPE_UV, .offset = 28, .location = 2},
        },
        .layout_array_length = 3,
        .element_size_internal = 36,
    },
    .index_data = {
        .element_size_internal = su_SIZE_OF_TYPE[su_TYPE_U32],
    },
    .shaders = {
        .frag = "#version "
#ifndef __EMSCRIPTEN__
                "330 core"
#else
                "300 es\n"
                "precision mediump float;"
#endif
                "\n\n"
                "in vec4 v_color;\n"
                "in vec2 v_texcoord;\n\n"
                "uniform sampler2D u_texture;\n"
                "uniform bool u_use_texture;\n\n"
                "out vec4 frag_color;\n\n"
                "void main()\n"
                "{\n"
                "   if (u_use_texture) {\n"
                "       vec4 texcolor = texture(u_texture, v_texcoord);\n"
                "       frag_color = texcolor * v_color;\n"
                "   } else {\n"
                "       frag_color = v_color;\n"
                "   }\n"
                "}",
        .vert = "#version "
#ifndef __EMSCRIPTEN__
                "330 core"
#else
                "300 es"
#endif
                "\n\n"
                "layout (location = 0) in vec3 a_pos;\n"
                "layout (location = 1) in vec4 a_color;\n"
                "layout (location = 2) in vec2 a_texcoord;\n"
                "layout (location = 3) in mat4 i_model_matrix;\n"
                "layout (location = 7) in vec4 i_color;\n\n"
                "uniform mat4 u_model_matrix;\n"
                "uniform mat4 u_view_matrix;\n"
                "uniform mat4 u_projection_matrix;\n"
                "uniform int u_flags;\n"
                "uniform vec4 u_lighting;\n\n"
                "out vec4 v_color;\n"
                "out vec2 v_texcoord;\n\n"
                "void main()\n"
                "{\n"
                "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * i_model_matrix * vec4(a_pos, 1.0);\n"
                "   v_color = a_color + i_color;\n"
                "   v_texcoord = a_texcoord;\n"
                "}",
        .geom = NULL,
    },
    .uniform_array = (struct su_RendererCfgUniform[7]){
        {.name = "u_texture", .type = su_TYPE_SAMPLER2D},
        {.name = "u_use_texture", .type = su_TYPE_BOOL},
        {.name = "u_model_matrix", .type = su_TYPE_MAT4},
        {.name = "u_view_matrix", .type = su_TYPE_MAT4},
        {.name = "u_projection_matrix", .type = su_TYPE_MAT4},
        {.name = "u_flags", .type = su_TYPE_S32},
        {.name = "u_lighting", .type = su_TYPE_VEC4},
    },
    .uniform_array_length = 7,
    .sampler_array = NULL,
    .batch = {
        .capacity = 10,
        .index_cfg = {
            .capacity = 1000, // TODO
        },
        .vertex_cfg = {
            .capacity = 1000, // TODO
        },
        .instance_cfg = {
            .capacity = 1000,
        },
    },
    .bound = {
        .index_cfg = {
            .capacity = 1000,
        },
        .instance_cfg = {
            .capacity = 10,
        },
    },
    .draw = {
        .primitive = su_PRIMITIVES_TRIANGLES,
        .cull_mode = su_RENDERER_CULL_MODE_BACK,
        .front_face = su_RENDERER_FRONT_FACE_CCW,
    },
    .pipeline = {
        .depth_test = su_TRUE,
        .blend = {
            .enabled = true,
            .opts = 0,
        },
    },
};

// Header impl

sb_Renderer* sb_renderer_new(const enum sb_RendererType type, const char* name)
{
    struct su_RendererConfig cfg = {0};
    union sb_GFXInfo info = {0};
    const char* name_ptr = name;

    if (!name) {
        switch (type) {
        case sb_RENDERER_STATIC:
            name_ptr = "static";
            break;
        case sb_RENDERER_DYNAMIC:
            name_ptr = "dynamic";
            break;
        case sb_RENDERER_INSTANCE:
            name_ptr = "instance";
            cfg = sb__instance_config_default;
            break;
        }
    }
#ifndef __EMSCRIPTEN__
    su_cfg_manager_get_renderer(name_ptr, &cfg);
#endif
    sb_init_shaders(&cfg, &info);
    sb__renderer_instance_fill_default(&cfg, &info);
    su_MemPool* pool = sb_renderer_get_pool_from_cfg(&cfg, type);
    sb_Renderer* rendr = su_mem_pool_alloc(pool, sizeof(struct sb_Renderer));
    rendr->type = type;

    switch (type) {
    case sb_RENDERER_STATIC:
        su_TODO_M;
        break;
    case sb_RENDERER_DYNAMIC:
        su_TODO_M;
        break;
    case sb_RENDERER_INSTANCE:
        rendr->interface = &sb__RENDERER_INSTANCE_INTERFACE_DEFAULT_INITIALIZER;
        rendr->interface->new(rendr, pool, &cfg, &info);
        break;
    }
    return rendr;
}

void sb_renderer_begin(struct sb_Renderer* rendr)
{
    rendr->interface->begin(rendr);
}

void sb_renderer_bind_texture(struct sb_Renderer* rendr, su_TextureId tex_id)
{
    rendr->interface->bind_texture(rendr, tex_id);
}

su_S32 sb_renderer_get_uniform_id(struct sb_Renderer* rendr,
                                  const char* const uniform_name)
{
    return rendr->interface->get_uniform_id(rendr, uniform_name);
}

void sb_renderer_set_uniform(struct sb_Renderer* rendr,
                             const su_S32 uniform_id,
                             const void* const value,
                             const su_DataType type)
{
    rendr->interface->set_uniform(rendr, uniform_id, value, type);
}

void sb_renderer_bind_index_buffer(struct sb_Renderer* rendr,
                                   const su_DArray* new_indices)
{
    rendr->interface->bind_index_buffer(rendr, new_indices);
}

void sb_renderer_push_mesh(struct sb_Renderer* rendr,
                           const su_DArray* pos_array,
                           const su_DArray* uv_array,
                           const su_DArray* color_array)
{
    rendr->interface->push_mesh(rendr, pos_array, uv_array, color_array);
}

void sb_renderer_draw(const struct sb_Renderer* rendr)
{
    rendr->interface->draw(rendr);
}

void sb_renderer_free(struct sb_Renderer* rendr)
{
    rendr->interface->free(rendr);
}

void sb_renderer_free_opts(struct sb_Renderer* rendr, int free_opts)
{
    rendr->interface->free_opts(rendr, free_opts);
}

void sb_renderer_init_bound(struct sb_RendererBound* bound_out, const struct su_RendererConfig cfg, su_MemPool* mem)
{
    bound_out->index_array = su_darray_create(
        cfg.bound.index_cfg.capacity,
        sizeof(su_U32),
        su_FALSE);

    // TODO
    bound_out->uniform_data_array = su_darray_create(
        cfg.uniform_array_length,
        sizeof(struct sb_GFXUniformData),
        su_FALSE);
}

void sb_init_uniforms(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info)
{
    SA_INTERNAL struct su_RendererCfgUniform uniform_array[6] = {
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_S32},
        {.name = NULL, .location = 0, .type = su_TYPE_VEC4},
        {.name = NULL, .location = 0, .type = su_TYPE_VEC4},
    };

    if (!uniform_array[0].name) {
        uniform_array[0].name = "u_model_matrix";
        uniform_array[1].name = "u_view_matrix";
        uniform_array[2].name = "u_projection_matrix";
        uniform_array[3].name = "u_flags";
        uniform_array[4].name = "u_lighting";
        uniform_array[5].name = "u_use_texture";

        for (su_U64 i = 0; i < su_ARRLEN_M(uniform_array); ++i) {
            uniform_array[i].location = sb_gfx_get_uniform_loc_cstr(gfx_info, uniform_array[i].name);
        }
    }

    if (!cfg_out->uniform_array) {
        // TODO
#if 0
        cfg_out->uniform_array = su_darray_create(
            su_ARRLEN_M(uniform_array),
            sizeof(struct su_RendererCfgUniform),
            su_TRUE);
        for (su_U64 i = 0; i < su_ARRLEN_M(uniform_array); ++i) {
            su_darray_push(cfg_out->uniform_array, &uniform_array[i]);
        }
#endif
    }
}

void sb_init_samplers(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info)
{
    SA_INTERNAL struct su_RendererCfgSampler sampler_array[1] = {
        {.name = NULL, .binding = 0, .type = su_TYPE_SAMPLER2D}};

    if (!sampler_array[0].name) {
        sampler_array[0].name = "u_texture";
        sampler_array[0].binding = sb_gfx_get_uniform_loc_cstr(gfx_info, sampler_array[0].name);
        sampler_array[0].unit = 0;
    }

    if (!cfg_out->sampler_array) {
        // cfg_out->sampler_array = su_darray_create(
        //     su_ARRLEN_M(sampler_array),
        //     sizeof(struct su_RendererCfgSampler),
        //     su_TRUE);
        // for (su_U64 i = 0; i < su_ARRLEN_M(sampler_array); ++i) {
        //     su_darray_push(cfg_out->sampler_array, &sampler_array[i]);
        // }
    }
}

void sb_init_vertex_layout(struct su_RendererConfig* cfg_out)
{
    SA_STATIC struct su_RendererCfgVertexLayout v_layout[3] = {
        {.name = "pos", .type = su_TYPE_VEC3, .location = 0, .offset = 0},
        {.name = "color", .type = su_TYPE_VEC4, .location = 1, .offset = 12},
        {.name = "uv", .type = su_TYPE_UV, .location = 2, .offset = 38},
    };
    su_U64 size_of_elements =
        su_SIZE_OF_TYPE[v_layout[0].type] +
        su_SIZE_OF_TYPE[v_layout[1].type] +
        su_SIZE_OF_TYPE[v_layout[2].type];

    cfg_out->vertex_data.layout_array = v_layout;
    cfg_out->vertex_data.layout_array_length = su_ARRLEN_M(v_layout);
    cfg_out->vertex_data.element_size_internal = size_of_elements;
}

void sb_init_shaders(struct su_RendererConfig* cfg, union sb_GFXInfo* info_out)
{
    sb_gfx_init_shader(info_out, *cfg);
}

union sb_GFXUniformValue sb_renderer_uniform_value_from_type(su_DataType type, const void* value)
{
    union sb_GFXUniformValue result = {0};

    switch (type) {
    case su_TYPE_U8:
        result.u8 = *(const su_U8*)value;
        break;
    case su_TYPE_U16:
        result.u16 = *(const su_U16*)value;
        break;
    case su_TYPE_U32:
        result.u32 = *(const su_U32*)value;
        break;
    case su_TYPE_U64:
        result.u64 = *(const su_U64*)value;
        break;

    case su_TYPE_S8:
        result.s8 = *(const su_S8*)value;
        break;
    case su_TYPE_S16:
        result.s16 = *(const su_S16*)value;
        break;
    case su_TYPE_S32:
        result.s32 = *(const su_S32*)value;
        break;
    case su_TYPE_S64:
        result.s64 = *(const su_S64*)value;
        break;

    case su_TYPE_BOOL:
        result.boolean = *(const su_Bool*)value;
        break;

    case su_TYPE_UV:
        result.uv = *(const su_Uv*)value;
        break;
    case su_TYPE_VEC2:
        result.vec2 = *(const su_Vec2*)value;
        break;
    case su_TYPE_VEC3:
        result.vec3 = *(const SacicSuVec3*)value;
        break;
    case su_TYPE_VEC4:
        result.vec4 = *(const su_Vec4*)value;
        break;

    case su_TYPE_COLOR:
        result.color = *(const su_Color*)value;
        break;

    case su_TYPE_MAT2:
    case su_TYPE_MAT3:
        break;
    case su_TYPE_MAT4:
        result.mat4 = *(const su_Mat4*)value;
        break;

    case su_TYPE_MAT2X3:
    case su_TYPE_MAT2X4:
    case su_TYPE_MAT3X2:
    case su_TYPE_MAT3X4:
    case su_TYPE_MAT4X2:
    case su_TYPE_MAT4X3:
        break;

    default:
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                        "Invalid type %d for uniform", type);
        break;
    }

    return result;
}

su_MemPool* sb_renderer_get_pool_from_cfg(const struct su_RendererConfig* cfg, enum sb_RendererType type)
{
    su_U64 total_size = 0,
           vertex_size = cfg->vertex_data.element_size_internal,
           index_size = cfg->index_data.element_size_internal,
           batch_capacity = cfg->batch.capacity;

    total_size += sizeof(struct sb_Renderer);
    // total_size += sizeof(struct sb_RendererInterface); // References a static var, doesn't need to be allocated

    // If there is any instance buffer data
    switch (type) {
    case sb_RENDERER_STATIC:
        total_size += sizeof(struct sb_StaticRenderer);
        break;
    case sb_RENDERER_DYNAMIC:
        break;
    case sb_RENDERER_INSTANCE:
        total_size += sizeof(struct sb_InstanceRenderer);
        break;
    }
    { // InstanceBoundExtra
        total_size += su_SIZE_OF_DARRAY;
        total_size += cfg->bound.instance_cfg.capacity * sizeof(struct sb_GFXInstanceData);
    }
    { // GFXDrawData
        total_size += sizeof(struct sb_GFXDrawData);
        total_size += su_SIZE_OF_DARRAY; // vertex_array;
        total_size += su_SIZE_OF_DARRAY; // index_array;
        total_size += su_SIZE_OF_DARRAY; // instance_data_array;
        total_size += su_SIZE_OF_DARRAY; // uniform_data_array;

        total_size += vertex_size * cfg->batch.vertex_cfg.capacity * batch_capacity;
        total_size += index_size * cfg->batch.index_cfg.capacity * batch_capacity;
    }
    { // RendererBound
        total_size += cfg->bound.index_cfg.capacity * index_size;
        total_size += cfg->uniform_array_length * sizeof(struct sb_GFXUniformData);
    }

    return su_mem_create_pool(su_MEM_CONTEXT_RENDERER, total_size);
}

void sb_renderer_cfg_copy_and_cleanup(struct su_RendererConfig* dest, struct su_RendererConfig* src, su_MemPool* pool)
{
    if (!src || !dest || !pool)
        return;
    {
        dest->name = su_mem_pool_cpy_str(src->name, pool);
        dest->shaders.frag = su_mem_pool_cpy_str(src->shaders.frag, pool);
        dest->shaders.vert = su_mem_pool_cpy_str(src->shaders.vert, pool);
        dest->shaders.geom = su_mem_pool_cpy_str(src->shaders.geom, pool);

        dest->uniform_array_length = src->uniform_array_length;
        dest->sampler_array_length = src->sampler_array_length;

        dest->vertex_data.element_size_internal = src->vertex_data.element_size_internal;
        dest->vertex_data.layout_array_length = src->vertex_data.layout_array_length;

        dest->index_data.element_size_internal = src->index_data.element_size_internal;

        dest->instance_data.buffer_array_length = src->instance_data.buffer_array_length;

        dest->batch = src->batch;
        dest->bound = src->bound;
        dest->draw = src->draw;
        dest->pipeline = src->pipeline;
    }
    { // Uniform
        if (src->uniform_array_length > 0 && src->uniform_array) {
            dest->uniform_array = (struct su_RendererCfgUniform*)
                su_mem_pool_alloc(pool, sizeof(struct su_RendererCfgUniform) * src->uniform_array_length);

            for (su_U64 i = 0; i < src->uniform_array_length; ++i) {
                dest->uniform_array[i] = src->uniform_array[i];
                dest->uniform_array[i].name = su_mem_pool_cpy_str(src->uniform_array[i].name, pool);
            }
        } else {
            dest->uniform_array = NULL;
        }
    }
    {
        if (src->sampler_array_length > 0 && src->sampler_array) {
            dest->sampler_array = (struct su_RendererCfgSampler*)
                su_mem_pool_alloc(pool, sizeof(struct su_RendererCfgSampler) * src->sampler_array_length);

            for (su_U64 i = 0; i < src->sampler_array_length; ++i) {
                dest->sampler_array[i] = src->sampler_array[i];
                dest->sampler_array[i].name = su_mem_pool_cpy_str(src->sampler_array[i].name, pool);
            }
        } else {
            dest->sampler_array = NULL;
        }
    }
    {
        if (src->vertex_data.layout_array_length > 0 && src->vertex_data.layout_array) {
            dest->vertex_data.layout_array = (struct su_RendererCfgVertexLayout*)
                su_mem_pool_alloc(pool, sizeof(struct su_RendererCfgVertexLayout) * src->vertex_data.layout_array_length);

            for (su_U64 i = 0; i < src->vertex_data.layout_array_length; ++i) {
                dest->vertex_data.layout_array[i] = src->vertex_data.layout_array[i];
                dest->vertex_data.layout_array[i].name = su_mem_pool_cpy_str(src->vertex_data.layout_array[i].name, pool);
            }
        } else {
            dest->vertex_data.layout_array = NULL;
        }
    }
    {
        if (src->instance_data.buffer_array_length > 0 && src->instance_data.buffer_array) {
            dest->instance_data.buffer_array = (struct su_RendererCfgInstanceBuffer*)
                su_mem_pool_alloc(pool, sizeof(struct su_RendererCfgInstanceBuffer) * src->instance_data.buffer_array_length);

            for (su_U64 i = 0; i < src->instance_data.buffer_array_length; ++i) {
                const struct su_RendererCfgInstanceBuffer* src_buf = &src->instance_data.buffer_array[i];
                struct su_RendererCfgInstanceBuffer* dst_buf = &dest->instance_data.buffer_array[i];

                *dst_buf = *src_buf;
                dst_buf->name = su_mem_pool_cpy_str(src_buf->name, pool);

                if (src_buf->layout_array_length > 0 && src_buf->layout_array) {
                    dst_buf->layout_array = (struct su_RendererCfgInstanceBufferLayout*)
                        su_mem_pool_alloc(pool, sizeof(struct su_RendererCfgInstanceBufferLayout) * src_buf->layout_array_length);

                    for (su_U64 j = 0; j < src_buf->layout_array_length; ++j) {
                        dst_buf->layout_array[j] = src_buf->layout_array[j];
                        dst_buf->layout_array[j].name = su_mem_pool_cpy_str(src_buf->layout_array[j].name, pool);
                    }
                } else {
                    dst_buf->layout_array = NULL;
                }
            }
        } else {
            dest->instance_data.buffer_array = NULL;
        }
    }

#ifndef __EMSCRIPTEN__
    su_cfg_manager_cleanup_renderer_cfg(dest);
#endif
}

// Internal

SA_INTERNAL void sb__renderer_instance_fill_default(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info)
{
    sb_init_uniforms(cfg_out, gfx_info);
    sb_init_samplers(cfg_out, gfx_info);
    sb_init_vertex_layout(cfg_out);
    sb__init_instance_buffers(cfg_out);
}

SA_INTERNAL void sb__init_instance_buffers(struct su_RendererConfig* cfg_out)
{
    if (!cfg_out->instance_data.buffer_array) {
        // TODO remove magic numbers, have this as a constant value somewhere else
        SA_STATIC struct su_RendererCfgInstanceBufferLayout layouts[] = {
            {
                .name = "i_model_matrix",
                .type = su_TYPE_MAT4,
                .offset = 0,
                .location = 3,
            },
            {
                .name = "i_color",
                .type = su_TYPE_COLOR,
                .offset = 64,
                .location = 7,
            },
        };
        SA_STATIC struct su_RendererCfgInstanceBuffer buffer = {
            .name = "default string buffer",
            .layout_array = NULL,
        };
        buffer.layout_array = layouts;
        buffer.layout_array_length = su_ARRLEN_M(layouts);

        cfg_out->instance_data.buffer_array = &buffer;
    }
}
