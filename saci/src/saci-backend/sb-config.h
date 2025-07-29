#ifndef SACI_BACKEND_CONFIG_H
#define SACI_BACKEND_CONFIG_H

#include <lua5.4/lua.h>

#include "saci-utils/su-general.h"
#include "saci-utils/su-types.h"

typedef lua_State sb_ConfigState;

SA_API sb_ConfigState* sb_config_load(const char* file_path);

SA_API su_Bool sb_config_load_table(sb_ConfigState* state, const char* table_name);

SA_API su_S8 sb_config_get_bool(sb_ConfigState* state, const char* b_name);

SA_API su_U8 sb_config_get_uint8(sb_ConfigState* state, const char* i_name);

SA_API su_U32 sb_config_get_uint32(sb_ConfigState* state, const char* i_name);

SA_API su_U64 sb_config_get_uint64(sb_ConfigState* state, const char* i_name);

SA_API const char* sb_config_get_str(sb_ConfigState* state, const char* s_name);

SA_API void sb_config_close(sb_ConfigState* state);

#endif // SACI_BACKEND_CONFIG_H
