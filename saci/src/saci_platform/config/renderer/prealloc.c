#include "saci_platform/config/internal/renderer_prealloc.h"

#include "saci_platform/config/config.h"
#include "saci_platform/config/internal/lua.h"
#include "saci_platform/config/internal/prealloc.h"

#include "saci_util/memory.h"
#include "saci_util/internal/general.h"

/* = Internal Funcs = */

/* == Size Iterables == */

SACI_INTERNAL void psaci__cfg_renderer_iterable_vertex_layout_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_uniform_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_sampler_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_batch_instance_buffers_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_batch_instance_buffer_layout_name(PSaciLuaState*, SaciU64 idx, void* user_data);

/* == Internal Pool Funcs == */

SACI_INTERNAL SaciMemPool* psaci__cfg_renderer_get_pool(PSaciLuaState* lua, const char* name);

/* = renderer_prealloc.h = */

void psaci_cfg_renderer_prealloc(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out, const char* renderer_name, SaciMemPool** pool_out)
{
    if (!pool_out) {
        return;
    }
    *pool_out = psaci__cfg_renderer_get_pool(lua, renderer_name);
    struct PSaciCfgPreallocArrayEntity prealloc_table[1] = {
        {"vertex.layout", &cfg_out->vertex_data.layout_array_length, (void**)&cfg_out->vertex_data.layout_array, sizeof(struct PSaciRendererCfgVertexLayout)},
    };

    for (SaciU64 i = 0; i < SACI_ARRLEN_M(prealloc_table); ++i) {
        psaci_cfg_preallocate_array_component(lua, *pool_out, prealloc_table[i]);
    }
}

/* = Internal Funcs Implementation = */

/* == Size Iterables == */

void psaci__cfg_renderer_iterable_vertex_layout_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;
    psaci_lua_get_length_name(lua, (SaciU64*)user_data, sizeof(struct PSaciRendererCfgVertexLayout));
}

void psaci__cfg_renderer_iterable_uniform_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;
    psaci_lua_get_length_name(lua, (SaciU64*)user_data, sizeof(struct PSaciRendererCfgUniform));
}

void psaci__cfg_renderer_iterable_sampler_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;
    psaci_lua_get_length_name(lua, (SaciU64*)user_data, sizeof(struct PSaciRendererCfgSampler));
}

void psaci__cfg_renderer_iterable_batch_instance_buffers_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;

    psaci_lua_array_iter(lua, "layout", psaci__cfg_renderer_iterable_batch_instance_buffer_layout_name, user_data);

    SaciU64* total_size = (SaciU64*)user_data;
    struct PSaciLuaValue val = {0};

    if (!psaci_lua_get_value(lua, "name", &val, SACI_TYPE_STRING)) {
        return;
    }
    *total_size += sizeof(struct PSaciRendererCfgInstanceBuffer);
}

void psaci__cfg_renderer_iterable_batch_instance_buffer_layout_name(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;

    SaciU64* total_size = (SaciU64*)user_data;
    struct PSaciLuaValue val = {0};

    if (!psaci_lua_get_value(lua, "name", &val, SACI_TYPE_STRING)) {
        return;
    }
    *total_size += sizeof(struct PSaciRendererCfgInstanceBufferLayout) + SACI_STRSIZE_M(val.data.string);
}

/* == Internal Pool Funcs == */

SACI_INTERNAL_CONST struct PSaciCfgArrayIterablePathTable PSACI_G_CFG_RENDERER_POOL_LENGTH_TABLE[4] = {
    {"vertex.layout", psaci__cfg_renderer_iterable_vertex_layout_size},
    {"uniforms", psaci__cfg_renderer_iterable_uniform_size},
    {"samplers", psaci__cfg_renderer_iterable_sampler_size},
    {"batch.instances.buffers", psaci__cfg_renderer_iterable_batch_instance_buffers_size},
};

// Returns a memory pool with preallocated memory to use in the renderer cfg.
SaciMemPool* psaci__cfg_renderer_get_pool(PSaciLuaState* lua, const char* name)
{
    if (!lua) {
        return NULL;
    }
    SaciU64 total_size = 0;
    total_size += SACI_STRSIZE_M(name);

    struct PSaciLuaValue val = {0};

    for (SaciU64 i = 0; i < SACI_ARRLEN_M(PSACI_G_CFG_RENDERER_POOL_LENGTH_TABLE); ++i) {
        struct PSaciCfgArrayIterablePathTable entry = PSACI_G_CFG_RENDERER_POOL_LENGTH_TABLE[i];
        psaci_lua_array_iter(lua, entry.path, entry.iter, &total_size);
    }

    if (psaci_lua_get_value(lua, "shader.frag", &val, SACI_TYPE_STRING)) {
        total_size += SACI_STRSIZE_M(val.data.string);
    }
    if (psaci_lua_get_value(lua, "shader.vert", &val, SACI_TYPE_STRING)) {
        total_size += SACI_STRSIZE_M(val.data.string);
    }
    if (psaci_lua_get_value(lua, "shader.geom", &val, SACI_TYPE_STRING)) {
        total_size += SACI_STRSIZE_M(val.data.string);
    }

    return saci_mem_create_pool(SACI_MEM_CONTEXT_CONFIG, total_size);
}
