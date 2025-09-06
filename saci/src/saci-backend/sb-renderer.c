#include "saci-backend/sb-renderer.h"

#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types.h"

#include "saci-backend/sb-config-manager.h"
#include "saci-backend/sb-gfx.h"
#include "saci-backend/sb-texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_ASSERT(x) su_LOG_ASSERT_MESSAGE_M(x, "Error in arena function")
#define ARENA_FREE(x) su_FREE_M(x)
#define ARENA_MALLOC(x) su_MALLOC_M(x)
#define ARENA_ALLOCATOR_IMPL
#define ARENA_DEBUG
#include "arena/arena.h"

/* === Renderer === */

// Used in config functions, checks if value is -1 if is uses default
#define SET_FIXED_SIZE(var, default_value, getter) \
    var = (getter == -1) ? default_value : getter;

// Used in config functions, checks if value is 0 if is uses default
#define SET_CAPACITY(var, default_value, getter) \
    var = (getter == 0) ? default_value : getter;

// TODO to be used
#ifndef SC_TEXTURE_ARRAY_SIZE
#  ifdef _WIN32
// Windows
#    define SC_TEXTURE_ARRAY_SIZE 16
#  elif defined(__APPLE__) && defined(__MACH__)
// macOS
#    define SC_TEXTURE_ARRAY_SIZE 32
#  elif defined(__linux__)
// Linux
#    define SC_TEXTURE_ARRAY_SIZE 16
#  elif defined(__ANDROID__)
// Android
#    define SC_TEXTURE_ARRAY_SIZE 8
#  else
// Default case
#    define SC_TEXTURE_ARRAY_SIZE 8
#  endif // _WIN32
#endif   // SC_TEXTURE_ARRAY_SIZE

#ifndef SC_RENDERER_UNIFORMS
#  define SC_RENDERER_UNIFORMS

// TODO test these
// These might change and might not be used later...
#  define SC_U_MODEL_MATRIX_LOC 0
#  define SC_U_VIEW_MATRIX_LOC 1
#  define SC_U_PROJECTION_MATRIX_LOC 2
#  define SC_U_FLAGS 3
#  define SC_U_LIGHTING 4
#  define SC_U_TEXTURE_LOC 5
#  define SC_U_USE_TEXTURE_LOC 6

#endif // SC_RENDERER_UNIFORMS

SA_INTERNAL const char* const sb_INSTANCE_VERT_SHADER =
    // TODO Evaluate if there should be multiple shaders, one for 2d, 3d
    // instancing, 3d dynamic mesh etc. Saci-Core needs to be
    // performatic but also "just work", so the user shouldn't need to
    // change it unless they want that '1%' boost
    "#version 330 core\n"

    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec4 a_color;\n"
    "layout (location = 2) in vec2 a_texcoord;\n"
    "layout (location = 3) in mat4 i_model_matrix;\n"
    "layout (location = 7) in vec4 i_color;\n"

    "uniform mat4 u_model_matrix;\n"
    "uniform mat4 u_view_matrix;\n"
    "uniform mat4 u_projection_matrix;\n"
    "uniform int u_flags;\n"
    "uniform vec4 u_lighting;\n"

    "out vec4 v_color;\n"
    "out vec2 v_texcoord;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * i_model_matrix * vec4(a_pos, 1.0);\n"
    "   v_color = a_color + i_color;\n" // TODO later there might be some flag or way to not only add but multiply or divide or whatever
    "   v_texcoord = a_texcoord;\n"
    "}\n\0";

SA_INTERNAL const char* const sb_FRAG_SHADER =
    "#version 330 core\n"

    "in vec4 v_color;\n"
    "in vec2 v_texcoord;\n"

    "uniform sampler2D u_texture;\n"
    "uniform bool u_use_texture;\n"

    "out vec4 frag_color;\n"

    "void main()\n"
    "{\n"
    "   if (u_use_texture) {\n"
    "       vec4 texcolor = texture(u_texture, v_texcoord);\n"
    "       frag_color = texcolor * v_color;\n"
    "   } else {\n"
    "       frag_color = v_color;\n"
    "   }\n"
    "}\n\0";

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

/* --- Renderer Helper --- */

SA_INTERNAL struct sb_StaticRenderer* sb__renderer_static_new(void);

SA_INTERNAL void sb__renderer_instanced_new(sb_Renderer* rendr);

SA_INTERNAL void sb__renderer_static_fill_default(struct sb_StaticRenderer* rendr);

SA_INTERNAL su_S32 sb__renderer_get_uniform_id(struct sb_Renderer* self,
                                               const char* const uniform_name);

SA_INTERNAL void sb__renderer_bind_index_buffer(struct sb_Renderer* self, const su_DArray* new_indices);

SA_INTERNAL void sb__renderer_instance_fill_default(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info);

SA_INTERNAL void sb__renderer_init_bound(struct sb_RendererBound* bound_out, struct sb_RendererBoundInfo* bound_cfg_out, const struct sb_RendererConfig cfg);

SA_INTERNAL struct sb_RendererConfig sb__renderer_get_static_config(const char* const path);

// Initializes with default opengl data
SA_INTERNAL void sb__renderer_init_gl(union sb_GFXInfo* info_out, const struct sb_RendererConfig rendr_cfg);

SA_INTERNAL void sb__renderer_init_static_batch(struct sb_StaticRenderer* rendr, struct sb_RendererConfig cfg);

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr);

SA_INTERNAL union sb_GFXUniformValue sb__renderer_uniform_value_from_type(su_DataType type, const void* value);

SA_INTERNAL void sb__renderer_set_uniform_from_uniform_data(const struct sb_GFXUniformData uniform_data);

SA_INTERNAL void sb__renderer_draw_instance_batch(const struct sb_Renderer* rendr);

SA_INTERNAL void sb__renderer_bind_texture(struct sb_Renderer* self, su_TextureId tex_id);

SA_INTERNAL void sb__renderer_push_mesh_instanced(struct sb_Renderer* self,
                                                  const su_DArray* pos_array,
                                                  const su_DArray* uv_array,
                                                  const su_DArray* color_array);

SA_INTERNAL void sb__renderer_set_uniform(struct sb_Renderer* self,
                                          const su_S32 uniform_id,
                                          const void* const value,
                                          const su_DataType type);

SA_INTERNAL void sb__renderer_instance_begin(const struct sb_Renderer* self);

SA_INTERNAL const struct sb_RendererInterface sb_INSTANCE_RENDERER_DEFAULT_INTERFACE =
    {
        .new = sb__renderer_instanced_new,
        .begin = sb__renderer_instance_begin,
        .bind_texture = sb__renderer_bind_texture,
        .get_uniform_id = sb__renderer_get_uniform_id,
        .set_uniform = sb__renderer_set_uniform,
        .bind_index_buffer = sb__renderer_bind_index_buffer,
        .push_mesh = sb__renderer_push_mesh_instanced,
        .draw = sb__renderer_draw_instance_batch,
        .free = NULL,
        .free_opts = NULL,
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
        rendr->interface = &sb_INSTANCE_RENDERER_DEFAULT_INTERFACE;
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

/* --- Renderer specific impl --- */

void sb_renderer_set_instance_data(struct sb_Renderer* rendr, su_DArray* transform_array, su_DArray* color_array) {
    su_LOG_ASSERT_MESSAGE_M(rendr->type == sb_RENDERER_INSTANCE, "Trying to set instance transforms in non instance renderer");
    su_LOG_ASSERT_MESSAGE_M(su_darray_length(transform_array) == su_darray_length(color_array), "Seting instances with different sizes");

    su_LOG_DEBUGF_PRINT_M(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Binding %lu transforms and colors",
        su_darray_length(transform_array));
    su_DArray* data_array = su_darray_create(su_darray_length(transform_array), sizeof(struct sb_InstanceData), su_TRUE);
    for (su_U64 i = 0; i < su_darray_length(transform_array); ++i) {
        struct sb_InstanceData data = {0};
        su_darray_get(transform_array, i, &data.model);
        su_darray_get(color_array, i, &data.color);
        su_darray_push(data_array, &data);
    }

    su_darray_clear(rendr->rendr.instance_renderer->bound_extra.bound_instance_array);
    su_darray_append(rendr->rendr.instance_renderer->bound_extra.bound_instance_array, data_array);
}

/* --- Renderer Helper impl --- */

SA_INTERNAL void sb__renderer_instanced_new(sb_Renderer* self) {
    struct sb_InstanceRenderer* rendr = su_MALLOC_M(sizeof(struct sb_InstanceRenderer));
    self->rendr.instance_renderer = rendr;
    struct sb_RendererConfig cfg = sb_CFG_DEFAULT_INSTANCE;
    sb_cfg_manager_get_renderer(su_string_create("instance", su_REALLOCATION_KIND_FIXED_SIZE), &cfg);
    sb_gfx_init_shader(&rendr->gfx, cfg);
    sb__renderer_instance_fill_default(&cfg, &rendr->gfx);
    sb_gfx_create(&rendr->gfx, cfg);
    sb__renderer_init_instance_batch(rendr);
    sb__renderer_init_bound(&rendr->bound, &rendr->bound_info, cfg);
}

SA_INTERNAL su_S32 sb__renderer_get_uniform_id(struct sb_Renderer* self,
                                               const char* const uniform_name) {
    switch (self->type) {
    case sb_RENDERER_STATIC:
        return sb_gfx_get_uniform_loc_cstr(
            &self->rendr.static_renderer->gfx,
            uniform_name);
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        return sb_gfx_get_uniform_loc_cstr(
            &self->rendr.instance_renderer->gfx,
            uniform_name);
    }
    return -1;
}

SA_INTERNAL void sb__renderer_init_bound(struct sb_RendererBound* bound_out, struct sb_RendererBoundInfo* bound_cfg_out, const struct sb_RendererConfig cfg) {
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

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr) {
    struct sb_InstanceBatchInfo instance_info = rendr->batch_info;
    struct sb_RendererBoundInfo bound_info = rendr->bound_info;

    su_U64 index_size = instance_info.index_array_capacity * sizeof(su_U32);
    su_U64 vertex_size = instance_info.vertex_array_capacity * sizeof(struct sb_Vertex);
    su_U64 instance_size = instance_info.transform_array_capacity * sizeof(struct sb_InstanceData);
    su_U64 uniform_size = bound_info.uniform_array_capacity * sizeof(struct sb_GFXUniformData);

    su_U64 batch_arena_element_size =
        index_size + vertex_size + instance_size + uniform_size +
        sizeof(struct sb_GFXDrawData);

    rendr->batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->batch_ptr_array = su_CALLOC_M(instance_info.batch_capacity, sizeof(struct sb_GFXDrawData*));

    ArenaInit(
        &rendr->batch_arena,
        batch_arena_element_size * instance_info.batch_capacity + 1024);

    for (su_U8 i = 0; i < instance_info.batch_capacity; ++i) {
        void* drawdata_mem = ArenaPush(&rendr->batch_arena, sizeof(struct sb_GFXDrawData));
        void* index_mem = ArenaPush(&rendr->batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->batch_arena, vertex_size);
        void* instance_mem = ArenaPush(&rendr->batch_arena, instance_size);
        void* uniform_mem = ArenaPush(&rendr->batch_arena, uniform_size);

        struct sb_GFXDrawData* drawdata = su_SCAST_TO_M(struct sb_GFXDrawData*)(drawdata_mem);

        drawdata->vertex_array = su_darray_create_ctx(
            vertex_mem,
            vertex_size + su_SIZE_OF_DARRAY,
            instance_info.vertex_array_capacity,
            sizeof(struct sb_Vertex),
            su_TRUE);
        drawdata->vertex_struct_size = sizeof(struct sb_Vertex);

        drawdata->index_array = su_darray_create_ctx(
            index_mem,
            index_size + su_SIZE_OF_DARRAY,
            instance_info.index_array_capacity,
            sizeof(su_U32),
            su_TRUE);
        drawdata->index_struct_size = sizeof(su_U32);

        drawdata->instance_array_array = su_darray_create_ctx(
            instance_mem,
            instance_size + su_SIZE_OF_DARRAY,
            instance_info.transform_array_capacity,
            sizeof(struct sb_InstanceData),
            su_TRUE);
        drawdata->instance_location_array = NULL; // optional
        drawdata->instance_struct_size = sizeof(struct sb_InstanceData);

        drawdata->uniform_array_array = su_darray_create_ctx(
            uniform_mem,
            uniform_size + su_SIZE_OF_DARRAY,
            bound_info.uniform_array_capacity,
            sizeof(struct sb_GFXUniformData),
            su_TRUE);
        drawdata->uniform_location_array = NULL;

        for (su_U32 t = 0; t < SACI_MAX_TEXTURES; ++t) {
            // TODO when changing renderer api
            drawdata->texture_array[t].gl_texture.texture = sb_TEXTURE_INVALID;
            drawdata->texture_array_loc[t] = 0;
        }

        rendr->batch_ptr_array[i] = drawdata;
    }
}

SA_INTERNAL union sb_GFXUniformValue sb__renderer_uniform_value_from_type(su_DataType type, const void* value) {
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

SA_INTERNAL void sb__init_uniforms(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
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

SA_INTERNAL void sb__init_samplers(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
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

SA_INTERNAL void sb__init_vertex_layout(struct sb_RendererConfig* cfg_out) {
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

SA_INTERNAL void sb__init_shaders(struct sb_RendererConfig* cfg_out) {
    if (!cfg_out->shaders.vert) {
        cfg_out->shaders.vert = su_string_create(sb_INSTANCE_VERT_SHADER, su_REALLOCATION_KIND_FIXED_SIZE);
    }
    if (!cfg_out->shaders.frag) {
        cfg_out->shaders.frag = su_string_create(sb_FRAG_SHADER, su_REALLOCATION_KIND_FIXED_SIZE);
    }
}

SA_INTERNAL void sb__init_instance_buffers(struct sb_RendererConfig* cfg_out) {
    if (!cfg_out->batch.instance_cfg.buffer_array) {
        cfg_out->batch.instance_cfg.buffer_array =
            su_darray_create(1, sizeof(struct sb_RendererCfgInstanceBuffer), su_TRUE);

        struct sb_RendererCfgInstanceBufferLayout model_mat = {
            .name = su_string_create("i_model_matrix", su_REALLOCATION_KIND_FIXED_SIZE),
            .type = su_TYPE_MAT4,
            .offset = 0,
            .location = 3,
        };
        struct sb_RendererCfgInstanceBufferLayout color = {
            .name = su_string_create("i_color", su_REALLOCATION_KIND_FIXED_SIZE),
            .type = su_TYPE_COLOR,
            .offset = 64,
            .location = 7,
        };

        struct sb_RendererCfgInstanceBuffer buffer = {
            .name = su_string_create("default string buffer", su_REALLOCATION_KIND_FIXED_SIZE),
            .layout_array = su_darray_create(2, sizeof(struct sb_RendererCfgInstanceBufferLayout), su_TRUE),
        };

        su_darray_push(buffer.layout_array, &model_mat);
        su_darray_push(buffer.layout_array, &color);
        su_darray_push(cfg_out->batch.instance_cfg.buffer_array, &buffer);
    }
}

SA_INTERNAL void sb__renderer_instance_fill_default(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
    sb__init_uniforms(cfg_out, gfx_info);
    sb__init_samplers(cfg_out, gfx_info);
    sb__init_vertex_layout(cfg_out);
    sb__init_shaders(cfg_out);
    sb__init_instance_buffers(cfg_out);
}

SA_INTERNAL void sb__renderer_bind_texture(struct sb_Renderer* self, su_TextureId tex_id) {
    switch (self->type) {
    case sb_RENDERER_STATIC:
        self->rendr.static_renderer->bound.texture = tex_id;
        break;
    case sb_RENDERER_DYNAMIC:
        // TODO
    case sb_RENDERER_INSTANCE:
        self->rendr.instance_renderer->bound.texture = tex_id;
        break;
    }
}

SA_INTERNAL void sb__renderer_instance_begin(const struct sb_Renderer* self) {
    su_darray_clear(self->rendr.instance_renderer->bound.index_array);
    su_darray_clear(self->rendr.instance_renderer->bound.uniform_data_array);
    self->rendr.instance_renderer->bound.texture = sb_TEXTURE_INVALID;

    self->rendr.instance_renderer->batch_info.in_use = 0;
    for (su_U8 i = 0; i < self->rendr.instance_renderer->batch_info.batch_capacity; ++i) {
        struct sb_GFXDrawData* batch = self->rendr.instance_renderer->batch_ptr_array[i];
        for (int j = 0; j < SACI_MAX_TEXTURES; ++j) {
            batch->texture_array[j].gl_texture.texture = sb_TEXTURE_INVALID;
            batch->texture_array_loc[j] = 0;
        }
        su_darray_clear(batch->vertex_array);
        su_darray_clear(batch->index_array);
        su_darray_clear(batch->instance_array_array);
        su_darray_clear(batch->instance_location_array);
        su_darray_clear(batch->uniform_array_array);
        su_darray_clear(batch->uniform_location_array);
    }
}

SA_INTERNAL void sb__renderer_set_uniform(struct sb_Renderer* self,
                                          const su_S32 uniform_id,
                                          const void* const value,
                                          const su_DataType type) {
    // TODO check if is needed
    su_DArray** uniform_data_array;
    switch (self->type) {
    case sb_RENDERER_STATIC:
        uniform_data_array = &self->rendr.static_renderer->bound.uniform_data_array;
        break;
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        uniform_data_array = &self->rendr.instance_renderer->bound.uniform_data_array;
        break;
    }

    if (uniform_id < 0 || uniform_id >= su_TYPE_MAX ||
        uniform_id == su_TYPE_BUFFERID || uniform_id == su_TYPE_SHADERID ||
        uniform_id == su_TYPE_TEXTUREID) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid ID");
        return;
    }
    if (su_SCAST_TO_M(int)(type) == 0 || su_SCAST_TO_M(int)(type) > 26) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid type");
        return;
    }
    struct sb_GFXUniformData new_uniform = {0};
    new_uniform.type = type;
    new_uniform.value = sb__renderer_uniform_value_from_type(type, value);
    new_uniform.location = uniform_id;

    struct sb_GFXUniformData* uniform_data = su_MALLOC_M(sizeof(struct sb_GFXUniformData));
    for (su_U64 i = 0; i < su_darray_length(*uniform_data_array); ++i) {
        su_darray_get(*uniform_data_array, i, uniform_data);
        if (uniform_data->location == uniform_id) {
            su_FREE_M(uniform_data);
            su_darray_set(*uniform_data_array, i, &new_uniform);
            return;
        }
    }
    if (!su_darray_push(*uniform_data_array, &new_uniform)) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                              "Could not set uniform in renderer");
    }
    su_FREE_M(uniform_data);
}

SA_INTERNAL void sb__renderer_bind_index_buffer(struct sb_Renderer* self, const su_DArray* new_indices) {
    if (!new_indices) {
        su_LOG_WARN_PRINT_M(su_LOG_SEVERITY_MEDIUM,
                            su_LOG_CONTEXT_RENDERER,
                            "Indices are NULL and cannot be bound");
        return;
    }
    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, su_LOG_CONTEXT_RENDERER,
                          "Bound %lu indices", su_darray_length(new_indices));

    switch (self->type) {
    case sb_RENDERER_STATIC:
        su_darray_clear(self->rendr.static_renderer->bound.index_array);
        su_darray_append(self->rendr.static_renderer->bound.index_array, new_indices);
        break;
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        su_darray_clear(self->rendr.instance_renderer->bound.index_array);
        su_darray_append(self->rendr.instance_renderer->bound.index_array, new_indices);
        break;
    }
}

SA_INTERNAL void sb__renderer_push_mesh_instanced(struct sb_Renderer* self,
                                                  const su_DArray* pos_array,
                                                  const su_DArray* uv_array,
                                                  const su_DArray* color_array) {
    struct sb_InstanceRenderer* rendr = self->rendr.instance_renderer;
    // s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);
    // TODO validate instance_transform

    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu vertices to instance batch",
                          su_darray_length(pos_array));
    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu indices to instance batch",
                          su_darray_length(rendr->common.bound.index_array));
    struct sb_GFXDrawData* batch = rendr->batch_ptr_array[rendr->batch_info.in_use];
    su_Color default_color = {0, 0, 0, 0};
    su_Uv default_uv = {0, 0};
    for (su_U64 i = 0; i < su_darray_length(pos_array); ++i) {
        su_Vec3 pos;
        su_Color color = default_color;
        su_Uv uv = default_uv;
        su_darray_get(pos_array, i, &pos);
        if (uv_array) {
            su_darray_get(uv_array, i, &uv);
        }
        if (color_array) {
            su_darray_get(color_array, i, &color);
        }
        struct sb_Vertex vertex = (struct sb_Vertex){pos, color, uv};
        if (!su_darray_push(batch->vertex_array, &vertex)) {
            su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER, "Could not push vertex to instance batch");
        }
    }
    su_darray_append(batch->index_array, rendr->bound.index_array);
    su_darray_append(batch->uniform_array_array, rendr->bound.uniform_data_array);
    su_darray_append(batch->instance_array_array, rendr->bound_extra.bound_instance_array);
    rendr->batch_info.in_use++;
}

SA_INTERNAL void sb__renderer_draw_instance_batch(const struct sb_Renderer* self) {
    const struct sb_InstanceRenderer* rendr = self->rendr.instance_renderer;
    su_LOG_DEBUGF_PRINT_M(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Flushing %d instance batches",
        rendr->batch_info.in_use);

    struct sb_PreviousBatch { // Will get bigger later
        su_TextureId texture;
    } previous_batch = {0};

    for (su_U8 i = 0; i < rendr->batch_info.in_use; ++i) {
        struct sb_InstanceBatch* batch = rendr->batch_ptr_array[i];
        // sb_gfx_draw(&self->rendr.instance_renderer->gfx);
    }
}
