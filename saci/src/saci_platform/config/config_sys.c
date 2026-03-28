#include "saci_platform/config/internal/config_sys.h"

#include "saci_platform/scripting/scripting.h"
#include "saci_platform/scripting/lua.h"

#include "saci_util/internal/log.h"
#include "saci_util/log.h"
#include "saci_util/types.h"
#include "saci_util/defines.h"
#include "saci_util/memory.h"
#include "saci_util/internal/general.h"
#include <string.h>

/* ===  GLOBALS === */

SACI_STATIC struct {
    SaciBool is_loaded;
    struct SaciMemPool* config_sys_pool; // Used for allowed values
} psaci_g_config_sys_context = {0};

/* === Opaque structures === */

struct PSaciConfigItem {
    SaciMemChunk* chunk;
    struct PSaciScriptingTable root_table;
    struct PSaciConfigSystem* system;
    const char* config_name;
};

struct PSaciConfigSchemaInfo {
    struct PSaciConfigSchema* schema;
    SaciU64 validation_rule_count;
    SaciU64 child_schema_ptr_array_count;
};

/* === Internal === */

SACI_INTERNAL SaciBool psaci__config_table_is_subschema(PSaciLuaState* lua, const struct PSaciScriptingTable* val);

SACI_INTERNAL void psaci__config_constraint_set_if_exists(
    PSaciLuaState* lua,
    struct PSaciConfigValidationConstraints* constraint_out);

SACI_INTERNAL void psaci__config_validation_rule_iterator(PSaciLuaState* lua, SaciU64 idx, struct PSaciScriptingField field, void* user_data);

/* === Config System Implementation === */

void psaci_config_system_init(void)
{
    if (psaci_g_config_sys_context.is_loaded) {
        return;
    }
    psaci_g_config_sys_context.is_loaded = true;
    psaci_g_config_sys_context.config_sys_pool =
        saci_mem_create_pool(SACI_MEM_CONTEXT_CONFIG, 1000 * 10); // TODO set the proper size
}

void psaci_config_create_validation_constraints(
    PSaciLuaState* lua,
    struct PSaciConfigValidationConstraints* constraint_out)
{
    SACI_LOG_ASSERT_M(lua, SACI_LOG_CONTEXT_CORE_CONFIG, "Lua context not present");
    if (!constraint_out) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not create validation constraint, "
                         "constraint_out ptr is null");
        return;
    }

    psaci__config_constraint_set_if_exists(lua, constraint_out);
}

void psaci_config_create_validation_rule(
    PSaciLuaState* lua,
    struct PSaciConfigValidationRule* validation_rule_out)
{
    SACI_LOG_ASSERT_M(lua, SACI_LOG_CONTEXT_CORE_CONFIG, "Lua context not present");
    if (!validation_rule_out) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not create validation rule, "
                         "validation_rule_out ptr is null");
        return;
    }
    struct PSaciConfigValidationRule rule = *validation_rule_out;

    psaci_config_create_validation_constraints(lua, &rule.contraints);
}

void psaci_config_create_schema(PSaciLuaState* lua,
                                struct PSaciConfigSchema* schema_out)
{
    SACI_LOG_ASSERT_M(lua, SACI_LOG_CONTEXT_CORE_CONFIG, "Lua context not present");
    if (!schema_out) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not create schema, schema_out ptr is null");
        return;
    }

    if (schema_out->parent_schema_ptr) {
        // Inherit the constraints of the parent schema
        schema_out->self_constraints = schema_out->parent_schema_ptr->self_constraints;
    }
    // Check if the constraints vary from the parent schema
    psaci_config_create_validation_constraints(lua, &schema_out->self_constraints);
    // Loop to get all validation rules or nested schemas
    struct PSaciConfigSchemaInfo schema_info = {
        .schema = schema_out,
        .child_schema_ptr_array_count = schema_out->child_schema.ptr_array_count,
        .validation_rule_count = schema_out->validation_rule.count,
    };
    psaci_lua_table_iter(lua, NULL, psaci__config_validation_rule_iterator, &schema_info);
}

void psaci_config_create_system(PSaciLuaState* lua,
                                struct PSaciConfigSystem* system_out)
{
    SACI_LOG_ASSERT_M(lua, SACI_LOG_CONTEXT_CORE_CONFIG, "Lua context not present");
    if (!system_out) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not create system, system_out ptr is NULL");
        return;
    }
    // for table in root_tables
    // psaci_config_create_schema
}

const struct PSaciConfigItem* psaci_config_get_config_from_system(
    PSaciLuaState* lua,
    const struct PSaciConfigSystem* system,
    const char* config_name)
{
    SACI_LOG_DUMMY_CHECK_M(lua && system, SACI_LOG_CONTEXT_CORE_CONFIG,
                           "Lua state, config system or config root ptr is null");
    return NULL;
}

const void* psaci_config_get_field(const struct PSaciConfigItem* config_item,
                                   const char* field_name)
{
}

SaciBool psaci_config_set_field(const struct PSaciConfigItem* config_item,
                                const char* field_name, const void* data)
{
}

/* === Internal === */

SACI_INTERNAL void psaci__config_constraint_set_allowed_values(
    PSaciLuaState* lua,
    struct PSaciConfigValidationConstraints* constraint_out)
{
    constraint_out->allowed_values_count = 0;
    constraint_out->allowed_values = NULL;

    SaciDataType type;
    if (!psaci_lua_query_field_type(lua, "allowed_values", &type)) {
        return;
    }

    struct PSaciScriptingValue val = {0};
    if (!psaci_lua_get_value(lua, "allowed_values", &val,
                             SACI_TYPE_UNKNOWN)) {
        // Shouldn't happen since query field was true, but this check won't
        // affect performance all that much
        return;
    }

    const SaciU64 alloc_size = sizeof(struct PSaciScriptingValue);
    constraint_out->allowed_values =
        saci_mem_pool_alloc(psaci_g_config_sys_context.config_sys_pool,
                            alloc_size);
    saci_mem_safe_copy(constraint_out->allowed_values,
                       alloc_size, 0,
                       &val, sizeof(val), 0,
                       alloc_size);
}

SACI_INTERNAL void psaci__config_constraint_set_if_exists(
    PSaciLuaState* lua,
    struct PSaciConfigValidationConstraints* constraint_out)
{
    struct PSaciScriptingValue val = {0};
    struct {
        const char* constraint_name;
        SaciDataType constraint_type;
        void* data_ptr;
    } constraint_object_array[] = {
        {
            .constraint_name = "expected_type",
            .constraint_type = SACI_TYPE_S64,
            .data_ptr = &constraint_out->expected_type,
        },
        {
            .constraint_name = "min_value",
            .constraint_type = SACI_TYPE_S64,
            .data_ptr = &constraint_out->min_value,
        },
        {
            .constraint_name = "max_value",
            .constraint_type = SACI_TYPE_S64,
            .data_ptr = &constraint_out->max_value,
        },
        {
            .constraint_name = "max_str_size",
            .constraint_type = SACI_TYPE_U64,
            .data_ptr = &constraint_out->max_str_size,
        },
        {
            .constraint_name = "default_value",
            .constraint_type = SACI_TYPE_UNKNOWN, // Unkown at compiletime,
                                                  // known in runtime
            .data_ptr = &constraint_out->expected_type,
        },
        // "Allowed values" works differently
    };

    for (SaciU64 i = 0; i < SACI_ARRLEN_M(constraint_object_array); ++i) {
        SaciBool found = psaci_lua_get_value(
            lua,
            constraint_object_array[i].constraint_name,
            &val, constraint_object_array[i].constraint_type);
        if (found) {
            // constraint_object_array[i].data_ptr = val.data;
        }
    }

    psaci__config_constraint_set_allowed_values(lua, constraint_out);
}

void psaci__config_validation_rule_iterator(
    PSaciLuaState* lua,
    SaciU64 idx,
    struct PSaciScriptingField field,
    void* user_data)
{
    (void)idx;
    const char constraints[6][36] = {
        "expected_type",
        "min_value",
        "max_value",
        "max_str_size",
        "default_value",
        "allowed_values",
    };

    struct PSaciConfigSchemaInfo* schema_info = (struct PSaciConfigSchemaInfo*)user_data;

    for (SaciU64 i = 0; i < SACI_ARRLEN_M(constraints); ++i) {
        if (strcmp(field.name, constraints[i])) {
            // Already checked through `psaci__config_constraint_set_if_exists`
            return;
        }
    }

    if (field.value.type == SACI_TYPE_STRUCT) {
        if (psaci__config_table_is_subschema(lua, field.value.data.table)) {
            // Re iterate inside the subsystem
            struct PSaciConfigSchemaInfo new_info = {
                .child_schema_ptr_array_count = 0,
                .validation_rule_count = 0,
                .schema = NULL, // TODO Should crash, but test to se if it reaches here first, then solve the crash
            };
            SaciU64 child_index = schema_info->child_schema_ptr_array_count;
            psaci_config_create_schema(
                lua,
                schema_info->schema->child_schema.ptr_array[child_index]);

            schema_info->schema
                ->child_schema.ptr_array[child_index]
                ->parent_schema_ptr = schema_info->schema;

            schema_info->child_schema_ptr_array_count++;
            return;
        }
    }

    schema_info->schema->validation_rule.array[idx].field_name = field.name;
    psaci_config_create_validation_rule(lua,
                                        &schema_info
                                             ->schema
                                             ->validation_rule.array[idx]);
}

SaciBool psaci__config_table_is_subschema(PSaciLuaState* lua, const struct PSaciScriptingTable* val)
{
    const char constraints[6][36] = {
        "expected_type",
        "min_value",
        "max_value",
        "max_str_size",
        "default_value",
        "allowed_values",
    };

    for (SaciU64 i = 0; i < val->field_count; ++i) {
        struct PSaciScriptingField field = val->field_array[i];
        SaciBool found_non_constraint = SACI_TRUE;

        // If non are equal to constraints, then it found a non-constraint,
        // returns true
        for (SaciU64 j = 0; j < SACI_ARRLEN_M(constraints); ++j) {
            if (strcmp(field.name, constraints[j])) {
                found_non_constraint = SACI_FALSE;
                // if a saci key, return
                continue;
            }
        }
        if (found_non_constraint) {
            return SACI_TRUE;
        }
    }
    return SACI_FALSE;
}
