#ifndef SACI_UTILS_CONFIG_SU_CONFIG_H
#define SACI_UTILS_CONFIG_SU_CONFIG_H

#include <lua5.4/lua.h>

#include "saci_util/types.h"

#include "saci_util/defines.h"

// Lua

typedef lua_State PSaciLuaState;

SACI_API PSaciLuaState* psaci_lua_load(const char* file_path);

SACI_API SaciBool psaci_lua_load_table(PSaciLuaState* state, const char* table_name);

SACI_API SaciS8 psaci_lua_get_bool(PSaciLuaState* state, const char* b_name);

SACI_API SaciU8 psaci_lua_get_uint8(PSaciLuaState* state, const char* i_name);

SACI_API SaciU32 psaci_lua_get_uint32(PSaciLuaState* state, const char* i_name);

SACI_API SaciU64 psaci_lua_get_uint64(PSaciLuaState* state, const char* i_name);

SACI_API SaciU64 psaci_lua_get_array_length(PSaciLuaState* state);

SACI_API const char* psaci_lua_get_str(PSaciLuaState* state, const char* s_name);

SACI_API SaciS64 psaci_lua_get_enum(PSaciLuaState* state, const char* e_name);

SACI_API SaciBool psaci_lua_push_global_table(PSaciLuaState* state, const char* table_name);

SACI_API SaciBool psaci_lua_push_field_table(PSaciLuaState* state, const char* field_name);

SACI_API SaciU64 psaci_lua_push_field_array(PSaciLuaState* state, const char* array_name);

SACI_API SaciU64 psaci_lua_push_array_entry(PSaciLuaState* state, const SaciU64 index);

SACI_API void psaci_lua_pop(PSaciLuaState* state, int count);

SACI_API void psaci_lua_close(PSaciLuaState* state);

SACI_API void psaci_lua_dump_stack(PSaciLuaState* l);

#endif // SACI_UTILS_CONFIG_SU_CONFIG_H
