#include "saci-backend/sb-renderer.h"

#include "saci-backend/sb-config-manager.h"
#include "saci-backend/sb-gl.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

#include "saci-backend/sb-texture.h"
#include "saci-backend/sb-config.h"

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

#define sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY (16000)

#define sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY su_SCAST_TO_M(su_U64)(sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 6 / 4)

#define sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY

#define sb_RENDERER_DEFAULT_UNIFORM_CAPACITY 1024

#define sb_RENDERER_DEFAULT_BATCH_CAPACITY (10)

#define sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_INDEX_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_VERTEX_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_DEFAULT_UNIFORM_IS_FIXED_SIZE su_TRUE

// TODO needs to be evaluated
#define sb_RENDERER_DEFAULT_INSTANCE_TRANSFORM_ARRAY_SIZE 1024

#define sb_RENDERER_DEFAULT_INSTANCE_TRANSFORM_IS_FIXED_SIZE su_TRUE

#define sb_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH (300)

SA_INTERNAL struct sb_RenderApiFuncs sb_gl_funcs;

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

union sb_UniformValue {
    // Scalar types
    su_U8 u8;
    su_U16 u16;
    su_U32 u32;
    su_U64 u64;
    su_S8 s8;
    su_S16 s16;
    su_S32 s32;
    su_S64 s64;
    su_Bool boolean;

    su_Uv uv;
    su_Vec2 vec2;
    su_Vec3 vec3;
    su_Vec4 vec4;

    // Color
    su_Color color;

    // TODO NEED TO BE ADDED
    su_Mat4 mat4;

    // float mat2[2][2];
    // float mat3[3][3];
    // float mat2x3[2][3];
    // float mat2x4[2][4];
    // float mat3x2[3][2];
    // float mat3x4[3][4];
    // float mat4x2[4][2];
    // float mat4x3[4][3];
};

struct sb_RendererUniformData {
    su_DataType type;
    su_S32 location;
    union sb_UniformValue value;
};

struct sb_InstanceBatch {
    su_TextureId texture;
    su_DArray* model_matrix_array;
    su_DArray* color_array;
    su_DArray* index_array;
    su_DArray* vertex_array;
    su_DArray* uniform_data;
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

struct sb_RendererBoundConfig {
    struct sb_IndexArrayInfo index_info;

    struct sb_UniformArrayInfo uniform_info;
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

struct sb_RendererConfig {
    struct sb_StaticBatchConfig static_batch_cfg;
    struct sb_InstanceBatchConfig instance_batch_cfg;
    struct sb_RendererBoundConfig bound_cfg;
    char* v_shader_code;
    char* f_shader_code;
    char* g_shader_code;
};

struct sb_RendererBound {
    su_TextureId texture;
    su_DArray* index_array;
    su_DArray* uniform_data_array;
};

struct sb_RendererCommon {
    su_ShaderId shader_program;
    su_BufferId ibo, vbo, vao;

    struct sb_RendererBoundInfo bound_info;

    struct sb_RendererBound bound;
};

struct sb_StaticRenderer {
    struct sb_RendererCommon common;

    struct sb_StaticBatchInfo batch_info;
    struct sb_StaticBatch** batch_ptr_array;
    Arena batch_arena;
};

struct sb_InstanceBoundExtra {
    su_DArray* bound_transform_array;
    su_DArray* bound_color_array;
};

struct sb_InstanceRenderer {
    struct sb_RendererCommon common;
    struct sb_InstanceBoundExtra bound_extra;
    su_BufferId instance_transform_vbo;
    su_BufferId instance_color_vbo;

    struct sb_InstanceBatchInfo batch_info;
    struct sb_InstanceBatch** batch_ptr_array;
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

    union {
        struct sb_StaticRenderer* static_renderer;
        struct sb_InstanceRenderer* instance_renderer;
    } rendr;
};

#endif // sb_RENDERER_STRUCT

/* --- Renderer Helper --- */

SA_INTERNAL struct sb_StaticRenderer* sb__renderer_static_new(void);

SA_INTERNAL void sb__renderer_instanced_new(sb_Renderer* rendr);

SA_INTERNAL void sb__renderer_static_fill_default(struct sb_StaticRenderer* rendr);

SA_INTERNAL su_S32 sb__renderer_get_uniform_id(struct sb_Renderer* self,
                                               const char* const uniform_name);

SA_INTERNAL void sb__renderer_bind_index_buffer(struct sb_Renderer* self, const su_DArray* new_indices);

SA_INTERNAL void sb__renderer_instance_fill_default(struct sb_InstanceRenderer* rendr);

SA_INTERNAL void sb__renderer_init_bound(struct sb_RendererCommon* common, const struct sb_RendererConfig cfg);

SA_INTERNAL struct sb_RendererConfig sb__renderer_get_static_config(const char* const path);

// Initializes with default opengl data
SA_INTERNAL void sb__renderer_init_gl(struct sb_RendererCommon* rendr, const struct sb_RendererConfig rendr_cfg);

SA_INTERNAL void sb__renderer_init_static_batch(struct sb_StaticRenderer* rendr, struct sb_RendererConfig cfg);

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr);

SA_INTERNAL union sb_UniformValue sb__renderer_uniform_value_from_type(su_DataType type, const void* value);

SA_INTERNAL void sb__renderer_set_uniform_from_uniform_data(const struct sb_RendererUniformData uniform_data);

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
    if (!sb_gl_funcs.attach_shader) {
        sb_gl_funcs = sb_cfg_manager_get_render_funcs();
    }
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

// TODO there can be a discrepancy in these instance bindings as there can be a
// bigger amount of colors then transform, the renderer will use the smallest
// amount of either but that shouldn't be the case
void sb_renderer_set_instance_colors(struct sb_Renderer* rendr, su_DArray* color_array) {
    su_LOG_ASSERT_MESSAGE_M(rendr->type == sb_RENDERER_INSTANCE, "Trying to set instance colors in non instance renderer");

    su_LOG_DEBUGF_PRINT_M(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Binding %lu colors",
        su_darray_length(color_array));
    su_darray_clear(rendr->rendr.instance_renderer->bound_extra.bound_color_array);
    su_darray_append(rendr->rendr.instance_renderer->bound_extra.bound_color_array, color_array);
}

// TODO, it's working but needs some improvement
void sb_renderer_set_instance_transforms(struct sb_Renderer* rendr, su_DArray* transform_array) {
    su_LOG_ASSERT_MESSAGE_M(rendr->type == sb_RENDERER_INSTANCE, "Trying to set instance transforms in non instance renderer");

    su_LOG_DEBUGF_PRINT_M(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Binding %lu transforms",
        su_darray_length(transform_array));
    su_darray_clear(rendr->rendr.instance_renderer->bound_extra.bound_transform_array);
    su_darray_append(rendr->rendr.instance_renderer->bound_extra.bound_transform_array, transform_array);
}

/* --- Renderer Helper impl --- */

SA_INTERNAL struct sb_StaticRenderer* sb__renderer_static_new(void) {
    struct sb_StaticRenderer* rendr = su_MALLOC_M(sizeof(struct sb_StaticRenderer));
    struct sb_RendererConfig cfg = {0};
    sb__renderer_static_fill_default(rendr);
    sb__renderer_init_gl(&rendr->common, cfg);
    sb__renderer_init_static_batch(rendr, cfg);
    return rendr;
}

SA_INTERNAL void sb__renderer_instanced_new(sb_Renderer* self) {
    struct sb_InstanceRenderer* rendr = su_MALLOC_M(sizeof(struct sb_InstanceRenderer));
    self->rendr.instance_renderer = rendr;
    struct sb_RendererConfig cfg = {0};
    sb__renderer_instance_fill_default(rendr);
    sb__renderer_init_gl(&rendr->common, cfg);
    sb__renderer_init_instance_batch(rendr);
    sb__renderer_init_bound(&rendr->common, cfg);
    // todo send to a separate function
    rendr->instance_transform_vbo = sb_gl_create_vertex_buffer(
        sizeof(su_Mat4) * 1024, NULL, GL_DYNAMIC_DRAW);
    rendr->instance_color_vbo = sb_gl_create_vertex_buffer(
        sizeof(su_Color) * 1024, NULL, GL_DYNAMIC_DRAW);
    sb_gl_bind_vertex_buffer(rendr->instance_transform_vbo);
    for (su_U32 i = 0; i < 4; ++i) {
        sb_gl_set_vertex_attrib_pointer(3 + i, 4, GL_FLOAT, GL_FALSE,
                                        sizeof(su_Mat4), (void*)(sizeof(float) * i * 4));
        sb_gl_enable_vertex_attrib_array(3 + i);
        sb_gl_funcs.vertex_attrib_divisor(3 + i, 1);
    }
    sb_gl_bind_vertex_buffer(rendr->instance_color_vbo);
    sb_gl_set_vertex_attrib_pointer(7, 4, GL_FLOAT, GL_FALSE,
                                    sizeof(su_Vec4),
                                    (void*)(0));
    sb_gl_enable_vertex_attrib_array(7);
    sb_gl_funcs.vertex_attrib_divisor(7, 1);
}

SA_INTERNAL void sb__renderer_init_gl(struct sb_RendererCommon* rendr_common, const struct sb_RendererConfig rendr_cfg) {
#ifndef SACI_RENDERING_DISABLED
    { // Shader init
        su_ShaderId v_shader = sb_shader_compile_shader_vert(sb_INSTANCE_VERT_SHADER);
        su_ShaderId f_shader = sb_shader_compile_shader_frag(sb_FRAG_SHADER);
        su_LOG_ASSERT_MESSAGE_M(v_shader && f_shader, "Shaders could not be initialized");
        rendr_common->shader_program = sb_shader_create_shader_program(v_shader, f_shader);
        su_LOG_ASSERT_MESSAGE_M(rendr_common->shader_program, "Shader program could not be initialized");
    }
    { // Opengl buffers
        rendr_common->vbo = sb_gl_create_vertex_buffer(
            sizeof(struct sb_Vertex) * sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            NULL, GL_DYNAMIC_DRAW);
        su_LOG_ASSERT_MESSAGE_M(rendr_common->vbo, "VBO could not be initialized");
        rendr_common->ibo = sb_gl_create_index_buffer_dynamic(NULL, sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY);
        su_LOG_ASSERT_MESSAGE_M(rendr_common->ibo, "IBO could not be initialized");
        sb_gl_create_vertex_array(1, &rendr_common->vao);
        su_LOG_ASSERT_MESSAGE_M(rendr_common->vao, "VAO could not be initialized");
        // TODO
    }
    { // VertexAttrib init
        sb_gl_bind_vertex_array(rendr_common->vao);

        sb_gl_bind_vertex_buffer(rendr_common->vbo);

        sb_gl_set_vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct sb_Vertex),
                                        su_SCAST_TO_M(void*) offsetof(struct sb_Vertex, pos));
        sb_gl_enable_vertex_attrib_array(0);
        sb_gl_set_vertex_attrib_pointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct sb_Vertex),
                                        su_SCAST_TO_M(void*) offsetof(struct sb_Vertex, color));
        sb_gl_enable_vertex_attrib_array(1);
        sb_gl_set_vertex_attrib_pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct sb_Vertex),
                                        su_SCAST_TO_M(void*) offsetof(struct sb_Vertex, uv));
        sb_gl_enable_vertex_attrib_array(2);
    }
#endif // SACI_RENDERING_DISABLED
}

SA_INTERNAL void sb__renderer_init_static_batch(struct sb_StaticRenderer* rendr, struct sb_RendererConfig cfg) {
    const struct sb_StaticBatchInfo info = rendr->batch_info;
    const struct sb_RendererBoundInfo bound_info = rendr->common.bound_info;
    su_U64 index_size = info.index_array_capacity * sizeof(su_U32);
    su_U64 vertex_size = info.vertex_array_capacity * sizeof(struct sb_Vertex);
    su_U64 uniform_size = bound_info.uniform_array_capacity * sizeof(struct sb_RendererUniformData);
    su_U64 batch_arena_element_size = index_size + vertex_size + uniform_size + sizeof(struct sb_StaticBatch);
    rendr->batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->batch_ptr_array = su_MALLOC_M(sizeof(struct sb_StaticBatch*) * info.batch_capacity);

    ArenaInit(
        &rendr->batch_arena,
        batch_arena_element_size * info.batch_capacity + 1024);

    for (su_U8 i = 0; i < info.batch_capacity; ++i) {
        void* dyn_batch_mem = ArenaPush(&rendr->batch_arena, sizeof(struct sb_StaticBatch));
        void* index_mem = ArenaPush(&rendr->batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->batch_arena, vertex_size);
        void* uniform_mem = ArenaPush(&rendr->batch_arena, uniform_size);

        struct sb_StaticBatch* batch =
            su_SCAST_TO_M(struct sb_StaticBatch*)(dyn_batch_mem);
        batch->texture = sb_TEXTURE_INVALID;
        batch->index_array = su_darray_create_ctx(
            index_mem,
            index_size + su_SIZE_OF_DARRAY,
            info.index_array_capacity,
            sizeof(su_U32),
            su_TRUE);
        batch->vertex_array = su_darray_create_ctx(
            vertex_mem,
            vertex_size + su_SIZE_OF_DARRAY,
            info.vertex_array_capacity,
            sizeof(struct sb_Vertex),
            su_TRUE);
        batch->uniform_data = su_darray_create_ctx(
            uniform_mem,
            uniform_size + su_SIZE_OF_DARRAY,
            bound_info.uniform_array_capacity,
            sizeof(struct sb_RendererUniformData),
            su_TRUE);
        rendr->batch_ptr_array[i] = batch;
    }
}

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr) {
    struct sb_InstanceBatchInfo instance_info = rendr->batch_info;
    struct sb_RendererBoundInfo bound_info = rendr->common.bound_info;
    su_U64 index_size = instance_info.index_array_capacity * sizeof(su_U32);
    su_U64 vertex_size = instance_info.vertex_array_capacity * sizeof(struct sb_Vertex);
    su_U64 transform_size = instance_info.transform_array_capacity * sizeof(su_Mat4);
    su_U64 color_size = instance_info.transform_array_capacity * sizeof(su_Color);
    su_U64 uniform_size = bound_info.uniform_array_capacity * sizeof(struct sb_RendererUniformData);
    su_U64 batch_arena_element_size = index_size + vertex_size + transform_size + color_size + uniform_size + sizeof(struct sb_InstanceBatch);
    rendr->batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->batch_ptr_array = su_CALLOC_M(instance_info.batch_capacity, sizeof(struct sb_InstanceBatch*));
    // TODO
    rendr->bound_extra.bound_color_array = su_darray_create(instance_info.transform_array_capacity, sizeof(su_Color), su_TRUE);
    rendr->bound_extra.bound_transform_array = su_darray_create(instance_info.transform_array_capacity, sizeof(su_Mat4), su_TRUE);

    ArenaInit(
        &rendr->batch_arena,
        batch_arena_element_size * instance_info.batch_capacity + 1024);

    for (su_U8 i = 0; i < instance_info.batch_capacity; ++i) {
        void* instance_batch_mem = ArenaPush(&rendr->batch_arena, sizeof(struct sb_InstanceBatch));
        void* index_mem = ArenaPush(&rendr->batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->batch_arena, vertex_size);
        void* transform_mem = ArenaPush(&rendr->batch_arena, transform_size);
        void* color_mem = ArenaPush(&rendr->batch_arena, color_size);
        void* uniform_mem = ArenaPush(&rendr->batch_arena, uniform_size);

        struct sb_InstanceBatch* instance_batch =
            su_SCAST_TO_M(struct sb_InstanceBatch*)(instance_batch_mem);
        instance_batch->texture = sb_TEXTURE_INVALID;
        instance_batch->index_array = su_darray_create_ctx(
            index_mem,
            index_size + su_SIZE_OF_DARRAY,
            instance_info.index_array_capacity,
            sizeof(su_U32),
            su_TRUE);
        instance_batch->vertex_array = su_darray_create_ctx(
            vertex_mem,
            vertex_size + su_SIZE_OF_DARRAY,
            instance_info.vertex_array_capacity,
            sizeof(struct sb_Vertex),
            su_TRUE);
        instance_batch->model_matrix_array = su_darray_create_ctx(
            transform_mem,
            transform_size + su_SIZE_OF_DARRAY,
            instance_info.transform_array_capacity,
            sizeof(su_Mat4),
            su_TRUE);
        instance_batch->color_array = su_darray_create_ctx(
            color_mem,
            color_size + su_SIZE_OF_DARRAY,
            instance_info.transform_array_capacity,
            sizeof(su_Color),
            su_TRUE);
        instance_batch->uniform_data = su_darray_create_ctx(
            uniform_mem,
            uniform_size + su_SIZE_OF_DARRAY,
            bound_info.uniform_array_capacity,
            sizeof(struct sb_RendererUniformData),
            su_TRUE);
        rendr->batch_ptr_array[i] = instance_batch;
    }
}

SA_INTERNAL void sb__renderer_set_uniform_from_uniform_data(const struct sb_RendererUniformData uniform_data) {
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
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", uniform_data.type);
        return;
    }

    sb_gl_uniform_set_value(uniform_data.location, uniform_data.type, value_ptr);
}

SA_INTERNAL void sb__renderer_static_fill_default(struct sb_StaticRenderer* rendr) {
    rendr->batch_info.uniform_array_capacity = sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->batch_info.batch_capacity = sb_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_info.in_use = 0;
    rendr->batch_info.vertex_array_capacity = sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;

    rendr->common.bound_info.index_array_capacity = sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    rendr->common.bound_info.uniform_array_capacity = sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->common.bound.texture = sb_TEXTURE_INVALID;
}

SA_INTERNAL void sb__renderer_instance_fill_default(struct sb_InstanceRenderer* rendr) {
    rendr->batch_info.uniform_array_capacity = sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->batch_info.index_array_capacity = sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY;
    rendr->batch_info.vertex_array_capacity = sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;
    rendr->batch_info.transform_array_capacity = sb_RENDERER_DEFAULT_INSTANCE_TRANSFORM_ARRAY_SIZE;
    rendr->batch_info.batch_capacity = sb_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_info.in_use = 0;

    rendr->common.bound_info.index_array_capacity = sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    rendr->common.bound_info.uniform_array_capacity = sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->common.bound.texture = sb_TEXTURE_INVALID;
}

SA_INTERNAL su_S32 sb__renderer_get_uniform_id(struct sb_Renderer* self,
                                               const char* const uniform_name) {
    switch (self->type) {
    case sb_RENDERER_STATIC:
        return sb_gl_uniform_location(
            self->rendr.static_renderer->common.shader_program,
            uniform_name);
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        return sb_gl_uniform_location(
            self->rendr.instance_renderer->common.shader_program,
            uniform_name);
    }
    return -1;
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
        su_darray_clear(self->rendr.static_renderer->common.bound.index_array);
        su_darray_append(self->rendr.static_renderer->common.bound.index_array, new_indices);
        break;
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        su_darray_clear(self->rendr.instance_renderer->common.bound.index_array);
        su_darray_append(self->rendr.instance_renderer->common.bound.index_array, new_indices);
        break;
    }
}

SA_INTERNAL void sb__renderer_bind_texture(struct sb_Renderer* self, su_TextureId tex_id) {
    switch (self->type) {
    case sb_RENDERER_STATIC:
        self->rendr.static_renderer->common.bound.texture = tex_id;
        break;
    case sb_RENDERER_DYNAMIC:
        // TODO
    case sb_RENDERER_INSTANCE:
        self->rendr.instance_renderer->common.bound.texture = tex_id;
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
    struct sb_InstanceBatch* batch = rendr->batch_ptr_array[rendr->batch_info.in_use];
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
    su_darray_append(batch->index_array, rendr->common.bound.index_array);
    su_darray_append(batch->uniform_data, rendr->common.bound.uniform_data_array);
    su_darray_append(batch->model_matrix_array, rendr->bound_extra.bound_transform_array);
    su_darray_append(batch->color_array, rendr->bound_extra.bound_color_array);
    rendr->batch_info.in_use++;
}

SA_INTERNAL void sb__renderer_set_uniform(struct sb_Renderer* self,
                                          const su_S32 uniform_id,
                                          const void* const value,
                                          const su_DataType type) {
    // TODO check if is needed
    su_DArray** uniform_data_array;
    switch (self->type) {
    case sb_RENDERER_STATIC:
        uniform_data_array = &self->rendr.static_renderer->common.bound.uniform_data_array;
        break;
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        uniform_data_array = &self->rendr.instance_renderer->common.bound.uniform_data_array;
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
    struct sb_RendererUniformData new_uniform = {0};
    new_uniform.type = type;
    new_uniform.value = sb__renderer_uniform_value_from_type(type, value);
    new_uniform.location = uniform_id;

    struct sb_RendererUniformData* uniform_data = su_MALLOC_M(sizeof(struct sb_RendererUniformData));
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

SA_INTERNAL void sb__renderer_instance_begin(const struct sb_Renderer* self) {
    su_darray_clear(self->rendr.instance_renderer->common.bound.index_array);
    su_darray_clear(self->rendr.instance_renderer->common.bound.uniform_data_array);
    self->rendr.instance_renderer->common.bound.texture = sb_TEXTURE_INVALID;

    self->rendr.instance_renderer->batch_info.in_use = 0;
    for (su_U8 i = 0; i < self->rendr.instance_renderer->batch_info.batch_capacity; ++i) {
        struct sb_InstanceBatch* batch = self->rendr.instance_renderer->batch_ptr_array[i];
        batch->texture = sb_TEXTURE_INVALID;
        su_darray_clear(batch->index_array);
        su_darray_clear(batch->model_matrix_array);
        su_darray_clear(batch->color_array);
        su_darray_clear(batch->vertex_array);
        su_darray_clear(batch->uniform_data);
    }
}

SA_INTERNAL void sb__renderer_init_bound(struct sb_RendererCommon* common, const struct sb_RendererConfig cfg) {
    su_U64 index_array_capacity = cfg.bound_cfg.index_info.capacity
                                      ? cfg.bound_cfg.index_info.capacity
                                      : sb_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    su_U64 uniform_array_capacity = cfg.bound_cfg.uniform_info.capacity
                                        ? cfg.bound_cfg.uniform_info.capacity
                                        : sb_RENDERER_DEFAULT_UNIFORM_CAPACITY;

    common->bound.index_array = su_darray_create(
        index_array_capacity,
        sizeof(su_U32),
        cfg.bound_cfg.index_info.is_fixed_size);

    common->bound.uniform_data_array = su_darray_create(
        uniform_array_capacity,
        sizeof(struct sb_RendererUniformData),
        cfg.bound_cfg.uniform_info.is_fixed_size);
}

SA_INTERNAL struct sb_RendererConfig sb__renderer_get_static_config(const char* const path) {
    struct sb_RendererConfig cfg = {0};
    if (!path) {
        su_LOG_WARN_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                            "Could not open renderer config");
        return cfg;
    }
    sb_ConfigState* state = sb_config_load(path);
    if (!state) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                             "Couldn't load config state");
        return cfg;
    }
    if (!sb_config_load_table(state, "static_renderer")) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                              "Couldn't find 'static' renderer in config: %s", path);
        return cfg;
    }

    {
        su_S8 fixed_size = sb_config_get_bool(state, "bound.index.fixed_size");
        SET_FIXED_SIZE(cfg.bound_cfg.index_info.is_fixed_size, sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE, fixed_size);

        su_U64 capacity = sb_config_get_uint32(state, "bound.index.capacity");
        SET_CAPACITY(cfg.bound_cfg.index_info.capacity, sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY, capacity);
    }

    {
        su_S8 uniform_fixed_size = sb_config_get_bool(state, "bound.uniform.fixed_size");
        SET_FIXED_SIZE(cfg.bound_cfg.uniform_info.is_fixed_size, sb_RENDERER_DEFAULT_UNIFORM_IS_FIXED_SIZE, uniform_fixed_size);

        su_U64 uniform_capacity = sb_config_get_uint32(state, "bound.uniform.capacity");
        SET_CAPACITY(cfg.bound_cfg.uniform_info.capacity, sb_RENDERER_DEFAULT_UNIFORM_CAPACITY, uniform_capacity);
    }

    {
        su_S8 batch_index_fixed_size = sb_config_get_bool(state, "batch.index.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.index_info.is_fixed_size, sb_RENDERER_DEFAULT_INDEX_IS_FIXED_SIZE, batch_index_fixed_size);

        su_U64 batch_index_capacity = sb_config_get_uint32(state, "batch.index.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.index_info.capacity, sb_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY, batch_index_capacity);
    }

    {
        su_S8 batch_vertex_fixed_size = sb_config_get_bool(state, "batch.vertex.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.vertex_info.is_fixed_size, sb_RENDERER_DEFAULT_VERTEX_IS_FIXED_SIZE, batch_vertex_fixed_size);

        su_U64 batch_vertex_capacity = sb_config_get_uint32(state, "batch.vertex.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.vertex_info.capacity, sb_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY, batch_vertex_capacity);
    }

    {
        su_S8 batch_fixed_size = sb_config_get_bool(state, "batch.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.is_fixed_size, sb_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE, batch_fixed_size);

        su_U8 batch_capacity = sb_config_get_uint8(state, "batch.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.capacity, sb_RENDERER_DEFAULT_BATCH_CAPACITY, batch_capacity);
    }

    sb_config_close(state);

    return cfg;
}

SA_INTERNAL void sb__renderer_draw_static_batch(const struct sb_StaticRenderer* rendr) {
    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Flushing %d batches", rendr->batch_info.in_use);

    struct sb_PreviousBatch { // Will get bigger later
        su_TextureId texture;
    } previous_batch = {0};

    for (su_U8 i = 0; i < rendr->batch_info.in_use; ++i) {
        struct sb_StaticBatch* batch = rendr->batch_ptr_array[i];

        sb_gl_funcs.use_program(rendr->common.shader_program);

        sb_gl_funcs.bind_vertex_array(rendr->common.vao);

        sb_gl_funcs.bind_buffer(GL_ARRAY_BUFFER, rendr->common.vbo);
        sb_gl_funcs.buffer_subdata(GL_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(long int)(sizeof(struct sb_Vertex) * su_darray_length(batch->vertex_array)),
                                   su_darray_get_ptr(batch->vertex_array, 0));

        sb_gl_funcs.bind_buffer(GL_ELEMENT_ARRAY_BUFFER, rendr->common.ibo);
        sb_gl_funcs.buffer_subdata(GL_ELEMENT_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(long int)(sizeof(su_U32) * su_darray_length(batch->index_array)),
                                   su_darray_get_ptr(batch->index_array, 0));

        { // Uniforms
            {
                // TODO should be removed
                sb_gl_funcs.enable(GL_DEPTH_TEST);
            }
            for (su_U32 j = 0; j < su_darray_length(rendr->common.bound.uniform_data_array); ++j) {
                struct sb_RendererUniformData uniform_data = {0};
                su_darray_get(batch->uniform_data, j, &uniform_data);
                sb__renderer_set_uniform_from_uniform_data(uniform_data);
            }
        }

        if (batch->texture != previous_batch.texture) {
            sb_gl_funcs.uniform1i(SC_U_USE_TEXTURE_LOC, su_TRUE);
            sb_gl_funcs.active_texture(GL_TEXTURE0);
            sb_gl_funcs.bind_texture(GL_TEXTURE_2D, batch->texture);
            previous_batch.texture = batch->texture;
        }

        sb_gl_funcs.draw_elements(GL_TRIANGLES, su_SCAST_TO_M(int)(su_darray_length(batch->index_array)), GL_UNSIGNED_INT, 0);
        sb_gl_funcs.bind_buffer(GL_UNIFORM_BUFFER, 0);
    }
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

        sb_gl_funcs.use_program(rendr->common.shader_program);

        sb_gl_funcs.bind_vertex_array(rendr->common.vao);

        sb_gl_funcs.bind_buffer(GL_ARRAY_BUFFER, rendr->common.vbo);
        sb_gl_funcs.buffer_subdata(GL_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(long int)(sizeof(struct sb_Vertex) * su_darray_length(batch->vertex_array)),
                                   su_darray_get_ptr(batch->vertex_array, 0));

        sb_gl_funcs.bind_buffer(GL_ELEMENT_ARRAY_BUFFER, rendr->common.ibo);
        sb_gl_funcs.buffer_subdata(GL_ELEMENT_ARRAY_BUFFER, 0,
                                   su_SCAST_TO_M(long int)(sizeof(su_U32) * su_darray_length(batch->index_array)),
                                   su_darray_get_ptr(batch->index_array, 0));

        // TODO this should be checked before, since if it is empty there is no reason to drawing
        if (!su_darray_is_empty(batch->model_matrix_array)) {
            sb_gl_funcs.bind_buffer(GL_ARRAY_BUFFER, rendr->instance_transform_vbo);
            sb_gl_funcs.buffer_subdata(GL_ARRAY_BUFFER, 0,
                                       su_SCAST_TO_M(long int)(sizeof(su_Mat4) * su_darray_length(batch->model_matrix_array)),
                                       su_darray_get_ptr(batch->model_matrix_array, 0));
        }
        if (!su_darray_is_empty(batch->color_array)) {
            sb_gl_funcs.bind_buffer(GL_ARRAY_BUFFER, rendr->instance_color_vbo);
            sb_gl_funcs.buffer_subdata(GL_ARRAY_BUFFER, 0,
                                       su_SCAST_TO_M(long int)(sizeof(su_Color) * su_darray_length(batch->color_array)),
                                       su_darray_get_ptr(batch->color_array, 0));
        }

        { // Uniforms
            {
                // TODO should be removed
                sb_gl_funcs.enable(GL_DEPTH_TEST);
            }
            for (su_U32 j = 0; j < su_darray_length(rendr->common.bound.uniform_data_array); ++j) {
                struct sb_RendererUniformData uniform_data = {0};
                su_darray_get(batch->uniform_data, j, &uniform_data);
                sb__renderer_set_uniform_from_uniform_data(uniform_data);
            }
        }

        if (batch->texture != previous_batch.texture && batch->texture != sb_TEXTURE_INVALID) {
            sb_gl_funcs.uniform1i(SC_U_USE_TEXTURE_LOC, su_TRUE);
            sb_gl_funcs.active_texture(GL_TEXTURE0);
            sb_gl_funcs.bind_texture(GL_TEXTURE_2D, batch->texture);
            previous_batch.texture = batch->texture;
        }

        sb_gl_funcs.draw_elements_instanced(GL_TRIANGLES,
                                            su_SCAST_TO_M(int)(su_darray_length(batch->index_array)),
                                            GL_UNSIGNED_INT,
                                            0,
                                            su_SCAST_TO_M(int)(su_darray_length(batch->model_matrix_array))); // instance count
        sb_gl_funcs.bind_buffer(GL_UNIFORM_BUFFER, 0);
    }
}

SA_INTERNAL union sb_UniformValue sb__renderer_uniform_value_from_type(su_DataType type, const void* value) {
    union sb_UniformValue result = {0};

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
