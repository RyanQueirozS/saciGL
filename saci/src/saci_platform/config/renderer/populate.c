// TODO remove unecessary checks for name when building for prod
// TODO log whenever `name` exists but the required fields are not
// TODO find a way to check extra fields that shouldn't be on that config table
#include "saci_platform/config/config.h"

#include "saci_platform/config/internal/prealloc.h"
#include "saci_platform/config/internal/lua.h"

#include "saci_util/defines.h"
#include "saci_util/internal/general.h"
#include "saci_util/internal/log.h"

#include <saci_platform/gfx/internal/gfx.h>
#include <saci_util/internal/sorting_algorithms.h>
#include <saci_util/log.h>
#include <saci_util/memory.h>
#include <saci_util/types.h>
#include <string.h>

/* = Internal = */

/* == Iterable Func Helpers == */

// Used in `psaci__cfg_renderer_iterable_instance_buffer`
SACI_INTERNAL void psaci__cfg_renderer_iterable_validator_instance_buffer(PSaciLuaState*, SaciU64 idx, void* user_data);
/* == Iterable Funcs == */

SACI_INTERNAL void psaci__cfg_renderer_iterable_vertex_attribute(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_uniform_data(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_sampler_data(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_instance_buffer(PSaciLuaState*, SaciU64 idx, void* user_data);
SACI_INTERNAL void psaci__cfg_renderer_iterable_instance_buffer_layout(PSaciLuaState*, SaciU64 idx, void* user_data);

/* == Populate Funcs == */

SACI_INTERNAL void psaci__cfg_renderer_populate_vertex_attributes(struct PSaciRendererCfgVertexAttributes* attribute_out);

SACI_INTERNAL void psaci__cfg_renderer_populate_iterable_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out);
SACI_INTERNAL void psaci__cfg_renderer_populate_empty_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out);

/* = renderer_populate.h Implementation = */

void psaci_cfg_renderer_populate_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    psaci__cfg_renderer_populate_iterable_fields(lua, cfg_out);
}

/* = Internal Implementation = */

/* == Internal Iterable Func Helpers == */

void psaci__cfg_renderer_iterable_validator_instance_buffer(PSaciLuaState* lua, SaciU64 idx, void* user_data)
{
    SaciBool* is_valid = (SaciBool*)user_data;

    SaciBool has_name = !psaci_lua_has_value(lua, "name", PSACI_LUA_TYPE_STRING);
    SaciBool has_required_fields =
        !psaci_lua_has_value(lua, "type", PSACI_LUA_TYPE_NUMBER) ||
        !psaci_lua_has_value(lua, "location", PSACI_LUA_TYPE_NUMBER);

    if (has_name && !has_required_fields) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_LOW,
                          SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Instance buffer layout " SACI_FMTU64 " has name but it does not have"
                          " all required fields (type, location)",
                          idx);
        *is_valid = SACI_FALSE;
    }
    *is_valid = SACI_TRUE;
}

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

    if (psaci_lua_get_value(lua, "name", &val, PSACI_LUA_TYPE_STRING)) {
        fields.has_name = SACI_TRUE;
        fields.name = val.data.string;
    }
    if (psaci_lua_get_value(lua, "type", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.has_type = SACI_TRUE;
        fields.type = (SaciU64)val.data.number;
    }
    if (psaci_lua_get_value(lua, "offset", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.has_offset = SACI_TRUE;
        fields.offset = (SaciU64)val.data.number;
    }
    if (psaci_lua_get_value(lua, "location", &val, PSACI_LUA_TYPE_NUMBER)) {
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

void psaci__cfg_renderer_iterable_uniform_data(PSaciLuaState* lua, SaciU64 idx, void* user_data)
{
    struct PSaciConfigRenderer* cfg = (struct PSaciConfigRenderer*)user_data;
    struct PSaciLuaValue val = {0};
    struct {
        SaciBool has_type, has_name;
        char* name;
        SaciDataType type;
    } fields;

    if (psaci_lua_get_value(lua, "type", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.type = (SaciDataType)val.data.number;
        fields.has_type = SACI_TRUE;
    }
    if (psaci_lua_get_value(lua, "name", &val, PSACI_LUA_TYPE_STRING)) {
        strcpy(fields.name, val.data.string);
        fields.has_name = SACI_TRUE;
    }
    if (!fields.has_name || !fields.has_type) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER,
                         SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not parse uniforms: Missing types (type or name)");
    }

    // Fields to cfg
    if (fields.has_name) {
        strcpy(cfg->uniform_array[idx].name, fields.name);
    }
    if (fields.has_type) {
        cfg->uniform_array[idx].type = fields.type;
    }
}

void psaci__cfg_renderer_iterable_instance_buffer(PSaciLuaState* lua, SaciU64 idx, void* user_data)
{
    struct PSaciConfigRenderer* cfg = (struct PSaciConfigRenderer*)user_data;
    struct PSaciLuaValue val = {0};
    SaciBool has_all_valid_layouts = SACI_FALSE;

    // Checks if buffer has all layouts valid
    psaci_lua_array_iter(lua, "layout", psaci__cfg_renderer_iterable_validator_instance_buffer, &has_all_valid_layouts);
    if (!has_all_valid_layouts) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER,
                         SACI_LOG_ERROR_SEVERITY_MEDIUM,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Invalid config layout, could not populate instance"
                         " buffer data");
        return;
    }

    if (psaci_lua_get_value(lua, "name", &val, PSACI_LUA_TYPE_STRING)) {
        strcpy(cfg->instance_data.buffer_array[idx].name, val.data.string);
    }
    psaci_lua_array_iter(lua, "layout", psaci__cfg_renderer_iterable_instance_buffer_layout, &(cfg->instance_data.buffer_array[idx]));
}

SACI_INTERNAL void psaci__cfg_renderer_iterable_instance_buffer_layout(PSaciLuaState* lua, SaciU64 idx, void* user_data)
{
    struct PSaciRendererCfgInstanceBuffer* instance_buffer = (struct PSaciRendererCfgInstanceBuffer*)user_data;
    struct PSaciLuaValue val = {0};
    struct {
        SaciBool has_type, has_name, has_location;
        char* name;
        SaciDataType type;
        SaciU32 location;
    } fields;

    if (psaci_lua_get_value(lua, "name", &val, PSACI_LUA_TYPE_STRING)) {
        fields.has_name = SACI_TRUE;
        fields.name = val.data.string;
    }
    if (psaci_lua_get_value(lua, "type", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.has_type = SACI_TRUE;
        fields.type = (SaciDataType)val.data.number;
    }
    if (psaci_lua_get_value(lua, "location", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.has_location = SACI_TRUE;
        fields.location = (SaciU32)val.data.number;
    }
    if (!fields.has_type && !fields.has_location) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER,
                         SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG, "Missing fields (type or location)");
    }
    if (fields.has_name) {
        strcpy(instance_buffer->layout_array[idx].name, fields.name);
    }
    if (fields.has_location) {
        instance_buffer->layout_array[idx].location = fields.location;
    }
    if (fields.has_type) {
        instance_buffer->layout_array[idx].type = fields.type;
    }
}

void psaci__cfg_renderer_iterable_sampler_data(PSaciLuaState* lua, SaciU64 idx, void* user_data)
{
    struct PSaciConfigRenderer* cfg = (struct PSaciConfigRenderer*)user_data;
    struct PSaciLuaValue val = {0};
    struct {
        SaciBool has_type, has_name, has_binding, has_unit;
        char* name;
        SaciDataType type;
        SaciS32 binding;
        SaciU64 unit;
    } fields;

    if (psaci_lua_get_value(lua, "name", &val, PSACI_LUA_TYPE_NUMBER)) {
        strcpy(fields.name, val.data.string);
        fields.has_name = SACI_TRUE;
    }
    if (psaci_lua_get_value(lua, "type", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.type = (SaciDataType)val.data.number;
        fields.has_type = SACI_TRUE;
    }
    if (psaci_lua_get_value(lua, "binding", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.binding = (SaciS32)val.data.number;
        fields.has_binding = SACI_TRUE;
    }
    if (psaci_lua_get_value(lua, "unit", &val, PSACI_LUA_TYPE_NUMBER)) {
        fields.unit = (SaciU64)val.data.number;
        fields.has_unit = SACI_TRUE;
    }
    if (!fields.has_type || !fields.has_unit || !fields.has_binding) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER,
                         SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not parse samplers: Missing types (type, unit or binding)");
    }

    struct PSaciRendererCfgSampler* sampler = &(cfg->sampler_array[idx]);
    if (fields.has_name) {
        strcpy(sampler->name, fields.name);
    }
    if (fields.has_type) {
        sampler->type = fields.type;
    }
    if (fields.has_binding) {
        sampler->binding = fields.binding;
    }
    if (fields.has_unit) {
        sampler->unit = fields.unit;
    }
}

/* == Internal Populate Funcs == */

SACI_INTERNAL_CONST struct PSaciCfgArrayIterablePathTable PSACI_G_CFG_RENDERER_POPULATE_ITERABLE_TABLE[3] = {
    {"vertex.layout", psaci__cfg_renderer_iterable_vertex_attribute},
    {"uniforms", psaci__cfg_renderer_iterable_uniform_data},
    {"batch.instances.buffers", psaci__cfg_renderer_iterable_instance_buffer},
};

SACI_INTERNAL void psaci__cfg_renderer_populate_vertex_attributes(struct PSaciRendererCfgVertexAttributes* attribute_out)
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
    for (SaciU64 i = 0; i < SACI_ARRLEN_M(PSACI_G_CFG_RENDERER_POPULATE_ITERABLE_TABLE); ++i) {
        struct PSaciCfgArrayIterablePathTable entry = PSACI_G_CFG_RENDERER_POPULATE_ITERABLE_TABLE[i];
        psaci_lua_array_iter(lua, entry.path, entry.iter, &cfg_out);
    }
}

SACI_INTERNAL void psaci__cfg_renderer_populate_empty_fields(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    psaci__cfg_renderer_populate_vertex_attributes(&cfg_out->vertex_attributes);
}

SACI_INTERNAL void psaci__cfg_renderer_populate_index_data(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    struct PSaciLuaValue element_type;
    if (psaci_lua_get_value(lua, "index.element_type", &element_type, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->index_data.element_size_internal = (SaciU64)element_type.data.number;
    }
}

SACI_INTERNAL void psaci__cfg_renderer_populate_shaders(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    struct PSaciLuaValue shader_code = {0};
    if (psaci_lua_get_value(lua, "shaders.frag", &shader_code, PSACI_LUA_TYPE_STRING)) {
        strcpy(cfg_out->shaders.frag, shader_code.data.string);
    }
    if (psaci_lua_get_value(lua, "shaders.vert", &shader_code, PSACI_LUA_TYPE_STRING)) {
        strcpy(cfg_out->shaders.vert, shader_code.data.string);
    }
    if (psaci_lua_get_value(lua, "shaders.geom", &shader_code, PSACI_LUA_TYPE_STRING)) {
        strcpy(cfg_out->shaders.geom, shader_code.data.string);
    }
}

SACI_INTERNAL void psaci__cfg_renderer_populate_batch(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out, SaciMemPool* pool)
{
    if (!psaci_lua_push_to_stack(lua, "batch")) {
        return;
    }

    struct PSaciLuaValue value = {0};
    if (psaci_lua_get_value(lua, "capacity", &value, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->batch.capacity = (SaciU64)value.data.number;
    }
    if (psaci_lua_get_value(lua, "index.capacity", &value, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->batch.index_cfg.capacity = (SaciU64)value.data.number;
    }
    if (psaci_lua_get_value(lua, "vertex.capacity", &value, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->batch.vertex_cfg.capacity = (SaciU64)value.data.number;
    }
    if (psaci_lua_get_value(lua, "instances.capacity", &value, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->batch.instance_cfg.capacity = (SaciU64)value.data.number;
    }
}

void psaci__cfg_renderer_populate_bound(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    struct PSaciLuaValue val = {0};
    if (psaci_lua_get_value(lua, "bound.index_capacity", &val, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->bound.index_cfg.capacity = (SaciU64)val.data.number;
    }
    if (psaci_lua_get_value(lua, "bound.instance_capacity", &val, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->bound.instance_cfg.capacity = (SaciU64)val.data.number;
    }
}

void psaci__cfg_renderer_populate_draw(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    struct PSaciLuaValue val = {0};
    if (psaci_lua_get_value(lua, "draw.primitives", &val, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->draw.primitive = (enum PSaciRendererPrimitives)val.data.number;
    }
    if (psaci_lua_get_value(lua, "draw.cull_mode", &val, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->draw.cull_mode = (enum PSaciRendererCullMode)val.data.number;
    }
    if (psaci_lua_get_value(lua, "draw.front_face", &val, PSACI_LUA_TYPE_NUMBER)) {
        cfg_out->draw.front_face = (enum PSaciRendererFrontFace)val.data.number;
    }
}

void psaci__cfg_renderer_populate_pipeline(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out)
{
    struct PSaciLuaValue val = {0};
    if (psaci_lua_get_value(lua, "pipeline.depth_test", &val, PSACI_LUA_TYPE_BOOLEAN)) {
        cfg_out->pipeline.depth_test = val.data.boolean;
    }

    if (psaci_lua_get_value(lua, "pipeline.blend.enabled", &val, PSACI_LUA_TYPE_BOOLEAN)) {
        cfg_out->pipeline.blend.enabled = val.data.boolean;
    }
}
