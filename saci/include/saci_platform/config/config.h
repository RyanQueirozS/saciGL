#ifndef SACI_PLATFORM_CONFIG_CONFIG_H
#define SACI_PLATFORM_CONFIG_CONFIG_H

#include "saci_util/types.h"
#include "saci_util/defines.h"

#include <lua5.4/lua.h>
#include <saci_util/memory.h>

/* === Lua CFG === */

enum PSaciLuaType {
    PSACI_LUA_TYPE_NIL,
    PSACI_LUA_TYPE_NUMBER,
    PSACI_LUA_TYPE_BOOLEAN,
    PSACI_LUA_TYPE_STRING,
    PSACI_LUA_TYPE_TABLE,
    PSACI_LUA_TYPE_USERDATA,
    PSACI_LUA_TYPE_FUNTION,
};

struct PSaciLuaValue {
    enum PSaciLuaType type;
    union {
        double number;
        int boolean;
        SaciMemChunk* string; /* Caller must free */
        void* userdata;       /* Lua-owned */
    } data;
};

typedef lua_State PSaciLuaState;

typedef void (*PSaciLuaArrayIter)(PSaciLuaState* lua, SaciU64 idx, void* user_data);

SACI_API PSaciLuaState* psaci_lua_load(const char* file_path);

SACI_API void psaci_lua_close(PSaciLuaState* state);

SACI_API void psaci_lua_dump_stack(PSaciLuaState* l);

SACI_API void psaci_lua_clear_stack(PSaciLuaState* lua);

SACI_API void psaci_lua_pop(PSaciLuaState* lua, int pops);

SACI_API SaciBool psaci_lua_push_to_stack(PSaciLuaState* lua, const char* path_to_value);

SACI_API SaciBool psaci_lua_push_array_entry_to_stack(PSaciLuaState* lua, const SaciU64 idx);

SACI_API SaciBool psaci_lua_array_iter(PSaciLuaState* lua, const char* path_to_array, PSaciLuaArrayIter array_iter, void* user_data);

// Already pops the data for the "path_to_value"
SACI_API SaciBool psaci_lua_get_value(PSaciLuaState* lua, const char* path_to_value, struct PSaciLuaValue* value_out, const enum PSaciLuaType expected_type);

SACI_API SaciBool psaci_lua_has_value(PSaciLuaState* lua, const char* path_to_value, enum PSaciLuaType expected_type);

SACI_API SaciU64 psaci_lua_get_array_length_in_stack(PSaciLuaState* lua);

SACI_API SaciU64 psaci_lua_get_array_length(PSaciLuaState* lua, const char* path_to_array);

/* === Renderer CFG === */

// TODO evaluate if these should be exposed
struct PSaciRendererCfgUniform {
    char* name; // NOT Debug
    SaciDataType type;
    SaciS32 location_internal;
};

struct PSaciRendererCfgSampler {
    char* name; // Debug
    SaciDataType type;
    SaciS32 binding;
    SaciU64 unit;
};

struct PSaciRendererCfgVertexElement {
    char* name;
    SaciDataType type;
    SaciU64 offset;
    SaciU32 location;
};

struct PSaciRendererCfgInstanceBufferLayout {
    char* name;
    SaciDataType type;
    SaciU64 offset;
    SaciU32 location;
};

struct PSaciRendererCfgInstanceBuffer {
    char* name;
    SaciU64 size_byte_internal;
    SaciU64 layout_array_length;
    struct PSaciRendererCfgInstanceBufferLayout* layout_array;
};

struct PSaciRendererCfgBatch {
    SaciU64 capacity;
    SaciBool fixed_capacity;

    struct {
        SaciU64 capacity;
    } index_cfg;
    struct {
        SaciU64 capacity;
    } vertex_cfg;
    struct {
        SaciU64 capacity;
    } instance_cfg;
};

struct PSaciRendererCfgBound {
    struct {
        SaciU64 capacity;
    } index_cfg;
    struct {
        SaciU64 capacity;
    } instance_cfg;
};

enum PSaciRendererPrimitives {
    PSACI_PRIMITIVES_TRIANGLES = 1,
};

enum PSaciRendererCullMode {
    PSACI_RENDERER_CULL_MODE_BACK = 1,
};

enum PSaciRendererFrontFace {
    PSACI_RENDERER_FRONT_FACE_CCW = 1,
};

struct PSaciRendererCfgDraw {
    enum PSaciRendererPrimitives primitive;
    enum PSaciRendererCullMode cull_mode;
    enum PSaciRendererFrontFace front_face;
};

struct PSaciRendererCfgBlend {
    SaciBool enabled;
    SaciU64 opts;
};

struct PSaciRendererCfgPipeline {
    SaciBool depth_test;
    struct PSaciRendererCfgBlend blend;
};

struct PSaciConfigRenderer {
    char* name; // Debug
    struct {
        char* frag;
        char* vert;
        char* geom;
    } shaders;
    SaciU64 uniform_array_length;
    struct PSaciRendererCfgUniform* uniform_array;
    SaciU64 sampler_array_length;
    struct PSaciRendererCfgSampler* sampler_array;

    struct PSaciRendererCfgVertexAttributes {
        SaciU64 element_size_internal;
        SaciU64 element_array_length;
        struct PSaciRendererCfgVertexElement* element_array;
    } vertex_attributes;

    struct {
        SaciU64 element_size_internal;
    } index_data;

    struct {
        SaciU64 buffer_array_length;
        struct PSaciRendererCfgInstanceBuffer* buffer_array;
    } instance_data;

    struct PSaciRendererCfgBatch batch;
    struct PSaciRendererCfgBound bound;
    struct PSaciRendererCfgDraw draw;
    struct PSaciRendererCfgPipeline pipeline;
};

SACI_API void psaci_cfg_renderer_get(const char* name, struct PSaciConfigRenderer* cfg_out, const char* cfg_file_path);

SACI_API SaciU64 psaci_cfg_renderer_cfg_size(const struct PSaciConfigRenderer* cfg);

SACI_API void psaci_cfg_renderer_cleanup(struct PSaciConfigRenderer* cfg);

enum {
    PSACI_CONFIG_RENDERER_PRINT_OPT_SHADERS = 1 << 0,
    PSACI_CONFIG_RENDERER_PRINT_OPT_UNIFORMS = 1 << 1,
    PSACI_CONFIG_RENDERER_PRINT_OPT_SAMPLERS = 1 << 2,
    PSACI_CONFIG_RENDERER_PRINT_OPT_VERTEX_ATTRIB = 1 << 3,
    PSACI_CONFIG_RENDERER_PRINT_OPT_INDEX_DATA = 1 << 4,
    PSACI_CONFIG_RENDERER_PRINT_OPT_INSTANCE_DATA = 1 << 5,
    PSACI_CONFIG_RENDERER_PRINT_OPT_BATCH = 1 << 6,
    PSACI_CONFIG_RENDERER_PRINT_OPT_BOUND = 1 << 7,
    PSACI_CONFIG_RENDERER_PRINT_OPT_DRAW_DATA = 1 << 8,
    PSACI_CONFIG_RENDERER_PRINT_OPT_PIPELINE = 1 << 9,
};

SACI_API void psaci_cfg_renderer_print(const struct PSaciConfigRenderer* cfg, int print_opts);

#endif // SACI_PLATFORM_CONFIG_CONFIG_H
