#ifndef SACI_PLATFORM_CONFIG_LUA_H
#define SACI_PLATFORM_CONFIG_LUA_H

#include "saci_platform/scripting/scripting.h"

#include "saci_util/defines.h"
#include "saci_util/types.h"

#include <lua5.4/lua.h>

/* === Lua CFG === */

typedef lua_CFunction PSaciLuaCFunc;

typedef lua_State PSaciLuaState;

typedef void (*PSaciLuaArrayIter)(PSaciLuaState* lua, SaciU64 idx, struct PSaciScriptingField field, void* user_data);

struct PSaciLuaRegFunc {
    PSaciLuaCFunc c_func;

    const char* func_name;
    const char** nested_table_array;
    SaciU64 nested_table_count;

    const char* root_table; // optional, default = "saci"
};

struct PSaciLuaModule {
    const char* name;

    const unsigned char* bytecode;
    SaciU64 size;
};

SACI_API PSaciLuaState* psaci_lua_load(const char* file_path);

SACI_API void psaci_lua_init_modules(const struct PSaciLuaModule* module_array,
                                     SaciU64 module_count);

SACI_API void psaci_lua_register_modules(lua_State* lua);

SACI_API void psaci_lua_close(PSaciLuaState* state);

SACI_API void psaci_lua_dump_stack(PSaciLuaState* l);

SACI_API void psaci_lua_clear_stack(PSaciLuaState* lua);

SACI_API void psaci_lua_pop(PSaciLuaState* lua, int pops);

SACI_API SaciBool psaci_lua_push_to_stack(PSaciLuaState* lua, const char* path_to_value);

SACI_API SaciBool psaci_lua_push_array_entry_to_stack(PSaciLuaState* lua, const SaciU64 idx);

SACI_API SaciBool psaci_lua_table_iter(PSaciLuaState* lua, const char* path_to_array, PSaciLuaArrayIter array_iter, void* user_data);

// Already pops the data for the "path_to_value"
SACI_API SaciBool psaci_lua_get_value(PSaciLuaState* lua, const char* path_to_value, struct PSaciScriptingValue* value_out, const SaciDataType expected_type);

SACI_API SaciBool psaci_lua_get_field(PSaciLuaState* lua, const char* path_to_field, struct PSaciScriptingField* field_out, const SaciDataType expected_type);

SACI_API SaciBool psaci_lua_has_value(PSaciLuaState* lua, const char* path_to_value, const SaciDataType expected_type);

SACI_API SaciU64 psaci_lua_get_key_value_count_in_stack(PSaciLuaState* lua);

SACI_API SaciU64 psaci_lua_get_key_value_count(PSaciLuaState* lua, const char* path_to_table);

SACI_API SaciBool psaci_lua_query_field_type(PSaciLuaState* lua, const char* path_to_value, SaciDataType* type_out);

SACI_API void psaci_lua_register_cfunc(PSaciLuaState* lua,
                                       struct PSaciLuaRegFunc reg);

#endif // SACI_PLATFORM_CONFIG_LUA_H
