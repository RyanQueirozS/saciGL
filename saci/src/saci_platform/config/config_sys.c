#include "saci_platform/config/internal/config_sys.h"

#include "saci_platform/scripting/scripting.h"
#include "saci_platform/scripting/lua.h"

#include "saci_util/internal/log.h"
#include "saci_util/log.h"
#include "saci_util/types.h"
#include "saci_util/defines.h"
#include "saci_util/internal/general.h"

/* === Internal === */

typedef void (*PSaciCheckFlagFunc)(PSaciLuaState*, SaciU64* flag_out);

const struct PSaciLuaConfigFieldConstraintCheck {
    const char* const FIELD_FLAG_NAME;
    const SaciU64 FIELD_FLAG_BIT;
    const SaciDataType TYPE;
    const void* const EXPECTED_VALUE;
} PSACI_G_CONFIG_FIELD_CONSTRAINT_CHECKS[] = {
    {
        .FIELD_FLAG_NAME = "readonly",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_READONLY,
        .TYPE = SACI_TYPE_BOOL,
    },
    {
        .FIELD_FLAG_NAME = "required",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_REQUIRED,
        .TYPE = SACI_TYPE_BOOL,
    },
    {
        .FIELD_FLAG_NAME = "internal",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_INTERNAL,
        .TYPE = SACI_TYPE_BOOL,
    },
    {
        .FIELD_FLAG_NAME = "is_array",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_IS_ARRAY,
        .TYPE = SACI_TYPE_BOOL,
    },
    {
        .FIELD_FLAG_NAME = "min",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_MIN,
        .TYPE = SACI_TYPE_U64,
    },
    {
        .FIELD_FLAG_NAME = "max",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_MAX,
        .TYPE = SACI_TYPE_U64,
    },
    {
        .FIELD_FLAG_NAME = "max_str_size",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_MAX_STR_SIZE,
        .TYPE = SACI_TYPE_U64,
    },
    {
        .FIELD_FLAG_NAME = "expected_value",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_EXPECTED_VALUE,
        .TYPE = SACI_TYPE_VOIDPTR,
    },
    {
        .FIELD_FLAG_NAME = "default_value",
        .FIELD_FLAG_BIT = PSACI_CONFIG_FIELD_FLAG_DEFAULT_VALUE,
        .TYPE = SACI_TYPE_VOIDPTR,
    },
};

SACI_INTERNAL void psaci__config_push_constraint_if_exists(
    PSaciLuaState*,
    SaciU64* flag_out,
    struct PSaciLuaConfigFieldConstraintCheck check);

/* === Config System Implementation === */

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
    struct PSaciConfigValidationConstraints constraint = *constraint_out;

    for (SaciU64 i = 0; i < SACI_ARRLEN_M(PSACI_G_CONFIG_FIELD_CONSTRAINT_CHECKS); ++i) {
        psaci__config_push_constraint_if_exists(
            lua, &constraint.flags,
            PSACI_G_CONFIG_FIELD_CONSTRAINT_CHECKS[i]);
    }
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

    struct PSaciConfigSchema schema = *schema_out;
    if (schema.parent_schema_ptr) {
        // Inherit the constraints of the parent schema
        schema.self_constraints = schema.parent_schema_ptr->self_constraints;
    }
    psaci_config_create_validation_constraints(lua, &schema.self_constraints);
    // Loop to get all validation rules
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

SaciBool psaci_config_get_config_from_system(
    PSaciLuaState* lua,
    const struct PSaciConfigSystem* system,
    struct PSaciScriptingTable* config_root_out)
{
    SACI_LOG_DUMMY_CHECK_M(lua && system && config_root_out, SACI_LOG_CONTEXT_CORE_CONFIG,
                           "Lua state, config system or config root ptr is null");
    return SACI_TRUE;
}

/* === Internal === */

SACI_INTERNAL void psaci__config_push_constraint_if_exists(
    PSaciLuaState* lua,
    SaciU64* flag_out,
    struct PSaciLuaConfigFieldConstraintCheck check)
{
}
