#ifndef SACI_BACKEND_SB_RENDERER_COMMON_H
#define SACI_BACKEND_SB_RENDERER_COMMON_H

#include "saci-backend/sb-renderer.h"
#include <saci-utils/su-debug.h>

#define ARENA_ASSERT(x) su_LOG_ASSERT_MESSAGE_M(x, "Error in arena function")
#define ARENA_FREE(x) su_FREE_M(x)
#define ARENA_MALLOC(x) su_MALLOC_M(x)
#define ARENA_ALLOCATOR_IMPL
#define ARENA_DEBUG
#include <arena/arena.h>

#include "saci-utils/su-types.h"

#include "saci-backend/sb-gfx.h"

#ifndef sb_RENDERER_STRUCT
#  define sb_RENDERER_STRUCT

struct sb_Vertex {
    su_Vec3 pos;
    su_Color color;
    su_Uv uv;
};

struct sb_StaticBatch {
    su_TextureId texture;
    su_DArray* index_array;
    su_DArray* vertex_array;
    su_DArray* uniform_data;
};

struct sb_IndexArrayInfo {
    su_U64 capacity;
    su_Bool is_fixed_size;
};

struct sb_VertexArrayInfo {
    su_U64 capacity;
    su_Bool is_fixed_size;
};

struct sb_UniformArrayInfo {
    su_U64 capacity;
    su_Bool is_fixed_size;
};

struct sb_RendererBoundInfo {
    su_U64 index_array_capacity;

    su_U64 uniform_array_capacity;
};

struct sb_InstanceBatchInfo {
    su_U64 vertex_array_capacity;
    su_U64 index_array_capacity;
    su_U64 uniform_array_capacity;
    su_U64 transform_array_capacity;

    su_U64 batch_struct_allocation_size;
    su_U8 batch_capacity;
    su_U8 in_use;
};

struct sb_InstanceBatchConfig {
    struct sb_IndexArrayInfo index_info;

    struct sb_VertexArrayInfo vertex_info;

    struct sb_TransformArrayInfo {
        su_U64 transform_array_max;
        su_Bool is_fixed_size;
    } transform_info;

    su_U64 batch_struct_allocation_size;

    su_U8 capacity;

    su_U8 in_use;

    su_Bool is_fixed_size;
};

struct sb_StaticBatchConfig {
    struct sb_IndexArrayInfo index_info;

    su_U64 batch_struct_allocation_size;

    su_U8 capacity;

    su_U8 in_use;

    su_Bool is_fixed_size;
};

struct sb_StaticBatchInfo {
    su_U64 vertex_array_capacity;
    su_U64 uniform_array_capacity;
    su_U64 index_array_capacity;
    su_U64 batch_struct_allocation_size;
    su_U8 batch_capacity;
    su_U8 in_use;
};

struct sb_RendererBound {
    su_TextureId texture;
    su_DArray* index_array;
    su_DArray* uniform_data_array;
};

struct sb_StaticRenderer {
    union sb_GFXInfo gfx;

    struct sb_StaticBatchInfo batch_info;
    struct sb_StaticBatch** batch_ptr_array;
    struct sb_RendererBoundInfo bound_info;

    struct sb_RendererBound bound;
    Arena batch_arena;
};

struct sb_InstanceData {
    su_Mat4 model;
    su_Color color;
};

struct sb_InstanceBoundExtra {
    su_DArray* bound_instance_array;
};

struct sb_InstanceRenderer {
    union sb_GFXInfo gfx;
    struct sb_InstanceBoundExtra bound_extra;
    su_BufferId instance_transform_vbo;
    su_BufferId instance_color_vbo;

    struct sb_InstanceBatchInfo batch_info;
    struct sb_GFXDrawData** batch_ptr_array;
    struct sb_RendererBoundInfo bound_info;

    struct sb_RendererBound bound;
    Arena batch_arena;
};

typedef void (*sb_RendererDrawFunction)(sb_Renderer*);
typedef void (*sb_RendererFreeFunction)(sb_Renderer*);

struct sb_RendererInterface {
    void (*new)(struct sb_Renderer* self);
    void (*begin)(const struct sb_Renderer* self);
    void (*bind_texture)(struct sb_Renderer* self, su_TextureId);
    void (*set_uniform)(struct sb_Renderer* self, su_S32, const void* const, su_DataType);
    void (*bind_index_buffer)(struct sb_Renderer* self, const su_DArray*);
    void (*push_mesh)(struct sb_Renderer* self, const su_DArray* vertex_array, const su_DArray* uv_array, const su_DArray* color_array);
    void (*draw)(const struct sb_Renderer* self);
    void (*free)(struct sb_Renderer* self);
    void (*free_opts)(struct sb_Renderer* self, int);
    su_S32 (*get_uniform_id)(struct sb_Renderer* self, const char* const);
};

struct sb_Renderer {
    enum sb_RendererType type;
    const struct sb_RendererInterface* interface;

    union sb_RendererUnion {
        struct sb_StaticRenderer* static_renderer;
        struct sb_InstanceRenderer* instance_renderer;
    } rendr;
};

#endif // sb_RENDERER_STRUCT

#define sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY (16000)

#define sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY su_SCAST_TO_M(su_U64)(sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 6 / 4)

#define sb_RENDERER_DEFAULT_BATCH_INDEX_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_BATCH_INDEX_ELEMENT_SIZE sizeof(su_U32)

#define sb_RENDERER_DEFAULT_BATCH_VERTEX_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_BATCH_VERTEX_STRUCT_SIZE sizeof(struct sb_Vertex)

#define sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY

#define sb_RENDERER_DEFAULT_UNIFORM_CAPACITY 1024

#define sb_RENDERER_DEFAULT_BATCH_CAPACITY (10)

#define sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_INDEX_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_VERTEX_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_UNIFORM_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_DEPTH_TEST su_TRUE

#define sb_RENDERER_DEFAULT_BLEND_IS_ENABLED su_TRUE

#define sb_RENDERER_DEFAULT_BLEND_OPTS 0

#define sb_RENDERER_DEFAULT_INSTANCE_CAPACITY 1024

#define SB_RENDERER_DEFAULT_INSTANCE_IS_FIXED_SIZE su_TRUE

// TODO needs to be evaluated
#define sb_RENDERER_DEFAULT_INSTANCE_TRANSFORM_ARRAY_SIZE 1024

#define sb_RENDERER_DEFAULT_INSTANCE_TRANSFORM_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH (300)

SA_INTERNAL const struct sb_RendererConfig sb_CFG_DEFAULT = {
    .name = NULL,
    .shaders = {
        .frag = NULL,
        .geom = NULL,
        .vert = NULL,
    },
    .uniform_array = NULL,
    .sampler_array = NULL,
    .batch = {
        .capacity = sb_RENDERER_DEFAULT_BATCH_CAPACITY,
        .fixed_capacity = sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE,
        .index_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY,
            .fixed_size = sb_RENDERER_DEFAULT_BATCH_INDEX_IS_FIXED_SIZE,
            .element_size = sb_RENDERER_DEFAULT_BATCH_INDEX_ELEMENT_SIZE,
        },
        .vertex_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            .fixed_size = sb_RENDERER_DEFAULT_BATCH_VERTEX_IS_FIXED_SIZE,
            .struct_size = sb_RENDERER_DEFAULT_BATCH_VERTEX_STRUCT_SIZE,
            .vertex_layout_array = NULL,
        },
    },
    .draw = {
        .primitive = sb_PRIMITIVES_TRIANGLES,
        .cull_mode = sb_RENDERER_CULL_MODE_BACK,
        .front_face = sb_RENDERER_FRONT_FACE_CCW,
    },
    .pipeline = {
        .depth_test = sb_RENDERER_DEFAULT_DEPTH_TEST,
        .blend = {
            .enabled = sb_RENDERER_DEFAULT_BLEND_IS_ENABLED,
            .opts = sb_RENDERER_DEFAULT_BLEND_OPTS,
        },
    },
};

SA_INTERNAL const struct sb_RendererConfig sb_CFG_DEFAULT_INSTANCE = {
    .name = NULL,
    .shaders = {
        .vert = NULL,
        .frag = NULL,
        .geom = NULL,
    },
    .uniform_array = NULL,
    .sampler_array = NULL,
    .batch = {
        .capacity = sb_RENDERER_DEFAULT_BATCH_CAPACITY,
        .fixed_capacity = sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE,
        .index_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY,
            .fixed_size = sb_RENDERER_DEFAULT_BATCH_INDEX_IS_FIXED_SIZE,
            .element_size = sb_RENDERER_DEFAULT_BATCH_INDEX_ELEMENT_SIZE,
        },
        .vertex_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            .fixed_size = sb_RENDERER_DEFAULT_BATCH_VERTEX_IS_FIXED_SIZE,
            .struct_size = sb_RENDERER_DEFAULT_BATCH_VERTEX_STRUCT_SIZE,
            .vertex_layout_array = NULL,
        },
        .instance_cfg = {
            .capacity = sb_RENDERER_DEFAULT_INSTANCE_CAPACITY,
            .fixed_size = SB_RENDERER_DEFAULT_INSTANCE_IS_FIXED_SIZE,
            .buffer_array = NULL,
        },
    },
    .draw = {
        .primitive = sb_PRIMITIVES_TRIANGLES,
        .cull_mode = sb_RENDERER_CULL_MODE_BACK,
        .front_face = sb_RENDERER_FRONT_FACE_CCW,
    },
    .pipeline = {
        .depth_test = sb_RENDERER_DEFAULT_DEPTH_TEST,
        .blend = {
            .enabled = sb_RENDERER_DEFAULT_BLEND_IS_ENABLED,
            .opts = sb_RENDERER_DEFAULT_BLEND_OPTS,
        },
    },
};

extern void sb_renderer_instanced_new(sb_Renderer* rendr); // Defined in sb-renderer-instance

SA_INTERNAL const struct sb_RendererInterface sb__RENDERER_INSTANCE_INTERFACE_DEFAULT_INITIALIZER =
    {
        .new = sb_renderer_instanced_new,
};

/* --- Renderer Header Impl --- */

sb_Renderer* sb_renderer_new(const enum sb_RendererType type) {
    sb_Renderer* rendr = su_MALLOC_M(sizeof(struct sb_Renderer));
    rendr->type = type;
    switch (type) {
    case sb_RENDERER_STATIC:
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        rendr->type = sb_RENDERER_INSTANCE;
        rendr->interface = &sb__RENDERER_INSTANCE_INTERFACE_DEFAULT_INITIALIZER;
        rendr->interface->new(rendr);
        break;
    }
    return rendr;
}

void sb_renderer_begin(struct sb_Renderer* rendr) {
    rendr->interface->begin(rendr);
}

void sb_renderer_bind_texture(struct sb_Renderer* rendr, su_TextureId tex_id) {
    rendr->interface->bind_texture(rendr, tex_id);
}

su_S32 sb_renderer_get_uniform_id(struct sb_Renderer* rendr,
                                  const char* const uniform_name) {
    return rendr->interface->get_uniform_id(rendr, uniform_name);
}

void sb_renderer_set_uniform(struct sb_Renderer* rendr,
                             const su_S32 uniform_id,
                             const void* const value,
                             const su_DataType type) {
    rendr->interface->set_uniform(rendr, uniform_id, value, type);
}

void sb_renderer_bind_index_buffer(struct sb_Renderer* rendr,
                                   const su_DArray* new_indices) {
    rendr->interface->bind_index_buffer(rendr, new_indices);
}

void sb_renderer_push_mesh(struct sb_Renderer* rendr,
                           const su_DArray* pos_array,
                           const su_DArray* uv_array,
                           const su_DArray* color_array) {
    rendr->interface->push_mesh(rendr, pos_array, uv_array, color_array);
}

void sb_renderer_draw(const struct sb_Renderer* rendr) {
    rendr->interface->draw(rendr);
}

void sb_renderer_free(struct sb_Renderer* rendr) {
    rendr->interface->free(rendr);
}

void sb_renderer_free_opts(struct sb_Renderer* rendr, int free_opts) {
    rendr->interface->free_opts(rendr, free_opts);
}

/* --- FUNCS USED IN OTHER RENDERER FILES --- */

void sb_renderer_init_bound(struct sb_RendererBound* bound_out, struct sb_RendererBoundInfo* bound_cfg_out, const struct sb_RendererConfig cfg) {
    bound_cfg_out->index_array_capacity = cfg.batch.index_cfg.capacity
                                              ? cfg.batch.index_cfg.capacity
                                              : sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    bound_cfg_out->uniform_array_capacity = !su_darray_is_empty(cfg.uniform_array)
                                                ? su_darray_length(cfg.uniform_array)
                                                : sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;

    bound_out->index_array = su_darray_create(
        bound_cfg_out->index_array_capacity,
        sizeof(su_U32),
        cfg.batch.index_cfg.fixed_size);

    // TODO
    bound_out->uniform_data_array = su_darray_create(
        bound_cfg_out->uniform_array_capacity,
        sizeof(struct sb_GFXUniformData),
        cfg.batch.index_cfg.fixed_size);
}

void sb_init_uniforms(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
    SA_INTERNAL struct sb_RendererCfgUniform uniform_array[6] = {
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_S32},
        {.name = NULL, .location = 0, .type = su_TYPE_VEC4},
        {.name = NULL, .location = 0, .type = su_TYPE_VEC4},
    };

    if (!uniform_array[0].name) {
        uniform_array[0].name = su_string_create("u_model_matrix", su_REALLOCATION_KIND_FIXED_SIZE);
        uniform_array[1].name = su_string_create("u_view_matrix", su_REALLOCATION_KIND_FIXED_SIZE);
        uniform_array[2].name = su_string_create("u_projection_matrix", su_REALLOCATION_KIND_FIXED_SIZE);
        uniform_array[3].name = su_string_create("u_flags", su_REALLOCATION_KIND_FIXED_SIZE);
        uniform_array[4].name = su_string_create("u_lighting", su_REALLOCATION_KIND_FIXED_SIZE);
        uniform_array[5].name = su_string_create("u_use_texture", su_REALLOCATION_KIND_FIXED_SIZE);

        for (su_U64 i = 0; i < su_ARRLEN_M(uniform_array); ++i) {
            uniform_array[i].location = sb_gfx_get_uniform_loc(gfx_info, uniform_array[i].name);
        }
    }

    if (!cfg_out->uniform_array) {
        cfg_out->uniform_array = su_darray_create(
            su_ARRLEN_M(uniform_array),
            sizeof(struct sb_RendererCfgUniform),
            su_TRUE);
        for (su_U64 i = 0; i < su_ARRLEN_M(uniform_array); ++i) {
            su_darray_push(cfg_out->uniform_array, &uniform_array[i]);
        }
    }
}

void sb_init_samplers(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
    SA_INTERNAL struct sb_RendererCfgSampler sampler_array[1] = {
        {.name = NULL, .binding = 0, .type = su_TYPE_SAMPLER2D}};

    if (!sampler_array[0].name) {
        sampler_array[0].name = su_string_create("u_texture", su_REALLOCATION_KIND_FIXED_SIZE);
        sampler_array[0].binding = sb_gfx_get_uniform_loc(gfx_info, sampler_array[0].name);
        sampler_array[0].unit = 0;
    }

    if (!cfg_out->sampler_array) {
        cfg_out->sampler_array = su_darray_create(
            su_ARRLEN_M(sampler_array),
            sizeof(struct sb_RendererCfgSampler),
            su_TRUE);
        for (su_U64 i = 0; i < su_ARRLEN_M(sampler_array); ++i) {
            su_darray_push(cfg_out->sampler_array, &sampler_array[i]);
        }
    }
}

void sb_init_vertex_layout(struct sb_RendererConfig* cfg_out) {
    SA_INTERNAL struct sb_RendererCfgVertexLayout v_layout[3] = {
        {.name = NULL, .type = su_TYPE_VEC3, .location = 0, .offset = 0},
        {.name = NULL, .type = su_TYPE_VEC4, .location = 1, .offset = 12},
        {.name = NULL, .type = su_TYPE_UV, .location = 2, .offset = 38},
    };

    if (!v_layout[0].name) {
        v_layout[0].name = su_string_create("pos", su_REALLOCATION_KIND_FIXED_SIZE);
        v_layout[1].name = su_string_create("color", su_REALLOCATION_KIND_FIXED_SIZE);
        v_layout[2].name = su_string_create("uv", su_REALLOCATION_KIND_FIXED_SIZE);
    }

    if (!cfg_out->batch.vertex_cfg.vertex_layout_array) {
        cfg_out->batch.vertex_cfg.vertex_layout_array =
            su_darray_create(3, sizeof(struct sb_RendererCfgVertexLayout), su_TRUE);
        for (su_U64 i = 0; i < su_ARRLEN_M(v_layout); ++i) {
            su_darray_push(cfg_out->batch.vertex_cfg.vertex_layout_array, &v_layout[i]);
        }
    }
}

void sb_init_shaders(struct sb_RendererConfig* cfg_out) {
    // if (!cfg_out->shaders.vert) {
    //     cfg_out->shaders.vert = su_string_create(sb_INSTANCE_VERT_SHADER, su_REALLOCATION_KIND_FIXED_SIZE);
    // }
    // if (!cfg_out->shaders.frag) {
    //     cfg_out->shaders.frag = su_string_create(sb_FRAG_SHADER, su_REALLOCATION_KIND_FIXED_SIZE);
    // }
}

union sb_GFXUniformValue sb_renderer_uniform_value_from_type(su_DataType type, const void* value) {
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
        result.vec3 = *(const su_Vec3*)value;
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
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", type);
        break;
    }

    return result;
}

#endif // SACI_BACKEND_SB_RENDERER_COMMON_H
