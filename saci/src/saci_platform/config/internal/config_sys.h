#ifndef SACI_PLATFORM_CONFIG_CONFIG_H
#define SACI_PLATFORM_CONFIG_CONFIG_H

#include "saci_platform/scripting/scripting.h"
#include "saci_platform/scripting/lua.h"

#include "saci_util/types.h"

enum PSaciConfigFieldFlags {
    PSACI_CONFIG_FIELD_FLAG_READONLY = 1 << 0,
    PSACI_CONFIG_FIELD_FLAG_REQUIRED = 1 << 1,
    PSACI_CONFIG_FIELD_FLAG_INTERNAL = 1 << 2,
    PSACI_CONFIG_FIELD_FLAG_IS_ARRAY = 1 << 3,
    PSACI_CONFIG_FIELD_FLAG_MIN = 1 << 4,
    PSACI_CONFIG_FIELD_FLAG_MAX = 1 << 5,
    PSACI_CONFIG_FIELD_FLAG_MAX_STR_SIZE = 1 << 6,
    PSACI_CONFIG_FIELD_FLAG_EXPECTED_VALUE = 1 << 7,
    PSACI_CONFIG_FIELD_FLAG_DEFAULT_VALUE = 1 << 8,
};

struct PSaciConfigValidationConstraints {
    SaciU64 flags;

    SaciDataType expected_type;

    struct PSaciScriptingValue min_value;
    struct PSaciScriptingValue max_value;

    SaciU64 max_str_size;

    struct PSaciScriptingValue default_value;

    SaciU64 allowed_values_count;
    struct PSaciScriptingValue* allowed_values;
};

struct PSaciConfigValidationRule {
    const char* field_name;

    struct PSaciConfigValidationConstraints contraints;
};

struct PSaciConfigSchema {
    const char* table_name;

    struct PSaciConfigValidationConstraints self_constraints;

    SaciU64 validation_rule_count;
    struct PSaciConfigValidationRule* validation_rule_array;

    SaciU64 child_schema_ptr_count;
    const struct PSaciConfigSchema* parent_schema_ptr;
    const struct PSaciConfigSchema** child_schema_ptr_array;
};

struct PSaciConfigSystem {
    struct PSaciScriptingTable root_table;
    const char* root_table_name;

    const struct PSaciConfigSchema* schema_array;
    SaciU64 schema_count;
};

void psaci_config_create_validation_constraints(
    PSaciLuaState* lua,
    struct PSaciConfigValidationConstraints* constraint_out);

void psaci_config_create_validation_rule(
    PSaciLuaState* lua,
    struct PSaciConfigValidationRule* validation_rule_out);

void psaci_config_create_schema(PSaciLuaState* lua,
                                struct PSaciConfigSchema* schema_out);

void psaci_config_create_system(PSaciLuaState* lua,
                                struct PSaciConfigSystem* system_out);

SaciBool psaci_config_get_config_from_system(
    PSaciLuaState* lua,
    const struct PSaciConfigSystem* system,
    struct PSaciScriptingTable* config_root_out);

#endif // SACI_PLATFORM_CONFIG_CONFIG_H
