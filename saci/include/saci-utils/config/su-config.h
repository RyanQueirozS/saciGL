#ifndef SACI_UTILS_CONFIG_SU_CONFIG_H
#define SACI_UTILS_CONFIG_SU_CONFIG_H

#include <lua5.4/lua.h>

#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"

typedef lua_State su_ConfigState;

SA_API su_ConfigState* su_config_load(const char* file_path);

SA_API su_Bool su_config_load_table(su_ConfigState* state, const char* table_name);

SA_API su_S8 su_config_get_bool(su_ConfigState* state, const char* b_name);

SA_API su_U8 su_config_get_uint8(su_ConfigState* state, const char* i_name);

SA_API su_U32 su_config_get_uint32(su_ConfigState* state, const char* i_name);

SA_API su_U64 su_config_get_uint64(su_ConfigState* state, const char* i_name);

SA_API su_U64 su_config_get_array_length(su_ConfigState* state);

SA_API char* su_config_get_str(su_ConfigState* state, const char* s_name);

SA_API su_S64 su_config_get_enum(su_ConfigState* state, const char* e_name);

SA_API su_Bool su_config_push_global_table(su_ConfigState* state, const char* table_name);

SA_API su_Bool su_config_push_field_table(su_ConfigState* state, const char* field_name);

SA_API su_U64 su_config_push_field_array(su_ConfigState* state, const char* array_name);

SA_API su_U64 su_config_push_array_entry(su_ConfigState* state, const su_U64 index);

SA_API void su_config_pop(su_ConfigState* state, int count);

SA_API void su_config_close(su_ConfigState* state);

#endif // SACI_UTILS_CONFIG_SU_CONFIG_H
