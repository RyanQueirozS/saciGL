#include <glad/glad.h>

#include "saci-backend/sc-renderer.h"

#include "saci-backend/sc-gl.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

#include "saci-backend/sc-texture.h"
#include "saci-backend/sc-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_ASSERT(x) su_Log_Assert_Message_m(x, "Error in arena function")
#define ARENA_FREE(x) su_Free_m(x)
#define ARENA_MALLOC(x) su_Malloc_m(x)
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

#define SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY (16000)

#define SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY su_Scast_To_m(su_u64)(SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 6 / 4)

#define SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY

#define SC_RENDERER_DEFAULT_UNIFORM_CAPACITY 1024

#define SC_RENDERER_DEFAULT_BATCH_CAPACITY (10)

#define SC_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE su_TRUE

#define SC_RENDERER_DEFAULT_INDEX_IS_FIXED_SIZE su_TRUE

#define SC_RENDERER_DEFAULT_VERTEX_IS_FIXED_SIZE su_TRUE

#define SC_RENDERER_DEFAULT_UNIFORM_IS_FIXED_SIZE su_TRUE

// TODO needs to be evaluated
#define SC_RENDERER_DEFAULT_INSTANCE_TRANSFORM_ARRAY_SIZE 1024

#define SC_RENDERER_DEFAULT_INSTANCE_TRANSFORM_IS_FIXED_SIZE su_TRUE

#define SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH (300)

SA_INTERNAL const char* const sc_INSTANCE_VERT_SHADER =
    // TODO Evaluate if there should be multiple shaders, one for 2d, 3d
    // instancing, 3d dynamic mesh etc. Saci-Core needs to be
    // performatic but also "just work", so the user shouldn't need to
    // change it unless they want that '1%' boost
    "#version 330 core\n"

    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec4 a_color;\n"
    "layout (location = 2) in vec2 a_texcoord;\n"
    "layout (location = 3) in mat4 a_model_matrix;\n"

    "uniform mat4 u_model_matrix;\n"
    "uniform mat4 u_view_matrix;\n"
    "uniform mat4 u_projection_matrix;\n"
    "uniform int u_flags;\n"
    "uniform vec4 u_lighting;\n"

    "out vec4 v_color;\n"
    "out vec2 v_texcoord;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * a_model_matrix * vec4(a_pos, 1.0);\n"
    "   v_color = a_color;\n"
    "   v_texcoord = a_texcoord;\n"
    "}\n\0";

SA_INTERNAL const char* const sc_FRAG_SHADER =
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

#ifndef SC_RENDERER_STRUCT
#  define SC_RENDERER_STRUCT

struct sc_vertex {
    su_vec3 pos;
    su_color color;
    su_uv uv;
};

union sc_uniformValue {
    // Scalar types
    su_u8 u8;
    su_u16 u16;
    su_u32 u32;
    su_u64 u64;
    su_s8 s8;
    su_s16 s16;
    su_s32 s32;
    su_s64 s64;
    su_bool boolean;

    su_uv uv;
    su_vec2 vec2;
    su_vec3 vec3;
    su_vec4 vec4;

    // Color
    su_color color;

    // TODO NEED TO BE ADDED
    su_mat4 mat4;

    // float mat2[2][2];
    // float mat3[3][3];
    // float mat2x3[2][3];
    // float mat2x4[2][4];
    // float mat3x2[3][2];
    // float mat3x4[3][4];
    // float mat4x2[4][2];
    // float mat4x3[4][3];
};

struct sc_rendererUniformData {
    su_dataType type;
    su_s32 location;
    union sc_uniformValue value;
};

struct sc_instanceBatch {
    su_textureId texture;
    su_dArray* model_matrix_array;
    su_dArray* index_array;
    su_dArray* vertex_array;
    su_dArray* uniform_data;
};

struct sc_staticBatch {
    su_textureId texture;
    su_dArray* index_array;
    su_dArray* vertex_array;
    su_dArray* uniform_data;
};

struct sc_indexArrayInfo {
    su_u64 capacity;
    su_bool is_fixed_size;
};

struct sc_vertexArrayInfo {
    su_u64 capacity;
    su_bool is_fixed_size;
};

struct sc_uniformArrayInfo {
    su_u64 capacity;
    su_bool is_fixed_size;
};

struct sc_rendererBoundConfig {
    struct sc_indexArrayInfo index_info;

    struct sc_uniformArrayInfo uniform_info;
};

struct sc_rendererBoundInfo {
    su_u64 index_array_capacity;

    su_u64 uniform_array_capacity;
};

struct sc_instanceBatchInfo {
    su_u64 vertex_array_capacity;
    su_u64 index_array_capacity;
    su_u64 uniform_array_capacity;
    su_u64 transform_array_capacity;

    su_u64 batch_struct_allocation_size;
    su_u8 batch_capacity;
    su_u8 in_use;
};

struct sc_instanceBatchConfig {
    struct sc_indexArrayInfo index_info;

    struct sc_vertexArrayInfo vertex_info;

    struct sc_transformArrayInfo {
        su_u64 transform_array_max;
        su_bool is_fixed_size;
    } transform_info;

    su_u64 batch_struct_allocation_size;

    su_u8 capacity;

    su_u8 in_use;

    su_bool is_fixed_size;
};

struct sc_staticBatchConfig {
    struct sc_indexArrayInfo index_info;

    su_u64 batch_struct_allocation_size;

    su_u8 capacity;

    su_u8 in_use;

    su_bool is_fixed_size;
};

struct sc_staticBatchInfo {
    su_u64 vertex_array_capacity;
    su_u64 uniform_array_capacity;
    su_u64 index_array_capacity;
    su_u64 batch_struct_allocation_size;
    su_u8 batch_capacity;
    su_u8 in_use;
};

struct sc_rendererConfig {
    struct sc_staticBatchConfig static_batch_cfg;
    struct sc_instanceBatchConfig instance_batch_cfg;
    struct sc_rendererBoundConfig bound_cfg;
    char* v_shader_code;
    char* f_shader_code;
    char* g_shader_code;
};

struct sc_rendererBound {
    su_textureId texture;
    su_dArray* index_array;
    su_dArray* uniform_data_array;
};

struct sc_rendererCommon {
    su_shaderId shader_program;
    su_bufferId ibo, vbo, vao;

    struct sc_rendererBoundInfo bound_info;

    struct sc_rendererBound bound;
};

struct sc_staticRenderer {
    struct sc_rendererCommon common;

    struct sc_staticBatchInfo batch_info;
    struct sc_staticBatch** batch_ptr_array;
    Arena batch_arena;
};

struct sc_instanceRenderer {
    struct sc_rendererCommon common;
    su_dArray* bound_transform_array;
    su_bufferId instance_vbo; // used for instancing

    struct sc_instanceBatchInfo batch_info;
    struct sc_instanceBatch** batch_ptr_array;
    Arena batch_arena;
};

typedef void (*sc_rendererDrawFunction)(sc_renderer*);
typedef void (*sc_freeFunction)(sc_renderer*);

struct sc_rendererInterface {
    void (*new)(struct sc_renderer* self);
    void (*begin)(const struct sc_renderer* self);
    void (*bind_texture)(struct sc_renderer* self, su_textureId);
    void (*set_uniform)(struct sc_renderer* self, su_s32, const void* const, su_dataType);
    void (*bind_index_buffer)(struct sc_renderer* self, const su_dArray*);
    void (*push_mesh)(struct sc_renderer* self, const su_dArray* vertex_array, const su_dArray* uv_array, const su_dArray* color_array);
    void (*draw)(const struct sc_renderer* self);
    void (*free)(struct sc_renderer* self);
    void (*free_opts)(struct sc_renderer* self, int);
    su_s32 (*get_uniform_id)(struct sc_renderer* self, const char* const);
};

struct sc_renderer {
    enum sc_rendererType type;
    const struct sc_rendererInterface* interface;

    union {
        struct sc_staticRenderer* static_renderer;
        struct sc_instanceRenderer* instance_renderer;
    } rendr;
};

#endif // SC_RENDERER_STRUCT

/* --- Renderer Helper --- */

SA_INTERNAL struct sc_staticRenderer* s_Renderer_Static_New(void);

SA_INTERNAL void s_Renderer_Instanced_New(sc_renderer* rendr);

SA_INTERNAL void s_Renderer_Static_Fill_Default(struct sc_staticRenderer* rendr);

SA_INTERNAL su_s32 s_Renderer_Get_Uniform_Id(struct sc_renderer* self,
                                             const char* const uniform_name);

SA_INTERNAL void s_Renderer_Bind_Index_Buffer(struct sc_renderer* self, const su_dArray* new_indices);

SA_INTERNAL void s_Renderer_Instance_Fill_Default(struct sc_instanceRenderer* rendr);

SA_INTERNAL void s_Renderer_Init_Bound(struct sc_rendererCommon* common, const struct sc_rendererConfig cfg);

SA_INTERNAL struct sc_rendererConfig s_Renderer_Get_Static_Config(const char* const path);

// Initializes with default opengl data
SA_INTERNAL void s_Renderer_Init_GL(struct sc_rendererCommon* rendr, const struct sc_rendererConfig rendr_cfg);

SA_INTERNAL void s_Renderer_Init_Static_Batch(struct sc_staticRenderer* rendr, struct sc_rendererConfig cfg);

SA_INTERNAL void s_Renderer_Init_Instance_Batch(struct sc_instanceRenderer* rendr);

SA_INTERNAL union sc_uniformValue s_Renderer_Uniform_Value_From_Type(su_dataType type, const void* value);

SA_INTERNAL void s_Renderer_Set_Uniform_From_Uniform_Data(const struct sc_rendererUniformData uniform_data);

SA_INTERNAL void s_Renderer_Draw_Instance_Batch(const struct sc_renderer* rendr);

SA_INTERNAL void s_Renderer_Bind_Texture(struct sc_renderer* self, su_textureId tex_id);

SA_INTERNAL void s_Renderer_Push_Mesh_Instanced(struct sc_renderer* self,
                                                const su_dArray* pos_array,
                                                const su_dArray* uv_array,
                                                const su_dArray* color_array);

SA_INTERNAL void s_Renderer_Set_Uniform(struct sc_renderer* self,
                                        const su_s32 uniform_id,
                                        const void* const value,
                                        const su_dataType type);

SA_INTERNAL void s_Renderer_Instance_Begin(const struct sc_renderer* self);

SA_INTERNAL const struct sc_rendererInterface sc_INSTANCE_RENDERER_DEFAULT_INTERFACE =
    {
        .new = s_Renderer_Instanced_New,
        .begin = s_Renderer_Instance_Begin,
        .bind_texture = s_Renderer_Bind_Texture,
        .get_uniform_id = s_Renderer_Get_Uniform_Id,
        .set_uniform = s_Renderer_Set_Uniform,
        .bind_index_buffer = s_Renderer_Bind_Index_Buffer,
        .push_mesh = s_Renderer_Push_Mesh_Instanced,
        .draw = s_Renderer_Draw_Instance_Batch,
        .free = NULL,
        .free_opts = NULL,
};

/* --- Renderer Header Impl --- */

SA_API sc_renderer* sc_Renderer_New(const enum sc_rendererType type) {
    sc_renderer* rendr = su_Malloc_m(sizeof(struct sc_renderer));
    rendr->type = type;
    switch (type) {
    case sc_RENDERER_STATIC:
    case sc_RENDERER_DYNAMIC:
    case sc_RENDERER_INSTANCE:
        rendr->type = sc_RENDERER_INSTANCE;
        rendr->interface = &sc_INSTANCE_RENDERER_DEFAULT_INTERFACE;
        rendr->interface->new(rendr);
        break;
    }
    return rendr;
}

SA_API void sc_Renderer_Begin(struct sc_renderer* rendr) {
    rendr->interface->begin(rendr);
}

SA_API void sc_Renderer_Bind_Texture(struct sc_renderer* rendr, su_textureId tex_id) {
    rendr->interface->bind_texture(rendr, tex_id);
}

SA_API su_s32 sc_Renderer_Get_Uniform_Id(struct sc_renderer* rendr,
                                         const char* const uniform_name) {
    return rendr->interface->get_uniform_id(rendr, uniform_name);
}

SA_API void sc_Renderer_Set_Uniform(struct sc_renderer* rendr,
                                    const su_s32 uniform_id,
                                    const void* const value,
                                    const su_dataType type) {
    rendr->interface->set_uniform(rendr, uniform_id, value, type);
}

SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr,
                                          const su_dArray* new_indices) {
    rendr->interface->bind_index_buffer(rendr, new_indices);
}

SA_API void sc_Renderer_Push_Mesh(struct sc_renderer* rendr,
                                  const su_dArray* pos_array,
                                  const su_dArray* uv_array,
                                  const su_dArray* color_array) {
    rendr->interface->push_mesh(rendr, pos_array, uv_array, color_array);
}

SA_API void sc_Renderer_Draw(const struct sc_renderer* rendr) {
    rendr->interface->draw(rendr);
}

SA_API void sc_Renderer_Free(struct sc_renderer* rendr) {
    rendr->interface->free(rendr);
}

SA_API void sc_Renderer_Free_Opts(struct sc_renderer* rendr, int free_opts) {
    rendr->interface->free_opts(rendr, free_opts);
}

/* --- Renderer specific impl --- */

// TODO, it's working but needs some improvement
SA_API void sc_Renderer_Set_Instance_Transforms(struct sc_renderer* rendr, su_dArray* transform_array) {
    su_Log_Assert_Message_m(rendr->type == sc_RENDERER_INSTANCE, "Trying to set instance transforms in non instance renderer");

    su_Log_DebugF_Print_m(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Binding %lu transforms",
        su_DArray_Length(transform_array));
    su_DArray_Clear(rendr->rendr.instance_renderer->bound_transform_array);
    su_DArray_Append(rendr->rendr.instance_renderer->bound_transform_array, transform_array);
}

/* --- Renderer Helper impl --- */

SA_INTERNAL struct sc_staticRenderer* s_Renderer_Static_New(void) {
    struct sc_staticRenderer* rendr = su_Malloc_m(sizeof(struct sc_staticRenderer));
    struct sc_rendererConfig cfg = {0};
    s_Renderer_Static_Fill_Default(rendr);
    s_Renderer_Init_GL(&rendr->common, cfg);
    s_Renderer_Init_Static_Batch(rendr, cfg);
    return rendr;
}

SA_INTERNAL void s_Renderer_Instanced_New(sc_renderer* self) {
    struct sc_instanceRenderer* rendr = su_Malloc_m(sizeof(struct sc_instanceRenderer));
    self->rendr.instance_renderer = rendr;
    struct sc_rendererConfig cfg = {0};
    s_Renderer_Instance_Fill_Default(rendr);
    s_Renderer_Init_GL(&rendr->common, cfg);
    s_Renderer_Init_Instance_Batch(rendr);
    s_Renderer_Init_Bound(&rendr->common, cfg);
    // todo send to a separate function
    rendr->instance_vbo = sc_GL_Create_Vertex_Buffer(
        sizeof(su_mat4) * 1024, NULL, GL_DYNAMIC_DRAW);
    sc_GL_Bind_Vertex_Buffer(rendr->instance_vbo);
    for (su_u32 i = 0; i < 4; ++i) {
        sc_GL_Set_Vertex_Attrib_Pointer(3 + i, 4, GL_FLOAT, GL_FALSE,
                                        sizeof(su_mat4), (void*)(sizeof(float) * i * 4));
        sc_GL_Enable_Vertex_Attrib_Array(3 + i);
        glVertexAttribDivisor(3 + i, 1);
    }
}

SA_INTERNAL void s_Renderer_Init_GL(struct sc_rendererCommon* rendr_common, const struct sc_rendererConfig rendr_cfg) {
#ifndef SACI_RENDERING_DISABLED
    { // Shader init
        su_shaderId v_shader = sc_Shader_Compile_Shader_Vert(sc_INSTANCE_VERT_SHADER);
        su_shaderId f_shader = sc_Shader_Compile_Shader_Frag(sc_FRAG_SHADER);
        su_Log_Assert_Message_m(v_shader && f_shader, "Shaders could not be initialized");
        rendr_common->shader_program = sc_Shader_Create_Shader_Program(v_shader, f_shader);
        su_Log_Assert_Message_m(rendr_common->shader_program, "Shader program could not be initialized");
    }
    { // Opengl buffers
        rendr_common->vbo = sc_GL_Create_Vertex_Buffer(
            sizeof(struct sc_vertex) * SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            NULL, GL_DYNAMIC_DRAW);
        su_Log_Assert_Message_m(rendr_common->vbo, "VBO could not be initialized");
        rendr_common->ibo = sc_GL_Create_Index_Buffer_Dynamic(NULL, SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY);
        su_Log_Assert_Message_m(rendr_common->ibo, "IBO could not be initialized");
        sc_GL_Create_Vertex_Array(1, &rendr_common->vao);
        su_Log_Assert_Message_m(rendr_common->vao, "VAO could not be initialized");
        // TODO
    }
    { // VertexAttrib init
        sc_GL_Bind_Vertex_Array(rendr_common->vao);

        sc_GL_Bind_Vertex_Buffer(rendr_common->vbo);

        sc_GL_Set_Vertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct sc_vertex),
                                        su_Scast_To_m(void*) offsetof(struct sc_vertex, pos));
        sc_GL_Enable_Vertex_Attrib_Array(0);
        sc_GL_Set_Vertex_Attrib_Pointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct sc_vertex),
                                        su_Scast_To_m(void*) offsetof(struct sc_vertex, color));
        sc_GL_Enable_Vertex_Attrib_Array(1);
        sc_GL_Set_Vertex_Attrib_Pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct sc_vertex),
                                        su_Scast_To_m(void*) offsetof(struct sc_vertex, uv));
        sc_GL_Enable_Vertex_Attrib_Array(2);
    }
#endif // SACI_RENDERING_DISABLED
}

SA_INTERNAL void s_Renderer_Init_Static_Batch(struct sc_staticRenderer* rendr, struct sc_rendererConfig cfg) {
    const struct sc_staticBatchInfo info = rendr->batch_info;
    const struct sc_rendererBoundInfo bound_info = rendr->common.bound_info;
    su_u64 index_size = info.index_array_capacity * sizeof(su_u32);
    su_u64 vertex_size = info.vertex_array_capacity * sizeof(struct sc_vertex);
    su_u64 uniform_size = bound_info.uniform_array_capacity * sizeof(struct sc_rendererUniformData);
    su_u64 batch_arena_element_size = index_size + vertex_size + uniform_size + sizeof(struct sc_staticBatch);
    rendr->batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->batch_ptr_array = su_Malloc_m(sizeof(struct sc_staticBatch*) * info.batch_capacity);

    ArenaInit(
        &rendr->batch_arena,
        batch_arena_element_size * info.batch_capacity + 1024);

    for (su_u8 i = 0; i < info.batch_capacity; ++i) {
        void* dyn_batch_mem = ArenaPush(&rendr->batch_arena, sizeof(struct sc_staticBatch));
        void* index_mem = ArenaPush(&rendr->batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->batch_arena, vertex_size);
        void* uniform_mem = ArenaPush(&rendr->batch_arena, uniform_size);

        struct sc_staticBatch* batch =
            su_Scast_To_m(struct sc_staticBatch*)(dyn_batch_mem);
        batch->texture = SC_TEXTURE_INVALID;
        batch->index_array = su_DArray_Create_Ctx(
            index_mem,
            index_size + SIZE_OF_DARRAY,
            info.index_array_capacity,
            sizeof(su_u32),
            su_TRUE);
        batch->vertex_array = su_DArray_Create_Ctx(
            vertex_mem,
            vertex_size + SIZE_OF_DARRAY,
            info.vertex_array_capacity,
            sizeof(struct sc_vertex),
            su_TRUE);
        batch->uniform_data = su_DArray_Create_Ctx(
            uniform_mem,
            uniform_size + SIZE_OF_DARRAY,
            bound_info.uniform_array_capacity,
            sizeof(struct sc_rendererUniformData),
            su_TRUE);
        rendr->batch_ptr_array[i] = batch;
    }
}

SA_INTERNAL void s_Renderer_Init_Instance_Batch(struct sc_instanceRenderer* rendr) {
    struct sc_instanceBatchInfo instance_info = rendr->batch_info;
    struct sc_rendererBoundInfo bound_info = rendr->common.bound_info;
    su_u64 index_size = instance_info.index_array_capacity * sizeof(su_u32);
    su_u64 vertex_size = instance_info.vertex_array_capacity * sizeof(struct sc_vertex);
    su_u64 transform_size = instance_info.transform_array_capacity * sizeof(su_mat4);
    su_u64 uniform_size = bound_info.uniform_array_capacity * sizeof(struct sc_rendererUniformData);
    su_u64 batch_arena_element_size = index_size + vertex_size + transform_size + uniform_size + sizeof(struct sc_instanceBatch);
    rendr->batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->batch_ptr_array = su_Calloc_m(instance_info.batch_capacity, sizeof(struct sc_instanceBatch*));
    // TODO
    rendr->bound_transform_array = su_DArray_Create(instance_info.transform_array_capacity, sizeof(su_mat4), su_TRUE);

    ArenaInit(
        &rendr->batch_arena,
        batch_arena_element_size * instance_info.batch_capacity + 1024);

    for (su_u8 i = 0; i < instance_info.batch_capacity; ++i) {
        void* instance_batch_mem = ArenaPush(&rendr->batch_arena, sizeof(struct sc_instanceBatch));
        void* index_mem = ArenaPush(&rendr->batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->batch_arena, vertex_size);
        void* transform_mem = ArenaPush(&rendr->batch_arena, transform_size);
        void* uniform_mem = ArenaPush(&rendr->batch_arena, uniform_size);

        struct sc_instanceBatch* instance_batch =
            su_Scast_To_m(struct sc_instanceBatch*)(instance_batch_mem);
        instance_batch->texture = SC_TEXTURE_INVALID;
        instance_batch->index_array = su_DArray_Create_Ctx(
            index_mem,
            index_size + SIZE_OF_DARRAY,
            instance_info.index_array_capacity,
            sizeof(su_u32),
            su_TRUE);
        instance_batch->vertex_array = su_DArray_Create_Ctx(
            vertex_mem,
            vertex_size + SIZE_OF_DARRAY,
            instance_info.vertex_array_capacity,
            sizeof(struct sc_vertex),
            su_TRUE);
        instance_batch->model_matrix_array = su_DArray_Create_Ctx(
            transform_mem,
            transform_size + SIZE_OF_DARRAY,
            instance_info.transform_array_capacity,
            sizeof(su_mat4),
            su_TRUE);
        instance_batch->uniform_data = su_DArray_Create_Ctx(
            uniform_mem,
            uniform_size + SIZE_OF_DARRAY,
            bound_info.uniform_array_capacity,
            sizeof(struct sc_rendererUniformData),
            su_TRUE);
        rendr->batch_ptr_array[i] = instance_batch;
    }
}

SA_INTERNAL void s_Renderer_Set_Uniform_From_Uniform_Data(const struct sc_rendererUniformData uniform_data) {
    const void* value_ptr = NULL;

    switch (uniform_data.type) {
    case SU_TYPE_U8:
        value_ptr = &uniform_data.value.u8;
        break;
    case SU_TYPE_U16:
        value_ptr = &uniform_data.value.u16;
        break;
    case SU_TYPE_U32:
    case SU_TYPE_SHADERID:
    case SU_TYPE_TEXTUREID:
    case SU_TYPE_BUFFERID:
        value_ptr = &uniform_data.value.u32;
        break;
    case SU_TYPE_U64:
        value_ptr = &uniform_data.value.u64;
        break;

    case SU_TYPE_S8:
        value_ptr = &uniform_data.value.s8;
        break;
    case SU_TYPE_S16:
        value_ptr = &uniform_data.value.s16;
        break;
    case SU_TYPE_S32:
        value_ptr = &uniform_data.value.s32;
        break;
    case SU_TYPE_S64:
        value_ptr = &uniform_data.value.s64;
        break;

    case SU_TYPE_BOOL:
        value_ptr = &uniform_data.value.boolean;
        break;

    case SU_TYPE_UV:
        value_ptr = &uniform_data.value.uv;
        break;
    case SU_TYPE_VEC2:
        value_ptr = &uniform_data.value.vec2;
        break;
    case SU_TYPE_VEC3:
        value_ptr = &uniform_data.value.vec3;
        break;
    case SU_TYPE_VEC4:
        value_ptr = &uniform_data.value.vec4;
        break;

    case SU_TYPE_COLOR:
        value_ptr = &uniform_data.value.color;
        break;

    case SU_TYPE_MAT4:
        value_ptr = &uniform_data.value.mat4;
        break;

    // TODO NEED TO BE ADDED
    case SU_TYPE_MAT2:
    case SU_TYPE_MAT3:
    case SU_TYPE_MAT2X3:
    case SU_TYPE_MAT2X4:
    case SU_TYPE_MAT3X2:
    case SU_TYPE_MAT3X4:
    case SU_TYPE_MAT4X2:
    case SU_TYPE_MAT4X3:
        break;

    default:
        su_Log_ErrorF_Print_m(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", uniform_data.type);
        return;
    }

    sc_GL_Uniform_Set_Value(uniform_data.location, uniform_data.type, value_ptr);
}

SA_INTERNAL void s_Renderer_Static_Fill_Default(struct sc_staticRenderer* rendr) {
    rendr->batch_info.uniform_array_capacity = SC_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->batch_info.batch_capacity = SC_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_info.in_use = 0;
    rendr->batch_info.vertex_array_capacity = SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;

    rendr->common.bound_info.index_array_capacity = SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    rendr->common.bound_info.uniform_array_capacity = SC_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->common.bound.texture = SC_TEXTURE_INVALID;
}

SA_INTERNAL void s_Renderer_Instance_Fill_Default(struct sc_instanceRenderer* rendr) {
    rendr->batch_info.uniform_array_capacity = SC_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->batch_info.index_array_capacity = SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY;
    rendr->batch_info.vertex_array_capacity = SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;
    rendr->batch_info.transform_array_capacity = SC_RENDERER_DEFAULT_INSTANCE_TRANSFORM_ARRAY_SIZE;
    rendr->batch_info.batch_capacity = SC_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_info.in_use = 0;

    rendr->common.bound_info.index_array_capacity = SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    rendr->common.bound_info.uniform_array_capacity = SC_RENDERER_DEFAULT_UNIFORM_CAPACITY;
    rendr->common.bound.texture = SC_TEXTURE_INVALID;
}

SA_INTERNAL su_s32 s_Renderer_Get_Uniform_Id(struct sc_renderer* self,
                                             const char* const uniform_name) {
    switch (self->type) {
    case sc_RENDERER_STATIC:
        return sc_GL_Uniform_Location(
            self->rendr.static_renderer->common.shader_program,
            uniform_name);
    case sc_RENDERER_DYNAMIC:
    case sc_RENDERER_INSTANCE:
        return sc_GL_Uniform_Location(
            self->rendr.instance_renderer->common.shader_program,
            uniform_name);
    }
    return -1;
}

SA_INTERNAL void s_Renderer_Bind_Index_Buffer(struct sc_renderer* self, const su_dArray* new_indices) {
    if (!new_indices) {
        su_Log_Warn_Print_m(su_LOG_SEVERITY_MEDIUM,
                            su_LOG_CONTEXT_RENDERER,
                            "Indices are NULL and cannot be bound");
        return;
    }
    su_Log_DebugF_Print_m(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, su_LOG_CONTEXT_RENDERER,
                          "Bound %lu indices", su_DArray_Length(new_indices));

    switch (self->type) {
    case sc_RENDERER_STATIC:
        su_DArray_Clear(self->rendr.static_renderer->common.bound.index_array);
        su_DArray_Append(self->rendr.static_renderer->common.bound.index_array, new_indices);
        break;
    case sc_RENDERER_DYNAMIC:
    case sc_RENDERER_INSTANCE:
        su_DArray_Clear(self->rendr.instance_renderer->common.bound.index_array);
        su_DArray_Append(self->rendr.instance_renderer->common.bound.index_array, new_indices);
        break;
    }
}

SA_INTERNAL void s_Renderer_Bind_Texture(struct sc_renderer* self, su_textureId tex_id) {
    switch (self->type) {
    case sc_RENDERER_STATIC:
        self->rendr.static_renderer->common.bound.texture = tex_id;
        break;
    case sc_RENDERER_DYNAMIC:
        // TODO
    case sc_RENDERER_INSTANCE:
        self->rendr.instance_renderer->common.bound.texture = tex_id;
        break;
    }
}

SA_INTERNAL void s_Renderer_Push_Mesh_Instanced(struct sc_renderer* self,
                                                const su_dArray* pos_array,
                                                const su_dArray* uv_array,
                                                const su_dArray* color_array) {
    struct sc_instanceRenderer* rendr = self->rendr.instance_renderer;
    // s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);
    // TODO validate instance_transform

    su_Log_DebugF_Print_m(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu vertices to instance batch",
                          su_DArray_Length(pos_array));
    su_Log_DebugF_Print_m(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu indices to instance batch",
                          su_DArray_Length(rendr->common.bound.index_array));
    struct sc_instanceBatch* batch = rendr->batch_ptr_array[rendr->batch_info.in_use];
    su_color default_color = {0, 0, 0, 0};
    su_uv default_uv = {0, 0};
    for (su_u64 i = 0; i < su_DArray_Length(pos_array); ++i) {
        su_vec3 pos;
        su_color color = default_color;
        su_uv uv = default_uv;
        su_DArray_Get(pos_array, i, &pos);
        if (uv_array) {
            su_DArray_Get(uv_array, i, &uv);
        }
        if (color_array) {
            su_DArray_Get(color_array, i, &color);
        }
        struct sc_vertex vertex = (struct sc_vertex){pos, color, uv};
        su_DArray_Push(batch->vertex_array, &vertex);
    }
    su_DArray_Append(batch->index_array, rendr->common.bound.index_array);
    su_DArray_Append(batch->uniform_data, rendr->common.bound.uniform_data_array);
    su_DArray_Append(batch->model_matrix_array, rendr->bound_transform_array);
    rendr->batch_info.in_use++;
}

SA_INTERNAL void s_Renderer_Set_Uniform(struct sc_renderer* self,
                                        const su_s32 uniform_id,
                                        const void* const value,
                                        const su_dataType type) {
    // TODO check if is needed
    su_dArray** uniform_data_array;
    switch (self->type) {
    case sc_RENDERER_STATIC:
        uniform_data_array = &self->rendr.static_renderer->common.bound.uniform_data_array;
        break;
    case sc_RENDERER_DYNAMIC:
    case sc_RENDERER_INSTANCE:
        uniform_data_array = &self->rendr.instance_renderer->common.bound.uniform_data_array;
        break;
    }

    if (uniform_id < 0 || uniform_id >= SU_TYPE_MAX ||
        uniform_id == SU_TYPE_BUFFERID || uniform_id == SU_TYPE_SHADERID ||
        uniform_id == SU_TYPE_TEXTUREID) {
        su_Log_Error_Print_m(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid ID");
        return;
    }
    if (su_Scast_To_m(int)(type) == 0 || su_Scast_To_m(int)(type) > 26) {
        su_Log_Error_Print_m(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid type");
        return;
    }
    struct sc_rendererUniformData new_uniform = {0};
    new_uniform.type = type;
    new_uniform.value = s_Renderer_Uniform_Value_From_Type(type, value);
    new_uniform.location = uniform_id;

    struct sc_rendererUniformData* uniform_data = su_Malloc_m(sizeof(struct sc_rendererUniformData));
    for (su_u64 i = 0; i < su_DArray_Length(*uniform_data_array); ++i) {
        su_DArray_Get(*uniform_data_array, i, uniform_data);
        if (uniform_data->location == uniform_id) {
            su_Free_m(uniform_data);
            su_DArray_Set(*uniform_data_array, i, &new_uniform);
            return;
        }
    }
    su_DArray_Push(*uniform_data_array, &new_uniform);
    su_Free_m(uniform_data);
}

SA_INTERNAL void s_Renderer_Instance_Begin(const struct sc_renderer* self) {
    su_DArray_Clear(self->rendr.instance_renderer->common.bound.index_array);
    su_DArray_Clear(self->rendr.instance_renderer->common.bound.uniform_data_array);
    self->rendr.instance_renderer->common.bound.texture = SC_TEXTURE_INVALID;

    self->rendr.instance_renderer->batch_info.in_use = 0;
    for (su_u8 i = 0; i < self->rendr.instance_renderer->batch_info.batch_capacity; ++i) {
        struct sc_instanceBatch* batch = self->rendr.instance_renderer->batch_ptr_array[i];
        batch->texture = SC_TEXTURE_INVALID;
        su_DArray_Clear(batch->index_array);
        su_DArray_Clear(batch->model_matrix_array);
        su_DArray_Clear(batch->vertex_array);
        su_DArray_Clear(batch->uniform_data);
    }
}

SA_INTERNAL void s_Renderer_Init_Bound(struct sc_rendererCommon* common, const struct sc_rendererConfig cfg) {
    su_u64 index_array_capacity = cfg.bound_cfg.index_info.capacity
                                      ? cfg.bound_cfg.index_info.capacity
                                      : SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    su_u64 uniform_array_capacity = cfg.bound_cfg.uniform_info.capacity
                                        ? cfg.bound_cfg.uniform_info.capacity
                                        : SC_RENDERER_DEFAULT_UNIFORM_CAPACITY;

    common->bound.index_array = su_DArray_Create(
        index_array_capacity,
        sizeof(su_u32),
        cfg.bound_cfg.index_info.is_fixed_size);

    common->bound.uniform_data_array = su_DArray_Create(
        uniform_array_capacity,
        sizeof(struct sc_rendererUniformData),
        cfg.bound_cfg.uniform_info.is_fixed_size);
}

SA_INTERNAL struct sc_rendererConfig s_Renderer_Get_Static_Config(const char* const path) {
    struct sc_rendererConfig cfg = {0};
    if (!path) {
        su_Log_Warn_Print_m(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                            "Could not open renderer config");
        return cfg;
    }
    sc_configState* state = sc_Config_Load(path);
    if (!state) {
        su_Log_Error_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                             "Couldn't load config state");
        return cfg;
    }
    if (!sc_Config_Load_Table(state, "static_renderer")) {
        su_Log_ErrorF_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                              "Couldn't find 'static' renderer in config: %s", path);
        return cfg;
    }

    {
        su_s8 fixed_size = sc_Config_Get_Bool(state, "bound.index.fixed_size");
        SET_FIXED_SIZE(cfg.bound_cfg.index_info.is_fixed_size, SC_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE, fixed_size);

        su_u64 capacity = sc_Config_Get_Uint32(state, "bound.index.capacity");
        SET_CAPACITY(cfg.bound_cfg.index_info.capacity, SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY, capacity);
    }

    {
        su_s8 uniform_fixed_size = sc_Config_Get_Bool(state, "bound.uniform.fixed_size");
        SET_FIXED_SIZE(cfg.bound_cfg.uniform_info.is_fixed_size, SC_RENDERER_DEFAULT_UNIFORM_IS_FIXED_SIZE, uniform_fixed_size);

        su_u64 uniform_capacity = sc_Config_Get_Uint32(state, "bound.uniform.capacity");
        SET_CAPACITY(cfg.bound_cfg.uniform_info.capacity, SC_RENDERER_DEFAULT_UNIFORM_CAPACITY, uniform_capacity);
    }

    {
        su_s8 batch_index_fixed_size = sc_Config_Get_Bool(state, "batch.index.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.index_info.is_fixed_size, SC_RENDERER_DEFAULT_INDEX_IS_FIXED_SIZE, batch_index_fixed_size);

        su_u64 batch_index_capacity = sc_Config_Get_Uint32(state, "batch.index.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.index_info.capacity, SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY, batch_index_capacity);
    }

    {
        su_s8 batch_vertex_fixed_size = sc_Config_Get_Bool(state, "batch.vertex.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.vertex_info.is_fixed_size, SC_RENDERER_DEFAULT_VERTEX_IS_FIXED_SIZE, batch_vertex_fixed_size);

        su_u64 batch_vertex_capacity = sc_Config_Get_Uint32(state, "batch.vertex.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.vertex_info.capacity, SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY, batch_vertex_capacity);
    }

    {
        su_s8 batch_fixed_size = sc_Config_Get_Bool(state, "batch.fixed_size");
        SET_FIXED_SIZE(cfg.instance_batch_cfg.is_fixed_size, SC_RENDERER_DEFAULT_BATCH_IS_FIXED_SIZE, batch_fixed_size);

        su_u8 batch_capacity = sc_Config_Get_Uint8(state, "batch.capacity");
        SET_CAPACITY(cfg.instance_batch_cfg.capacity, SC_RENDERER_DEFAULT_BATCH_CAPACITY, batch_capacity);
    }

    sc_Config_Close(state);

    return cfg;
}

SA_INTERNAL void s_Renderer_Draw_Static_Batch(const struct sc_staticRenderer* rendr) {
    su_Log_DebugF_Print_m(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Flushing %d batches", rendr->batch_info.in_use);

    struct previousBatch { // Will get bigger later
        su_textureId texture;
    } previous_batch = {0};

    for (su_u8 i = 0; i < rendr->batch_info.in_use; ++i) {
        struct sc_staticBatch* batch = rendr->batch_ptr_array[i];

        glUseProgram(rendr->common.shader_program);

        glBindVertexArray(rendr->common.vao);

        glBindBuffer(GL_ARRAY_BUFFER, rendr->common.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        su_Scast_To_m(long int)(sizeof(struct sc_vertex) * su_DArray_Length(batch->vertex_array)),
                        su_DArray_Get_Ptr(batch->vertex_array, 0));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendr->common.ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                        su_Scast_To_m(long int)(sizeof(su_u32) * su_DArray_Length(batch->index_array)),
                        su_DArray_Get_Ptr(batch->index_array, 0));

        { // Uniforms
            {
                // TODO should be removed
                glEnable(GL_DEPTH_TEST);
            }
            for (su_u32 j = 0; j < su_DArray_Length(rendr->common.bound.uniform_data_array); ++j) {
                struct sc_rendererUniformData uniform_data = {0};
                su_DArray_Get(batch->uniform_data, j, &uniform_data);
                s_Renderer_Set_Uniform_From_Uniform_Data(uniform_data);
            }
        }

        if (batch->texture != previous_batch.texture) {
            glUniform1i(SC_U_USE_TEXTURE_LOC, su_TRUE);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, batch->texture);
            previous_batch.texture = batch->texture;
        }

        glDrawElements(GL_TRIANGLES, su_Scast_To_m(int)(su_DArray_Length(batch->index_array)), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

SA_INTERNAL void s_Renderer_Draw_Instance_Batch(const struct sc_renderer* self) {
    const struct sc_instanceRenderer* rendr = self->rendr.instance_renderer;
    su_Log_DebugF_Print_m(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Flushing %d instance batches",
        rendr->batch_info.in_use);

    struct previousBatch { // Will get bigger later
        su_textureId texture;
    } previous_batch = {0};

    for (su_u8 i = 0; i < rendr->batch_info.in_use; ++i) {
        struct sc_instanceBatch* batch = rendr->batch_ptr_array[i];

        glUseProgram(rendr->common.shader_program);

        glBindVertexArray(rendr->common.vao);

        glBindBuffer(GL_ARRAY_BUFFER, rendr->common.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        su_Scast_To_m(long int)(sizeof(struct sc_vertex) * su_DArray_Length(batch->vertex_array)),
                        su_DArray_Get_Ptr(batch->vertex_array, 0));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendr->common.ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                        su_Scast_To_m(long int)(sizeof(su_u32) * su_DArray_Length(batch->index_array)),
                        su_DArray_Get_Ptr(batch->index_array, 0));

        // TODO this should be checked before, since if it is empty there is no reason to drawing
        if (!su_DArray_Is_Empty(batch->model_matrix_array)) {
            glBindBuffer(GL_ARRAY_BUFFER, rendr->instance_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            su_Scast_To_m(long int)(sizeof(su_mat4) * su_DArray_Length(batch->model_matrix_array)),
                            su_DArray_Get_Ptr(batch->model_matrix_array, 0));
        }

        { // Uniforms
            {
                // TODO should be removed
                glEnable(GL_DEPTH_TEST);
            }
            for (su_u32 j = 0; j < su_DArray_Length(rendr->common.bound.uniform_data_array); ++j) {
                struct sc_rendererUniformData uniform_data = {0};
                su_DArray_Get(batch->uniform_data, j, &uniform_data);
                s_Renderer_Set_Uniform_From_Uniform_Data(uniform_data);
            }
        }

        if (batch->texture != previous_batch.texture && batch->texture != SC_TEXTURE_INVALID) {
            glUniform1i(SC_U_USE_TEXTURE_LOC, su_TRUE);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, batch->texture);
            previous_batch.texture = batch->texture;
        }

        glDrawElementsInstanced(GL_TRIANGLES,
                                su_Scast_To_m(int)(su_DArray_Length(batch->index_array)),
                                GL_UNSIGNED_INT,
                                0,
                                su_Scast_To_m(int)(su_DArray_Length(batch->model_matrix_array))); // instance count
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

SA_INTERNAL union sc_uniformValue s_Renderer_Uniform_Value_From_Type(su_dataType type, const void* value) {
    union sc_uniformValue result = {0};

    switch (type) {
    case SU_TYPE_U8:
        result.u8 = *(const su_u8*)value;
        break;
    case SU_TYPE_U16:
        result.u16 = *(const su_u16*)value;
        break;
    case SU_TYPE_U32:
        result.u32 = *(const su_u32*)value;
        break;
    case SU_TYPE_U64:
        result.u64 = *(const su_u64*)value;
        break;

    case SU_TYPE_S8:
        result.s8 = *(const su_s8*)value;
        break;
    case SU_TYPE_S16:
        result.s16 = *(const su_s16*)value;
        break;
    case SU_TYPE_S32:
        result.s32 = *(const su_s32*)value;
        break;
    case SU_TYPE_S64:
        result.s64 = *(const su_s64*)value;
        break;

    case SU_TYPE_BOOL:
        result.boolean = *(const su_bool*)value;
        break;

    case SU_TYPE_UV:
        result.uv = *(const su_uv*)value;
        break;
    case SU_TYPE_VEC2:
        result.vec2 = *(const su_vec2*)value;
        break;
    case SU_TYPE_VEC3:
        result.vec3 = *(const su_vec3*)value;
        break;
    case SU_TYPE_VEC4:
        result.vec4 = *(const su_vec4*)value;
        break;

    case SU_TYPE_COLOR:
        result.color = *(const su_color*)value;
        break;

    case SU_TYPE_MAT2:
    case SU_TYPE_MAT3:
        break;
    case SU_TYPE_MAT4:
        result.mat4 = *(const su_mat4*)value;
        break;

    case SU_TYPE_MAT2X3:
    case SU_TYPE_MAT2X4:
    case SU_TYPE_MAT3X2:
    case SU_TYPE_MAT3X4:
    case SU_TYPE_MAT4X2:
    case SU_TYPE_MAT4X3:
        break;

    default:
        su_Log_ErrorF_Print_m(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", type);
        break;
    }

    return result;
}
