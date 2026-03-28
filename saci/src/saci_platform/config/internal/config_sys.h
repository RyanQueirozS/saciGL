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
    PSACI_CONFIG_FIELD_FLAG_ALLOWED_VALUE = 1 << 7,
    PSACI_CONFIG_FIELD_FLAG_DEFAULT_VALUE = 1 << 8,
};

struct PSaciConfigValidationConstraints {
    SaciU64 flags;

    SaciDataType expected_type;

    SaciS64 min_value;
    SaciS64 max_value;

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

    struct {
        SaciU64 count;
        struct PSaciConfigValidationRule* array;
    } validation_rule;

    struct {
        SaciU64 ptr_array_count;
        struct PSaciConfigSchema** ptr_array;
    } child_schema;
    const struct PSaciConfigSchema* parent_schema_ptr;
};

struct PSaciConfigSystem {
    struct PSaciScriptingTable root_table;
    const char* root_table_name;

    const struct PSaciConfigSchema* schema_array;
    SaciU64 schema_count;
};

// Item generated through a system. ConfigItem is the config itself
struct PSaciConfigItem;

void psaci_config_system_init(void);

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

const struct PSaciConfigItem* psaci_config_get_config_from_system(
    PSaciLuaState* lua,
    const struct PSaciConfigSystem* system,
    const char* config_name);

// Fields must be passed as "table1.table2.field"
const void* psaci_config_get_field(const struct PSaciConfigItem* config_item,
                                   const char* field_name);

SaciBool psaci_config_set_field(const struct PSaciConfigItem* config_item,
                                const char* field_name, const void* data);

#endif // SACI_PLATFORM_CONFIG_CONFIG_H
