#ifndef SACI_UTILS_CONFIG_SU_LUA_H
#define SACI_UTILS_CONFIG_SU_LUA_H

#include <lua5.4/lua.h>

#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"

typedef lua_State su_LuaState;

SA_API su_LuaState* su_lua_load(const char* file_path);

SA_API su_Bool su_lua_load_table(su_LuaState* state, const char* table_name);

SA_API su_S8 su_lua_get_bool(su_LuaState* state, const char* b_name);

SA_API su_U8 su_lua_get_uint8(su_LuaState* state, const char* i_name);

SA_API su_U32 su_lua_get_uint32(su_LuaState* state, const char* i_name);

SA_API su_U64 su_lua_get_uint64(su_LuaState* state, const char* i_name);

SA_API su_U64 su_lua_get_array_length(su_LuaState* state);

SA_API const char* su_lua_get_str(su_LuaState* state, const char* s_name);

SA_API su_S64 su_lua_get_enum(su_LuaState* state, const char* e_name);

SA_API su_Bool su_lua_push_global_table(su_LuaState* state, const char* table_name);

SA_API su_Bool su_lua_push_field_table(su_LuaState* state, const char* field_name);

SA_API su_U64 su_lua_push_field_array(su_LuaState* state, const char* array_name);

SA_API su_U64 su_lua_push_array_entry(su_LuaState* state, const su_U64 index);

SA_API void su_lua_pop(su_LuaState* state, int count);

SA_API void su_lua_close(su_LuaState* state);

SA_API void su_lua_dump_stack(su_LuaState* l);

#endif // SACI_UTILS_CONFIG_SU_LUA_H
