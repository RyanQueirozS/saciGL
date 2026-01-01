#include "saci_platform/config/config.h"

#include "saci_util/memory.h"
#include "saci_util/defines.h"
#include "saci_util/log.h"

#include "saci_util/internal/general.h"
#include "saci_util/internal/log.h"

#include <string.h>

/* === Internal === */

/* === Iterables get_renderer_pool Func === */
SACI_INTERNAL void psaci__cfg_iterable_vertex_layout_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_iterable_uniform_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_iterable_sampler_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_iterable_batch_instance_buffers_size(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_iterable_batch_instance_buffer_layout_name(PSaciLuaState*, SaciU64 idx, void* user_data);

SACI_INTERNAL SaciMemPool* psaci__cfg_get_renderer_pool(PSaciLuaState*, const char* name);

/* === Preallocate Funcs === */
struct PSaciCfgRendererPreallocEntity {
    const char* path_to_value;
    SaciU64* length_ptr;
    void** data_ptr;
    const SaciU64 STRUCT_SIZE;
};

SACI_INTERNAL void psaci__cfg_load_preallocate_component_array(PSaciLuaState* lua, SaciMemPool* pool, struct PSaciCfgRendererPreallocEntity prealloc_entity);
SACI_INTERNAL void psaci__cfg_load_preallocate_system(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool);

/* === Iterables Load Funcs === */

// All user_data in this section is PSaciCfgLoadRendererPool
SACI_INTERNAL void psaci__cfg_iterable_load_vertex_data(PSaciLuaState*, SaciU64 idx, void* user_data);

/* === Load Funcs === */
SACI_INTERNAL void psaci__cfg_load_iterable_fields(PSaciLuaState*, struct PSaciRendererConfig* cfg_out);

SACI_INTERNAL void psaci__cfg_load_renderer_vertex_data(PSaciLuaState*, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool);
SACI_INTERNAL void psaci__cfg_load_renderer_index_data(PSaciLuaState*, struct PSaciRendererConfig* cfg_out);
SACI_INTERNAL void psaci__cfg_load_renderer_shaders(PSaciLuaState*, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool);
SACI_INTERNAL void psaci__cfg_load_renderer_uniforms(PSaciLuaState*, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool);
SACI_INTERNAL void psaci__cfg_load_renderer_samplers(PSaciLuaState*, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool);
SACI_INTERNAL void psaci__cfg_load_renderer_batch(PSaciLuaState*, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool);
SACI_INTERNAL void psaci__cfg_load_renderer_bound(PSaciLuaState*, struct PSaciRendererConfig* cfg_out);
SACI_INTERNAL void psaci__cfg_load_renderer_draw(PSaciLuaState*, struct PSaciRendererConfig* cfg_out);
SACI_INTERNAL void psaci__cfg_load_renderer_pipeline(PSaciLuaState*, struct PSaciRendererConfig* cfg_out);

/* === config.h Impl === */

void psaci_cfg_get_renderer(const char* name, struct PSaciRendererConfig* cfg_out, const char* cfg_file_path)
{
    PSaciLuaState* lua = psaci_lua_load(cfg_file_path);

    SaciMemPool* pool = psaci__cfg_get_renderer_pool(lua, name);

    cfg_out->name = saci_mem_pool_cpy_str(name, pool);

    psaci__cfg_load_iterable_fields(lua, cfg_out);

    psaci__cfg_load_renderer_vertex_data(lua, cfg_out, pool);
    psaci__cfg_load_renderer_index_data(lua, cfg_out);
    psaci__cfg_load_renderer_shaders(lua, cfg_out, pool);
    psaci__cfg_load_renderer_uniforms(lua, cfg_out, pool);
    psaci__cfg_load_renderer_samplers(lua, cfg_out, pool);
    psaci__cfg_load_renderer_batch(lua, cfg_out, pool);
    psaci__cfg_load_renderer_bound(lua, cfg_out);
    psaci__cfg_load_renderer_draw(lua, cfg_out);
    psaci__cfg_load_renderer_pipeline(lua, cfg_out);

    psaci_lua_close(lua);
}

SaciU64 psaci_cfg_render_cfg_size(const struct PSaciRendererConfig* cfg)
{
    SaciU64 total_size = 0;
    return total_size;
}

void psaci_cfg_cleanup_renderer_cfg(struct PSaciRendererConfig* cfg)
{
}

/* === Internal Impl === */

SACI_INTERNAL void psaci__cfg_lua_get_length_name(
    PSaciLuaState* lua,
    SaciU64* total_size,
    SaciU64 struct_size)
{
    struct PSaciLuaValue val = {0};
    if (!psaci_lua_get_value(lua, "name", &val, SACI_TYPE_STRING)) {
        return;
    }
    *total_size += SACI_STRSIZE_M(val.data.string) + struct_size;
}

void psaci__cfg_iterable_vertex_layout_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;
    psaci__cfg_lua_get_length_name(lua, (SaciU64*)user_data, sizeof(struct PSaciRendererCfgVertexLayout));
}

void psaci__cfg_iterable_uniform_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;
    psaci__cfg_lua_get_length_name(lua, (SaciU64*)user_data, sizeof(struct PSaciRendererCfgUniform));
}

void psaci__cfg_iterable_sampler_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;
    psaci__cfg_lua_get_length_name(lua, (SaciU64*)user_data, sizeof(struct PSaciRendererCfgSampler));
}

void psaci__cfg_iterable_batch_instance_buffers_size(
    PSaciLuaState* lua,
    SaciU64 idx,
    void* user_data)
{
    (void)idx;

    psaci_lua_array_iter(lua, "layout", psaci__cfg_iterable_batch_instance_buffer_layout_name, user_data);

    SaciU64* total_size = (SaciU64*)user_data;
    struct PSaciLuaValue val = {0};

    if (!psaci_lua_get_value(lua, "name", &val, SACI_TYPE_STRING)) {
        return;
    }
    *total_size += sizeof(struct PSaciRendererCfgInstanceBuffer);
}

void psaci__cfg_iterable_batch_instance_buffer_layout_name(
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

struct PSaciCfgArrayEntry {
    const char* path;
    PSaciLuaArrayIter iter;
};

SACI_INTERNAL_CONST struct PSaciCfgArrayEntry PSACI_G_CFG_RENDERER_POOL_LENGTH_TABLE[4] = {
    {"vertex.layout", psaci__cfg_iterable_vertex_layout_size},
    {"uniforms", psaci__cfg_iterable_uniform_size},
    {"samplers", psaci__cfg_iterable_sampler_size},
    {"batch.instances.buffers", psaci__cfg_iterable_batch_instance_buffers_size},
};

// Returns a memory pool with preallocated memory to use in the renderer cfg.
SaciMemPool* psaci__cfg_get_renderer_pool(PSaciLuaState* lua, const char* name)
{
    if (!lua) {
        return NULL;
    }
    SaciU64 total_size = 0;
    total_size += SACI_STRSIZE_M(name);

    struct PSaciLuaValue val = {0};

    for (SaciU64 i = 0; i < SACI_ARRLEN_M(PSACI_G_CFG_RENDERER_POOL_LENGTH_TABLE); ++i) {
        struct PSaciCfgArrayEntry entry = PSACI_G_CFG_RENDERER_POOL_LENGTH_TABLE[i];
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

void psaci__cfg_load_preallocate_component_array(PSaciLuaState* lua, SaciMemPool* pool, struct PSaciCfgRendererPreallocEntity prealloc_entity)
{
    psaci_lua_push_to_stack(lua, prealloc_entity.path_to_value);
    *prealloc_entity.length_ptr = psaci_lua_get_array_length_in_stack(lua);
    psaci_lua_clear_stack(lua);

    *prealloc_entity.data_ptr = saci_mem_pool_alloc(pool, prealloc_entity.STRUCT_SIZE * (*prealloc_entity.length_ptr));
}

void psaci__cfg_load_preallocate_system(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool)
{
    struct PSaciCfgRendererPreallocEntity prealloc_table[1] = {
        {"vertex.data.layout", &cfg_out->vertex_data.layout_array_length, (void**)&cfg_out->vertex_data.layout_array, sizeof(struct PSaciRendererCfgVertexLayout)},
    };
    for (SaciU64 i = 0; i < SACI_ARRLEN_M(prealloc_table); ++i) {
        psaci__cfg_load_preallocate_component_array(lua, pool, prealloc_table[i]);
    }
}

void psaci__cfg_iterable_load_vertex_data(PSaciLuaState* lua, SaciU64 idx, void* user_data)
{
    (void)idx;

    // psaci_lua_get_value(lua, "name");
    // if (psaci_lua_push_field_array(lua, "layout")) {
    //     SaciU64 count = psaci_lua_get_array_length(lua);
    //     cfg_out->vertex_data.layout_array_length = count;
    //     cfg_out->vertex_data.layout_array = saci_mem_pool_alloc(pool, count * sizeof(struct PSaciRendererCfgVertexLayout));
    //     cfg_out->vertex_data.element_size_internal = 0;
    //
    //     for (SaciU64 i = 0; i < count; ++i) {
    //         if (psaci_lua_push_array_entry(lua, i)) {
    //             struct PSaciRendererCfgVertexLayout layout = {
    //                 .type = SACI_CAST_M(SaciDataType)(psaci_lua_get_enum(lua, "type")),
    //                 .offset = psaci_lua_get_uint64(lua, "offset"),
    //                 .location = psaci_lua_get_uint32(lua, "location"),
    //             };
    //
    //             const char* name_str = psaci_lua_get_str(lua, "name");
    //             if (name_str) {
    //                 SaciU64 size = (strlen(name_str) + 1) * sizeof(char);
    //                 layout.name = saci_mem_pool_alloc(pool, size);
    //                 saci_mem_safe_copy(layout.name, size, 0, name_str, size, 0, size);
    //             }
    //
    //             cfg_out->vertex_data.layout_array[i] = layout;
    //             psaci_lua_pop(lua, 1);
    //         }
    //     }
    //
    //     psaci_lua_pop(lua, 1);
    // }
    //
    // psaci_lua_pop(lua, 1);
}

SACI_INTERNAL_CONST struct PSaciCfgArrayEntry PSACI_G_CFG_RENDERER_FILL_TABLE[] = {
    {"vertex.layout", psaci__cfg_iterable_load_vertex_data},
};

void psaci__cfg_load_iterable_fields(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out)
{
    for (SaciU64 i = 0; i < SACI_ARRLEN_M(PSACI_G_CFG_RENDERER_FILL_TABLE); ++i) {
        struct PSaciCfgArrayEntry entry = PSACI_G_CFG_RENDERER_FILL_TABLE[i];
        psaci_lua_array_iter(lua, entry.path, entry.iter, &cfg_out);
    }
}

void psaci__cfg_load_renderer_index_data(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out)
{
    // if (psaci_lua_push_field_table(lua, "index")) {
    //     cfg_out->index_data.element_size_internal = SACI_G_TYPE_SIZE_TABLE[psaci_lua_get_enum(lua, "element_type")];
    //     psaci_lua_pop(lua, 1);
    // }
}

void psaci__cfg_load_renderer_shaders(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool)
{
    // if (!psaci_lua_push_field_table(lua, "shaders")) {
    //     return;
    // }
    //
    // const char* frag = psaci_lua_get_str(lua, "frag");
    // if (frag) {
    //     SaciU64 size = (strlen(frag) + 1) * sizeof(char);
    //     cfg_out->shaders.frag = saci_mem_pool_alloc(pool, size);
    //     saci_mem_safe_copy(cfg_out->shaders.frag, size, 0, frag, size, 0, size);
    // }
    //
    // const char* vert = psaci_lua_get_str(lua, "vert");
    // if (vert) {
    //     SaciU64 size = (strlen(vert) + 1) * sizeof(char);
    //     cfg_out->shaders.vert = saci_mem_pool_alloc(pool, size);
    //     saci_mem_safe_copy(cfg_out->shaders.vert, size, 0, vert, size, 0, size);
    // }
    //
    // const char* geom = psaci_lua_get_str(lua, "geom");
    // if (geom) {
    //     SaciU64 size = (strlen(geom) + 1) * sizeof(char);
    //     cfg_out->shaders.geom = saci_mem_pool_alloc(pool, size);
    //     saci_mem_safe_copy(cfg_out->shaders.geom, size, 0, geom, size, 0, size);
    // }
    //
    // psaci_lua_pop(lua, 1);
}

void psaci__cfg_load_renderer_uniforms(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool)
{
    // if (psaci_lua_push_field_array(lua, "uniforms")) {
    //     SaciU64 count = psaci_lua_get_array_length(lua);
    //     cfg_out->uniform_array_length = count;
    //     cfg_out->uniform_array = saci_mem_pool_alloc(pool, count * sizeof(struct PSaciRendererCfgUniform));
    //
    //     for (SaciU64 i = 0; i < count; i++) {
    //         if (psaci_lua_push_array_entry(lua, i)) {
    //             struct PSaciRendererCfgUniform uniform = {
    //                 .type = SACI_CAST_M(SaciDataType)(psaci_lua_get_enum(lua, "type")),
    //                 .location = SACI_CAST_M(SaciS32)(psaci_lua_get_uint32(lua, "location")),
    //             };
    //             const char* name_str = psaci_lua_get_str(lua, "name");
    //             if (name_str) {
    //                 SaciU64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
    //                 uniform.name = saci_mem_pool_alloc(pool, name_str_size);
    //                 saci_mem_safe_copy(uniform.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
    //             }
    //             cfg_out->uniform_array[i] = uniform;
    //             psaci_lua_pop(lua, 1);
    //         }
    //     }
    //     psaci_lua_pop(lua, 1);
    // }
}

void psaci__cfg_load_renderer_samplers(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool)
{
    // if (psaci_lua_push_field_array(lua, "samplers")) {
    //     SaciU64 count = psaci_lua_get_array_length(lua);
    //     cfg_out->sampler_array_length = count;
    //     cfg_out->sampler_array = saci_mem_pool_alloc(pool, count * sizeof(struct PSaciRendererCfgSampler));
    //
    //     for (SaciU64 i = 0; i < count; ++i) {
    //         if (psaci_lua_push_array_entry(lua, i)) {
    //             struct PSaciRendererCfgSampler sampler = {
    //                 .type = SACI_CAST_M(SaciDataType)(psaci_lua_get_enum(lua, "type")),
    //                 .binding = SACI_CAST_M(SaciS32)(psaci_lua_get_uint32(lua, "binding")),
    //             };
    //             const char* name_str = psaci_lua_get_str(lua, "name");
    //             if (name_str) {
    //                 SaciU64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
    //                 sampler.name = saci_mem_pool_alloc(pool, name_str_size);
    //                 saci_mem_safe_copy(sampler.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
    //             }
    //             psaci_lua_pop(lua, 1);
    //         }
    //     }
    //     psaci_lua_pop(lua, 1);
    // }
}

void psaci__cfg_load_renderer_batch(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out, SaciMemPool* pool)
{
    // if (!psaci_lua_push_field_table(lua, "batch")) {
    //     return;
    // }
    //
    // cfg_out->batch.capacity = psaci_lua_get_uint64(lua, "capacity");
    //
    // if (psaci_lua_push_field_table(lua, "index")) {
    //     cfg_out->batch.index_cfg.capacity = psaci_lua_get_uint64(lua, "capacity");
    //     psaci_lua_pop(lua, 1); // pop index
    // }
    //
    // if (psaci_lua_push_field_table(lua, "vertex")) {
    //     cfg_out->batch.vertex_cfg.capacity = psaci_lua_get_uint64(lua, "capacity");
    //     psaci_lua_pop(lua, 1); // pop vertex
    // }
    //
    // if (psaci_lua_push_field_table(lua, "instances")) {
    //     cfg_out->batch.instance_cfg.capacity = psaci_lua_get_uint64(lua, "capacity");
    //
    //     if (psaci_lua_push_field_array(lua, "buffers")) { // FIXED: should be push_field_array
    //         SaciU64 buffer_count = psaci_lua_get_array_length(lua);
    //         cfg_out->instance_data.buffer_array_length = buffer_count;
    //         cfg_out->instance_data.buffer_array = saci_mem_pool_alloc(pool, sizeof(struct PSaciRendererCfgInstanceBuffer) * buffer_count);
    //
    //         for (SaciU64 b = 0; b < buffer_count; ++b) {
    //             if (psaci_lua_push_array_entry(lua, b)) {
    //                 struct PSaciRendererCfgInstanceBuffer buffer = {0};
    //
    //                 const char* name_str = psaci_lua_get_str(lua, "name");
    //                 if (name_str) {
    //                     SaciU64 name_size = (strlen(name_str) + 1);
    //                     buffer.name = saci_mem_pool_alloc(pool, name_size);
    //                     saci_mem_safe_copy(buffer.name, name_size, 0, name_str, name_size, 0, name_size);
    //                 }
    //
    //                 if (psaci_lua_push_field_array(lua, "layout")) {
    //                     SaciU64 layout_count = psaci_lua_get_array_length(lua);
    //                     buffer.layout_array_length = layout_count;
    //                     buffer.layout_array = saci_mem_pool_alloc(pool, layout_count * sizeof(struct PSaciRendererCfgInstanceBufferLayout));
    //                     buffer.size_byte_internal = 0;
    //
    //                     for (SaciU64 l = 0; l < layout_count; ++l) {
    //                         if (psaci_lua_push_array_entry(lua, l)) {
    //                             struct PSaciRendererCfgInstanceBufferLayout layout = {
    //                                 .type = SACI_CAST_M(SaciDataType)(psaci_lua_get_enum(lua, "type")),
    //                                 .offset = psaci_lua_get_uint64(lua, "offset"),
    //                                 .location = psaci_lua_get_uint32(lua, "location"),
    //                             };
    //
    //                             const char* layout_name_str = psaci_lua_get_str(lua, "name");
    //                             if (layout_name_str) {
    //                                 SaciU64 layout_name_size = (strlen(layout_name_str) + 1);
    //                                 layout.name = saci_mem_pool_alloc(pool, layout_name_size);
    //                                 saci_mem_safe_copy(layout.name, layout_name_size, 0, layout_name_str, layout_name_size, 0, layout_name_size);
    //                             }
    //
    //                             buffer.size_byte_internal += SACI_G_TYPE_SIZE_TABLE[layout.type];
    //                             buffer.layout_array[l] = layout;
    //
    //                             psaci_lua_pop(lua, 1); // pop array entry
    //                         }
    //                     }
    //
    //                     psaci_lua_pop(lua, 1); // pop layout array
    //                 }
    //
    //                 cfg_out->instance_data.buffer_array[b] = buffer;
    //
    //                 psaci_lua_pop(lua, 1); // pop buffer entry
    //             }
    //         }
    //
    //         psaci_lua_pop(lua, 1); // pop buffers array
    //     }
    //
    //     psaci_lua_pop(lua, 1); // pop instances
    // }

    // psaci_lua_pop(lua, 1); // pop batch
}

void psaci__cfg_load_renderer_bound(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out)
{
    // if (psaci_lua_push_field_table(lua, "bound")) {
    //     cfg_out->bound.index_cfg.capacity = psaci_lua_get_uint64(lua, "index_capacity");
    //     cfg_out->bound.instance_cfg.capacity = psaci_lua_get_uint64(lua, "instance_capacity");
    //     psaci_lua_pop(lua, 1);
    // }
}

void psaci__cfg_load_renderer_draw(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out)
{
    // if (psaci_lua_push_field_table(lua, "draw")) {
    //     cfg_out->draw = (struct PSaciRendererCfgDraw){
    //         .primitive = SACI_CAST_M(enum PSaciRendererPrimitives)(psaci_lua_get_enum(lua, "primitives")),
    //         .cull_mode = SACI_CAST_M(enum PSaciRendererCullMode)(psaci_lua_get_enum(lua, "cull_mode")),
    //         .front_face = SACI_CAST_M(enum PSaciRendererFrontFace)(psaci_lua_get_enum(lua, "front_face")),
    //     };
    //     psaci_lua_pop(lua, 1);
    // }
}

void psaci__cfg_load_renderer_pipeline(PSaciLuaState* lua, struct PSaciRendererConfig* cfg_out)
{
    // if (psaci_lua_push_field_table(lua, "pipeline")) {
    //     cfg_out->pipeline.depth_test = psaci_lua_get_bool(lua, "depth_test");
    //     if (psaci_lua_push_field_table(lua, "blend")) {
    //         cfg_out->pipeline.blend.enabled = psaci_lua_get_bool(lua, "enabled");
    //         psaci_lua_pop(lua, 1);
    //     }
    //     psaci_lua_pop(lua, 1);
    // }
}
