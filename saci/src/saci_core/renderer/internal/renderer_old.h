#ifndef SACI_BACKEND_SB_RENDERER_COMMON_H
#define SACI_BACKEND_SB_RENDERER_COMMON_H

#include "saci_core/renderer/renderer.h"

#include "saci_util/types.h"

#include "saci_platform/gfx/graphics.h"
#include "saci_platform/gfx/internal/gfx.h"

#include <stddef.h>

#ifndef sb_RENDERER_STRUCT
#  define sb_RENDERER_STRUCT

struct CSaciDefaultVertex {
    SaciVec3 pos;
    SaciColor color;
    SaciUv uv;
};

struct CSaciIndexArrayInfo {
    SaciU64 capacity;
    SaciBool is_fixed_size;
};

struct CSaciVertexArrayInfo {
    SaciU64 capacity;
    SaciBool is_fixed_size;
};

struct CSaciUniformArrayInfo {
    SaciU64 capacity;
    SaciBool is_fixed_size;
};

struct CSaciInstanceBatchConfig {
    struct CSaciIndexArrayInfo index_info;

    struct CSaciVertexArrayInfo vertex_info;

    struct CSaciTransformArrayInfo {
        SaciU64 transform_array_max;
        SaciBool is_fixed_size;
    } transform_info;

    SaciU64 batch_struct_allocation_size;

    SaciU8 capacity;

    SaciU8 in_use;

    SaciBool is_fixed_size;
};

struct CSaciStaticBatchConfig {
    struct CSaciIndexArrayInfo index_info;

    SaciU64 batch_struct_allocation_size;

    SaciU8 capacity;

    SaciU8 in_use;

    SaciBool is_fixed_size;
};

struct CSaciStaticBatchInfo {
    SaciU64 vertex_array_capacity;
    SaciU64 uniform_array_capacity;
    SaciU64 index_array_capacity;
    SaciU64 batch_struct_allocation_size;
    SaciU8 batch_capacity;
    SaciU8 in_use;
};

struct CSaciRendererBound {
    union PSaciTexture texture;
    CSaciIndex* index_array;
    struct PSaciGFXUniformData* uniform_data_array;
    SaciU64 index_count, index_amount;
    SaciU64 uniform_count, uniform_amount;
};

struct CSaciStaticRenderer {
    union PSaciGFXInfo gfx;

    struct CSaciStaticBatchInfo batch_info;
    struct CSaciStaticBatch** batch_ptr_array;

    struct CSaciRendererBound bound;
    void* batch_arena; // TODO
};

struct CSaciInstanceBoundExtra {
    struct PSaciGFXInstanceData* bound_instance_data_array;
    SaciU64 bound_instance_data_array_count;
    SaciU64 bound_instance_data_array_amount;
};

struct CSaciInstanceRenderer {
    union PSaciGFXInfo gfx;
    struct PSaciConfigRenderer* cfg;
    struct CSaciInstanceBoundExtra bound_extra;
    SaciBufferId instance_transform_vbo;
    SaciBufferId instance_color_vbo;

    struct PSaciGFXDrawData* batch_array;

    struct CSaciRendererBound bound;

    struct {
        SaciU8 in_use;
    } batch_info;
};

typedef void (*CSaciRendererDrawFunction)(CSaciRenderer*);
typedef void (*CSaciRendererFreeFunction)(CSaciRenderer*);

struct CSaciRendererInterface {
    void (*new)(struct CSaciRenderer* self, SaciMemPool* mem, struct PSaciConfigRenderer* cfg, union PSaciGFXInfo* info);
    void (*begin)(const struct CSaciRenderer* self);
    void (*bind_texture)(struct CSaciRenderer* self, union PSaciTexture);
    void (*set_uniform)(struct CSaciRenderer* self, SaciS32, const void* const, SaciDataType);
    void (*bind_index_buffer)(struct CSaciRenderer* self, const CSaciIndex* data, const SaciU64 count);
    void (*push_mesh)(struct CSaciRenderer* self, const CSaciVertex* data, const SaciU64 count);
    void (*draw)(const struct CSaciRenderer* self);
    void (*free)(struct CSaciRenderer* self);
    void (*free_opts)(struct CSaciRenderer* self, int);
    SaciS32 (*get_uniform_id)(struct CSaciRenderer* self, const char* const);
};

struct CSaciRenderer {
    enum CSaciRendererType type;
    const struct CSaciRendererInterface* interface;

    union sb_RendererUnion {
        struct CSaciStaticRenderer* static_renderer;
        struct CSaciInstanceRenderer* instance_renderer;
    } rendr;
};

#endif // sb_RENDERER_STRUCT

#define CSACI_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY (16000)

#define CSACI_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY su_CAST_M(SaciU64)(sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 6 / 4)

#define CSACI_RENDERER_DEFAULT_BATCH_INDEX_ELEMENT_SIZE sizeof(su_U32)

#define CSACI_RENDERER_DEFAULT_BATCH_VERTEX_STRUCT_SIZE sizeof(struct sb_Vertex)

#define CSACI_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY

#define CSACI_RENDERER_DEFAULT_UNIFORM_CAPACITY 1024

#define CSACI_RENDERER_DEFAULT_BOUND_UNIFORM_CAPACITY sb_RENDERER_DEFAULT_UNIFORM_CAPACITY

#define CSACI_RENDERER_DEFAULT_BATCH_CAPACITY (10)

#define CSACI_RENDERER_DEFAULT_DEPTH_TEST su_TRUE

#define CSACI_RENDERER_DEFAULT_BLEND_IS_ENABLED su_TRUE

#define CSACI_RENDERER_DEFAULT_BLEND_OPTS 0

#define CSACI_RENDERER_DEFAULT_INSTANCE_CAPACITY 1024

#define CSACI_RENDERER_DEFAULT_BOUND_INSTANCE_CAPACITY sb_RENDERER_DEFAULT_INSTANCE_CAPACITY

#define CSACI_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH (300)

extern void csaci_renderer_instanced_new(struct CSaciRenderer* self, SaciMemPool* mem, struct PSaciConfigRenderer* cfg, union PSaciGFXInfo* info); // Implemented in sb-renderer-instance

SACI_INTERNAL const struct CSaciRendererInterface CSACI_G_RENDERER_INSTANCE_INTERFACE_DEFAULT_INITIALIZER =
    {
        .new = csaci_renderer_instanced_new,
};

/* --- FUNCS USED IN OTHER RENDERER FILES --- */

void csaci_renderer_init_bound(struct CSaciRendererBound* bound_out, const struct PSaciConfigRenderer cfg, SaciMemPool* mem);

void csaci_init_uniforms(struct PSaciConfigRenderer* cfg_out, const union PSaciGFXInfo* gfx_info);

void csaci_init_samplers(struct PSaciConfigRenderer* cfg_out, const union PSaciGFXInfo* gfx_info);

void csaci_init_vertex_layout(struct PSaciConfigRenderer* cfg_out);

void csaci_init_shaders(struct PSaciConfigRenderer* cfg, union PSaciGFXInfo* info_out);

union PSaciGFXUniformValue csaci_renderer_uniform_value_from_type(SaciDataType type, const void* value);

SaciMemPool* csaci_renderer_get_pool_from_cfg(const struct PSaciConfigRenderer* cfg, enum CSaciRendererType type);

void csaci_renderer_cfg_copy_and_cleanup(struct PSaciConfigRenderer* src, struct PSaciConfigRenderer* dest, SaciMemPool* pool);

#endif // SACI_BACKEND_SB_RENDERER_COMMON_H
