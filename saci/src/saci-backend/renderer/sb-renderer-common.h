#ifndef SACI_BACKEND_SB_RENDERER_COMMON_H
#define SACI_BACKEND_SB_RENDERER_COMMON_H

#include "saci-backend/renderer/sb-renderer.h"
#include "saci-utils/su-log.h"
#include "saci-utils/su-types-common.h"

#include "saci-backend/graphics/sb-gfx.h"

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

    struct sb_RendererBound bound;
    void* batch_arena; // TODO
};

struct sb_InstanceBoundExtra {
    su_DArray* bound_instance_data_array;
};

struct sb_InstanceRenderer {
    union sb_GFXInfo gfx;
    struct su_RendererConfig cfg;
    struct sb_InstanceBoundExtra bound_extra;
    su_BufferId instance_transform_vbo;
    su_BufferId instance_color_vbo;

    struct sb_GFXDrawData* batch_array;

    struct sb_RendererBound bound;

    struct {
        su_U8 in_use;
    } batch_info;
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

#define sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY su_CAST_M(su_U64)(sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 6 / 4)

#define sb_RENDERER_DEFAULT_BATCH_INDEX_ELEMENT_SIZE sizeof(su_U32)

#define sb_RENDERER_DEFAULT_BATCH_VERTEX_STRUCT_SIZE sizeof(struct sb_Vertex)

#define sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY

#define sb_RENDERER_DEFAULT_UNIFORM_CAPACITY 1024

#define sb_RENDERER_DEFAULT_BOUND_UNIFORM_CAPACITY sb_RENDERER_DEFAULT_UNIFORM_CAPACITY

#define sb_RENDERER_DEFAULT_BATCH_CAPACITY (10)

#define sb_RENDERER_DEFAULT_DEPTH_TEST su_TRUE

#define sb_RENDERER_DEFAULT_BLEND_IS_ENABLED su_TRUE

#define sb_RENDERER_DEFAULT_BLEND_OPTS 0

#define sb_RENDERER_DEFAULT_INSTANCE_CAPACITY 1024

#define sb_RENDERER_DEFAULT_BOUND_INSTANCE_CAPACITY sb_RENDERER_DEFAULT_INSTANCE_CAPACITY

#define sb_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH (300)

SA_INTERNAL const struct su_RendererConfig sb_CFG_DEFAULT = {
    .name = NULL,
    .shaders = {
        .frag = NULL,
        .geom = NULL,
        .vert = NULL,
    },
    .vertex_data = {
        .element_size_internal = sb_RENDERER_DEFAULT_BATCH_VERTEX_STRUCT_SIZE,
        .layout_array = NULL,
    },
    .index_data = {
        .element_size_internal = sb_RENDERER_DEFAULT_BATCH_INDEX_ELEMENT_SIZE,
    },
    .instance_data = {
        .buffer_array = NULL,
    },
    .uniform_array = NULL,
    .sampler_array = NULL,
    .batch = {
        .capacity = sb_RENDERER_DEFAULT_BATCH_CAPACITY,
        .fixed_capacity = su_TRUE,
        .index_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY,
            .fixed_size = su_TRUE,
        },
        .vertex_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            .fixed_size = su_TRUE,
        },
    },
    .draw = {
        .primitive = su_PRIMITIVES_TRIANGLES,
        .cull_mode = su_RENDERER_CULL_MODE_BACK,
        .front_face = su_RENDERER_FRONT_FACE_CCW,
    },
    .pipeline = {
        .depth_test = sb_RENDERER_DEFAULT_DEPTH_TEST,
        .blend = {
            .enabled = sb_RENDERER_DEFAULT_BLEND_IS_ENABLED,
            .opts = sb_RENDERER_DEFAULT_BLEND_OPTS,
        },
    },
};

SA_INTERNAL const struct su_RendererConfig sb_CFG_DEFAULT_INSTANCE = {
    .name = NULL,
    .shaders = {
        .vert = NULL,
        .frag = NULL,
        .geom = NULL,
    },
    .uniform_array = NULL,
    .sampler_array = NULL,
    .vertex_data = {
        .element_size_internal = sb_RENDERER_DEFAULT_BATCH_VERTEX_STRUCT_SIZE,
        .layout_array = NULL,
    },
    .index_data = {
        .element_size_internal = sb_RENDERER_DEFAULT_BATCH_INDEX_ELEMENT_SIZE,
    },
    .bound = {
        .index_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY,
            .fixed_size = su_TRUE,
        },
        .instance_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BOUND_INSTANCE_CAPACITY,
            .fixed_size = su_TRUE,
        },
    },
    .batch = {
        .capacity = sb_RENDERER_DEFAULT_BATCH_CAPACITY,
        .fixed_capacity = su_TRUE,
        .index_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY,
            .fixed_size = su_TRUE,
        },
        .vertex_cfg = {
            .capacity = sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            .fixed_size = su_TRUE,
        },
        .instance_cfg = {
            .capacity = sb_RENDERER_DEFAULT_INSTANCE_CAPACITY,
            .fixed_size = su_TRUE,
        },
    },
    .draw = {
        .primitive = su_PRIMITIVES_TRIANGLES,
        .cull_mode = su_RENDERER_CULL_MODE_BACK,
        .front_face = su_RENDERER_FRONT_FACE_CCW,
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
    sb_Renderer* rendr = malloc(sizeof(struct sb_Renderer));
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

void sb_renderer_init_bound(struct sb_RendererBound* bound_out, const struct su_RendererConfig cfg);

void sb_init_uniforms(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info);

void sb_init_samplers(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info);

void sb_init_vertex_layout(struct su_RendererConfig* cfg_out);

void sb_init_shaders(struct su_RendererConfig* cfg, union sb_GFXInfo* info_out);

union sb_GFXUniformValue sb_renderer_uniform_value_from_type(su_DataType type, const void* value);

#endif // SACI_BACKEND_SB_RENDERER_COMMON_H
