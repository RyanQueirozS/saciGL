#include "saci_platform/config/config.h"

#include "saci_platform/config/internal/prealloc.h"
#include "saci_platform/config/internal/lua.h"

#include "saci_util/defines.h"
#include "saci_util/internal/general.h"
#include "saci_util/internal/log.h"

#include <saci_util/internal/sorting_algorithms.h>
#include <saci_util/log.h>
#include <saci_util/types.h>
#include <string.h>
#include <time.h>

/* = Internal = */

/* == Iterable Func Helpers == */

/* == Iterable Funcs == */

SACI_INTERNAL void psaci__cfg_renderer_iterable_vertex_attribute(PSaciLuaState*, SaciU64 idx, void* user_data);

/* == Populate Funcs == */

SACI_INTERNAL void psaci__cfg_renderer_populate_vertex_attributes(PSaciLuaState* lua, struct PSaciRendererCfgVertexAttributes* attribute_out);

SACI_INTERNAL void psaci__cfg_renderer_populate_iterable_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out);
SACI_INTERNAL void psaci__cfg_renderer_populate_empty_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out);

/* = renderer_populate.h Implementation = */

void psaci_cfg_renderer_populate_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    psaci__cfg_renderer_populate_iterable_fields(lua, cfg_out);
}

/* = Internal Implementation = */

/* == Internal Iterable Funcs == */

void psaci__cfg_renderer_iterable_vertex_attribute(PSaciLuaState* lua, SaciU64 idx, void* user_data)
{
    struct PSaciConfigRenderer* cfg = (struct PSaciConfigRenderer*)user_data;
    struct PSaciLuaValue val = {0};
    struct {
        SaciBool has_name, has_type, has_offset, has_location;
        const char* name;
        SaciU64 type, offset;
        SaciU32 location;
    } fields = {0};

    if (psaci_lua_get_value(lua, "name", &val, SACI_TYPE_STRING)) {
        fields.has_name = SACI_TRUE;
        fields.name = val.data.string;
    }
    if (psaci_lua_get_value(lua, "type", &val, SACI_TYPE_DOUBLE)) {
        fields.has_type = SACI_TRUE;
        fields.type = (SaciU64)val.data.number;
    }
    if (psaci_lua_get_value(lua, "offset", &val, SACI_TYPE_DOUBLE)) {
        fields.has_offset = SACI_TRUE;
        fields.offset = (SaciU64)val.data.number;
    }
    if (psaci_lua_get_value(lua, "location", &val, SACI_TYPE_DOUBLE)) {
        fields.has_location = SACI_TRUE;
        fields.location = (SaciU32)val.data.number;
    }

    if (!fields.has_type || !fields.has_location) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER,
                         SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not parse vertex attribute: Missing types (type or location)");
        return;
    }
    if (fields.has_name) {
        strcpy(cfg->vertex_attributes.element_array[idx].name, fields.name);
    }
    // Fields like "type", "offset" and "location" are already set through the default
    // renderer implementation and MIGHT or MIGHT NOT be properly overwritten if
    // the fields.X is intentionally 0, that is the reason we also check here
    if (fields.has_type) {
        cfg->vertex_attributes.element_array[idx].type = (SaciDataType)fields.type;
    }
    if (fields.has_offset) {
        cfg->vertex_attributes.element_array[idx].offset = fields.offset;
    }
    if (fields.has_location) {
        cfg->vertex_attributes.element_array[idx].location = fields.location;
    }
}

/* == Internal Populate Funcs == */

SACI_INTERNAL_CONST struct PSaciCfgArrayIterablePathTable PSACI_G_CFG_RENDERER_POPULATE_TABLE[1] = {
    {"vertex.layout", psaci__cfg_renderer_iterable_vertex_attribute},
};

SACI_INTERNAL void psaci__cfg_renderer_populate_vertex_attributes(PSaciLuaState* lua, struct PSaciRendererCfgVertexAttributes* attribute_out)
{
    if (!saci_bubble_sort_ptr(attribute_out->element_array,
                              sizeof(struct PSaciRendererCfgVertexElement),
                              attribute_out->element_array_length,
                              offsetof(struct PSaciRendererCfgVertexElement, location))) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "NULL vertex attribute array while parsing config file");
    }
    SaciU64 size_total = 0;
    for (SaciU64 i = 0; i < attribute_out->element_array_length; ++i) {
        attribute_out->element_array[i].offset = size_total;
        size_total += SACI_G_TYPE_SIZE_TABLE[attribute_out->element_array[i].type];
    }
    attribute_out->element_size_internal = size_total;
}

SACI_INTERNAL void psaci__cfg_renderer_populate_iterable_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    for (SaciU64 i = 0; i < SACI_ARRLEN_M(PSACI_G_CFG_RENDERER_POPULATE_TABLE); ++i) {
        struct PSaciCfgArrayIterablePathTable entry = PSACI_G_CFG_RENDERER_POPULATE_TABLE[i];
        psaci_lua_array_iter(lua, entry.path, entry.iter, &cfg_out);
    }
}

SACI_INTERNAL void psaci__cfg_renderer_populate_empty_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    psaci__cfg_renderer_populate_vertex_attributes(lua, &cfg_out->vertex_attributes);
}

void psaci__cfg_load_renderer_index_data(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    // if (psaci_lua_push_field_table(lua, "index")) {
    //     cfg_out->index_data.element_size_internal = SACI_G_TYPE_SIZE_TABLE[psaci_lua_get_enum(lua, "element_type")];
    //     psaci_lua_pop(lua, 1);
    // }
}

void psaci__cfg_load_renderer_shaders(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out, SaciMemPool* pool)
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

void psaci__cfg_load_renderer_uniforms(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out, SaciMemPool* pool)
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

void psaci__cfg_load_renderer_samplers(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out, SaciMemPool* pool)
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

void psaci__cfg_load_renderer_batch(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out, SaciMemPool* pool)
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

void psaci__cfg_load_renderer_bound(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    // if (psaci_lua_push_field_table(lua, "bound")) {
    //     cfg_out->bound.index_cfg.capacity = psaci_lua_get_uint64(lua, "index_capacity");
    //     cfg_out->bound.instance_cfg.capacity = psaci_lua_get_uint64(lua, "instance_capacity");
    //     psaci_lua_pop(lua, 1);
    // }
}

void psaci__cfg_load_renderer_draw(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
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

void psaci__cfg_load_renderer_pipeline(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
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
