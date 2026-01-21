#include "saci_core/renderer/internal/renderer.h"

#include "saci_util/log.h"
#include "saci_util/internal/log.h"

#include "saci_platform/gfx/internal/gfx.h"
#include "saci_platform/dependencies/internal/dependency.h"

#ifndef __EMSCRIPTEN__
#  include "saci_platform/config/config.h"
#endif

#include <stdio.h>

// Internal
SACI_INTERNAL void csaci__init_instance_buffers(struct PSaciConfigRenderer* cfg_out);

SACI_INTERNAL void csaci__renderer_instance_fill_default(struct PSaciConfigRenderer* cfg_out, const union PSaciGFXInfo* gfx_info);

SACI_INTERNAL_CONST struct PSaciConfigRenderer CSACI_G_INSTANCE_CONFIG_DEFAULT = {
    .name = "instance",
    .vertex_attributes = {
        .element_array = (struct PSaciRendererCfgVertexElement[3]){
            {.name = "position", .type = SACI_TYPE_VEC3, .offset = 0, .location = 0},
            {.name = "color", .type = SACI_TYPE_COLOR, .offset = 12, .location = 1},
            {.name = "uv", .type = SACI_TYPE_UV, .offset = 28, .location = 2},
        },
        .element_array_length = 3,
        .element_size_internal = 36,
    },
    .index_data = {
        .element_size_internal = SACI_G_TYPE_SIZE_TABLE[SACI_TYPE_U32],
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
    .uniform_array = (struct PSaciRendererCfgUniform[7]){
        {.name = "u_texture", .type = SACI_TYPE_SAMPLER2D},
        {.name = "u_use_texture", .type = SACI_TYPE_BOOL},
        {.name = "u_model_matrix", .type = SACI_TYPE_MAT4},
        {.name = "u_view_matrix", .type = SACI_TYPE_MAT4},
        {.name = "u_projection_matrix", .type = SACI_TYPE_MAT4},
        {.name = "u_flags", .type = SACI_TYPE_S32},
        {.name = "u_lighting", .type = SACI_TYPE_VEC4},
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
        .primitive = PSACI_PRIMITIVES_TRIANGLES,
        .cull_mode = PSACI_RENDERER_CULL_MODE_BACK,
        .front_face = PSACI_RENDERER_FRONT_FACE_CCW,
    },
    .pipeline = {
        .depth_test = SACI_TRUE,
        .blend = {
            .enabled = true,
            .opts = 0,
        },
    },
};

// Header impl

CSaciRenderer* csaci_renderer_new(const enum CSaciRendererType type, const char* name)
{
    struct PSaciConfigRenderer cfg = {0};
    union PSaciGFXInfo info = {0};
    const char* name_ptr = name;

    if (!name) {
        switch (type) {
        case CSACI_RENDERER_STATIC:
            name_ptr = "static";
            break;
        case CSACI_RENDERER_DYNAMIC:
            name_ptr = "dynamic";
            break;
        case CSACI_RENDERER_INSTANCE:
            name_ptr = "instance";
            cfg = CSACI_G_INSTANCE_CONFIG_DEFAULT;
            break;
        }
    }
    psaci_cfg_renderer_get(name_ptr, &cfg, psaci_dependencies_get_cfg_file_path());
    csaci_init_shaders(&cfg, &info);
    csaci__renderer_instance_fill_default(&cfg, &info);
    SaciMemPool* pool = csaci_renderer_get_pool_from_cfg(&cfg, type);
    CSaciRenderer* rendr = saci_mem_pool_alloc(pool, sizeof(struct CSaciRenderer));
    rendr->type = type;

    switch (type) {
    case CSACI_RENDERER_STATIC:
        SACI_TODO_M;
        break;
    case CSACI_RENDERER_DYNAMIC:
        SACI_TODO_M;
        break;
    case CSACI_RENDERER_INSTANCE:
        rendr->interface = &CSACI_G_RENDERER_INSTANCE_INTERFACE_DEFAULT_INITIALIZER;
        rendr->interface->new (rendr, pool, &cfg, &info);
        break;
    }
    return rendr;
}

void csaci_renderer_begin(struct CSaciRenderer* rendr)
{
    rendr->interface->begin(rendr);
}

void csaci_renderer_bind_texture(struct CSaciRenderer* rendr, union PSaciTexture tex_id)
{
    rendr->interface->bind_texture(rendr, tex_id);
}

SaciS32 csaci_renderer_get_uniform_id(struct CSaciRenderer* rendr,
                                      const char* const uniform_name)
{
    return rendr->interface->get_uniform_id(rendr, uniform_name);
}

void csaci_renderer_set_uniform(struct CSaciRenderer* rendr,
                                const SaciS32 uniform_id,
                                const void* const value,
                                const SaciDataType type)
{
    rendr->interface->set_uniform(rendr, uniform_id, value, type);
}

void csaci_renderer_bind_index_buffer(struct CSaciRenderer* rendr,
                                      const CSaciIndex* data, const SaciU64 count)
{
    rendr->interface->bind_index_buffer(rendr, data, count);
}

// TODO

SACI_API void csaci_renderer_push_mesh(struct CSaciRenderer* rendr,
                                       const CSaciVertex* data, const SaciU64 count)
{
    rendr->interface->push_mesh(rendr, data, count);
}

void csaci_renderer_draw(const struct CSaciRenderer* rendr)
{
    rendr->interface->draw(rendr);
}

void csaci_renderer_free(struct CSaciRenderer* rendr)
{
    rendr->interface->free(rendr);
}

void csaci_renderer_init_bound(struct CSaciRendererBound* bound_out, const struct PSaciConfigRenderer cfg, SaciMemPool* mem)
{
    // bound_out->index_array = su_darray_create(
    //     cfg.bound.index_cfg.capacity,
    //     sizeof(SaciU32),
    //     SACI_FALSE);
    //
    // // TODO
    // bound_out->uniform_data_array = su_darray_create(
    //     cfg.uniform_array_length,
    //     sizeof(struct PSaciGFXUniformData),
    //     SACI_FALSE);
}

void csaci_init_uniforms(struct PSaciConfigRenderer* cfg_out, const union PSaciGFXInfo* gfx_info)
{
    SACI_INTERNAL struct PSaciRendererCfgUniform csaci__uniform_array[6] = {
        {.name = NULL, .location_internal = 0, .type = SACI_TYPE_MAT4},
        {.name = NULL, .location_internal = 0, .type = SACI_TYPE_MAT4},
        {.name = NULL, .location_internal = 0, .type = SACI_TYPE_MAT4},
        {.name = NULL, .location_internal = 0, .type = SACI_TYPE_S32},
        {.name = NULL, .location_internal = 0, .type = SACI_TYPE_VEC4},
        {.name = NULL, .location_internal = 0, .type = SACI_TYPE_VEC4},
    };

    if (!csaci__uniform_array[0].name) {
        csaci__uniform_array[0].name = "u_model_matrix";
        csaci__uniform_array[1].name = "u_view_matrix";
        csaci__uniform_array[2].name = "u_projection_matrix";
        csaci__uniform_array[3].name = "u_flags";
        csaci__uniform_array[4].name = "u_lighting";
        csaci__uniform_array[5].name = "u_use_texture";

        for (SaciU64 i = 0; i < SACI_ARRLEN_M(csaci__uniform_array); ++i) {
            csaci__uniform_array[i].location_internal = psaci_gfx_get_uniform_loc(gfx_info, csaci__uniform_array[i].name);
        }
    }

    if (!cfg_out->uniform_array) {
        // TODO
#if 0
        cfg_out->uniform_array = su_darray_create(
            SACI_ARRLEN_M(uniform_array),
            sizeof(struct PSaciRendererCfgUniform),
            su_TRUE);
        for (SaciU64 i = 0; i < SACI_ARRLEN_M(uniform_array); ++i) {
            su_darray_push(cfg_out->uniform_array, &uniform_array[i]);
        }
#endif
    }
}

void csaci_init_samplers(struct PSaciConfigRenderer* cfg_out, const union PSaciGFXInfo* gfx_info)
{
    SACI_INTERNAL struct PSaciRendererCfgSampler csaci__sampler_array[1] = {
        {.name = NULL, .binding = 0, .type = SACI_TYPE_SAMPLER2D}};

    if (!csaci__sampler_array[0].name) {
        csaci__sampler_array[0].name = "u_texture";
        csaci__sampler_array[0].binding = psaci_gfx_get_uniform_loc(gfx_info, csaci__sampler_array[0].name);
        csaci__sampler_array[0].unit = 0;
    }

    if (!cfg_out->sampler_array) {
        // cfg_out->sampler_array = su_darray_create(
        //     SACI_ARRLEN_M(sampler_array),
        //     sizeof(struct PSaciRendererCfgSampler),
        //     su_TRUE);
        // for (SaciU64 i = 0; i < SACI_ARRLEN_M(sampler_array); ++i) {
        //     su_darray_push(cfg_out->sampler_array, &sampler_array[i]);
        // }
    }
}

void csaci_init_shaders(struct PSaciConfigRenderer* cfg, union PSaciGFXInfo* info_out)
{
    psaci_gfx_init_shader(info_out, *cfg);
}

union PSaciGFXUniformValue csaci_renderer_uniform_value_from_type(SaciDataType type, const void* value)
{
    union PSaciGFXUniformValue result = {0};

    switch (type) {
    case SACI_TYPE_U8:
        result.u8 = *(const SaciU8*)value;
        break;
    case SACI_TYPE_U16:
        result.u16 = *(const SaciU16*)value;
        break;
    case SACI_TYPE_U32:
        result.u32 = *(const SaciU32*)value;
        break;
    case SACI_TYPE_U64:
        result.u64 = *(const SaciU64*)value;
        break;

    case SACI_TYPE_S8:
        result.s8 = *(const SaciS8*)value;
        break;
    case SACI_TYPE_S16:
        result.s16 = *(const SaciS16*)value;
        break;
    case SACI_TYPE_S32:
        result.s32 = *(const SaciS32*)value;
        break;
    case SACI_TYPE_S64:
        result.s64 = *(const SaciS64*)value;
        break;

    case SACI_TYPE_BOOL:
        result.boolean = *(const SaciBool*)value;
        break;

    case SACI_TYPE_UV:
        result.uv = *(const SaciUv*)value;
        break;
    case SACI_TYPE_VEC2:
        result.vec2 = *(const SaciVec2*)value;
        break;
    case SACI_TYPE_VEC3:
        result.vec3 = *(const SaciVec3*)value;
        break;
    case SACI_TYPE_VEC4:
        result.vec4 = *(const SaciVec4*)value;
        break;

    case SACI_TYPE_COLOR:
        result.color = *(const SaciColor*)value;
        break;

    case SACI_TYPE_MAT2:
    case SACI_TYPE_MAT3:
        break;
    case SACI_TYPE_MAT4:
        result.mat4 = *(const SaciMat4*)value;
        break;

    case SACI_TYPE_MAT2X3:
    case SACI_TYPE_MAT2X4:
    case SACI_TYPE_MAT3X2:
    case SACI_TYPE_MAT3X4:
    case SACI_TYPE_MAT4X2:
    case SACI_TYPE_MAT4X3:
        break;

    default:
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_RENDERER,
                          "Invalid type %d for uniform", type);
        break;
    }

    return result;
}

SaciMemPool* csaci_renderer_get_pool_from_cfg(const struct PSaciConfigRenderer* cfg, enum CSaciRendererType type)
{
    SaciU64 total_size = 0,
            vertex_size = cfg->vertex_attributes.element_size_internal,
            index_size = cfg->index_data.element_size_internal,
            batch_capacity = cfg->batch.capacity;

    total_size += sizeof(struct CSaciRenderer);
    // total_size += sizeof(struct sb_RendererInterface); // References a static var, doesn't need to be allocated

    // If there is any instance buffer data
    switch (type) {
    case CSACI_RENDERER_STATIC:
        total_size += sizeof(struct CSaciStaticRenderer);
        break;
    case CSACI_RENDERER_DYNAMIC:
        break;
    case CSACI_RENDERER_INSTANCE:
        total_size += sizeof(struct CSaciInstanceRenderer);
        break;
    }
    { // InstanceBoundExtra
        // total_size += su_SIZE_OF_DARRAY; // TODO
        total_size += cfg->bound.instance_cfg.capacity * sizeof(struct PSaciGFXInstanceData);
    }
    { // GFXDrawData
        total_size += sizeof(struct PSaciGFXDrawData);
        // total_size += su_SIZE_OF_DARRAY; // vertex_array;
        // total_size += su_SIZE_OF_DARRAY; // index_array;
        // total_size += su_SIZE_OF_DARRAY; // instance_data_array;
        // total_size += su_SIZE_OF_DARRAY; // uniform_data_array;

        total_size += vertex_size * cfg->batch.vertex_cfg.capacity * batch_capacity;
        total_size += index_size * cfg->batch.index_cfg.capacity * batch_capacity;
    }
    { // RendererBound
        total_size += cfg->bound.index_cfg.capacity * index_size;
        total_size += cfg->uniform_array_length * sizeof(struct PSaciGFXUniformData);
    }

    return saci_mem_create_pool(SACI_MEM_CONTEXT_RENDERER, total_size);
}

void csaci_renderer_cfg_copy_and_cleanup(struct PSaciConfigRenderer* dest, struct PSaciConfigRenderer* src, SaciMemPool* pool)
{
    if (!src || !dest || !pool) {
        return;
    }
    {
        dest->name = saci_mem_pool_cpy_str(src->name, pool);
        dest->shaders.frag = saci_mem_pool_cpy_str(src->shaders.frag, pool);
        dest->shaders.vert = saci_mem_pool_cpy_str(src->shaders.vert, pool);
        dest->shaders.geom = saci_mem_pool_cpy_str(src->shaders.geom, pool);

        dest->uniform_array_length = src->uniform_array_length;
        dest->sampler_array_length = src->sampler_array_length;

        dest->vertex_attributes.element_size_internal = src->vertex_attributes.element_size_internal;
        dest->vertex_attributes.element_array_length = src->vertex_attributes.element_array_length;

        dest->index_data.element_size_internal = src->index_data.element_size_internal;

        dest->instance_data.buffer_array_length = src->instance_data.buffer_array_length;

        dest->batch = src->batch;
        dest->bound = src->bound;
        dest->draw = src->draw;
        dest->pipeline = src->pipeline;
    }
    { // Uniform
        if (src->uniform_array_length > 0 && src->uniform_array) {
            dest->uniform_array = (struct PSaciRendererCfgUniform*)
                saci_mem_pool_alloc(pool, sizeof(struct PSaciRendererCfgUniform) * src->uniform_array_length);

            for (SaciU64 i = 0; i < src->uniform_array_length; ++i) {
                dest->uniform_array[i] = src->uniform_array[i];
                dest->uniform_array[i].name = saci_mem_pool_cpy_str(src->uniform_array[i].name, pool);
            }
        } else {
            dest->uniform_array = NULL;
        }
    }
    {
        if (src->sampler_array_length > 0 && src->sampler_array) {
            dest->sampler_array = (struct PSaciRendererCfgSampler*)
                saci_mem_pool_alloc(pool, sizeof(struct PSaciRendererCfgSampler) * src->sampler_array_length);

            for (SaciU64 i = 0; i < src->sampler_array_length; ++i) {
                dest->sampler_array[i] = src->sampler_array[i];
                dest->sampler_array[i].name = saci_mem_pool_cpy_str(src->sampler_array[i].name, pool);
            }
        } else {
            dest->sampler_array = NULL;
        }
    }
    {
        if (src->vertex_attributes.element_array_length > 0 && src->vertex_attributes.element_array) {
            dest->vertex_attributes.element_array = (struct PSaciRendererCfgVertexElement*)
                saci_mem_pool_alloc(pool, sizeof(struct PSaciRendererCfgVertexElement) * src->vertex_attributes.element_array_length);

            for (SaciU64 i = 0; i < src->vertex_attributes.element_array_length; ++i) {
                dest->vertex_attributes.element_array[i] = src->vertex_attributes.element_array[i];
                dest->vertex_attributes.element_array[i].name = saci_mem_pool_cpy_str(src->vertex_attributes.element_array[i].name, pool);
            }
        } else {
            dest->vertex_attributes.element_array = NULL;
        }
    }
    {
        if (src->instance_data.buffer_array_length > 0 && src->instance_data.buffer_array) {
            dest->instance_data.buffer_array = (struct PSaciRendererCfgInstanceBuffer*)
                saci_mem_pool_alloc(pool, sizeof(struct PSaciRendererCfgInstanceBuffer) * src->instance_data.buffer_array_length);

            for (SaciU64 i = 0; i < src->instance_data.buffer_array_length; ++i) {
                const struct PSaciRendererCfgInstanceBuffer* src_buf = &src->instance_data.buffer_array[i];
                struct PSaciRendererCfgInstanceBuffer* dst_buf = &dest->instance_data.buffer_array[i];

                *dst_buf = *src_buf;
                dst_buf->name = saci_mem_pool_cpy_str(src_buf->name, pool);

                if (src_buf->layout_array_length > 0 && src_buf->layout_array) {
                    dst_buf->layout_array = (struct PSaciRendererCfgInstanceBufferLayout*)
                        saci_mem_pool_alloc(pool, sizeof(struct PSaciRendererCfgInstanceBufferLayout) * src_buf->layout_array_length);

                    for (SaciU64 j = 0; j < src_buf->layout_array_length; ++j) {
                        dst_buf->layout_array[j] = src_buf->layout_array[j];
                        dst_buf->layout_array[j].name = saci_mem_pool_cpy_str(src_buf->layout_array[j].name, pool);
                    }
                } else {
                    dst_buf->layout_array = NULL;
                }
            }
        } else {
            dest->instance_data.buffer_array = NULL;
        }
    }

    psaci_cfg_renderer_cleanup(dest);
}

// Internal

SACI_INTERNAL void csaci__renderer_instance_fill_default(struct PSaciConfigRenderer* cfg_out, const union PSaciGFXInfo* gfx_info)
{
    csaci_init_uniforms(cfg_out, gfx_info);
    csaci_init_samplers(cfg_out, gfx_info);
    csaci_init_vertex_layout(cfg_out);
    csaci__init_instance_buffers(cfg_out);
}

SACI_INTERNAL void csaci__init_instance_buffers(struct PSaciConfigRenderer* cfg_out)
{
    if (!cfg_out->instance_data.buffer_array) {
        // TODO remove magic numbers, have this as a constant value somewhere else
        SACI_STATIC struct PSaciRendererCfgInstanceBufferLayout csaci__layouts[] = {
            {
                .name = "i_model_matrix",
                .type = SACI_TYPE_MAT4,
                .offset = 0,
                .location = 3,
            },
            {
                .name = "i_color",
                .type = SACI_TYPE_COLOR,
                .offset = 64,
                .location = 7,
            },
        };
        SACI_STATIC struct PSaciRendererCfgInstanceBuffer csaci__buffer = {
            .name = "default string buffer",
            .layout_array = NULL,
        };
        csaci__buffer.layout_array = csaci__layouts;
        csaci__buffer.layout_array_length = SACI_ARRLEN_M(csaci__layouts);

        cfg_out->instance_data.buffer_array = &csaci__buffer;
    }
}
