#ifndef SACI_PLATFORM_CONFIG_LUA_H
#define SACI_PLATFORM_CONFIG_LUA_H

#include "saci_util/defines.h"
#include "saci_util/memory.h"
#include "saci_util/types.h"

#include <lua5.4/lua.h>

/* === Lua CFG === */

struct PSaciLuaField {
    SaciDataType type;
    union PSaciLuaValueData {
        double float_val;
        SaciBool bool_val;
        SaciS64 int_val;
        SaciMemChunk* string_val;
        struct PSaciLuaTable* table;
        void* voidptr_val;
        int function_ref;
        int thread_ref;
    } data;
};

struct PSaciLuaTable {
    SaciMemChunk* chunk;
    struct PSaciLuaFieldName {
        struct PSaciLuaField value;
        const char* name;
    }* value_array;
    SaciU64 value_count;
};

typedef lua_State PSaciLuaState;

typedef void (*PSaciLuaArrayIter)(PSaciLuaState* lua, SaciU64 idx, void* user_data);

SACI_API PSaciLuaState* psaci_lua_load(const char* file_path);

SACI_API void psaci_lua_close(PSaciLuaState* state);

SACI_API void psaci_lua_dump_stack(PSaciLuaState* l);

SACI_API void psaci_lua_clear_stack(PSaciLuaState* lua);

SACI_API void psaci_lua_pop(PSaciLuaState* lua, int pops);

SACI_API SaciBool psaci_lua_push_to_stack(PSaciLuaState* lua, const char* path_to_value);

SACI_API SaciBool psaci_lua_push_array_entry_to_stack(PSaciLuaState* lua, const SaciU64 idx);

SACI_API SaciBool psaci_lua_array_iter(PSaciLuaState* lua, const char* path_to_array, PSaciLuaArrayIter array_iter, void* user_data);

// Already pops the data for the "path_to_value"
SACI_API SaciBool psaci_lua_get_value(PSaciLuaState* lua, const char* path_to_value, struct PSaciLuaField* value_out, const SaciDataType expected_type);

SACI_API SaciBool psaci_lua_has_value(PSaciLuaState* lua, const char* path_to_value, const SaciDataType expected_type);

SACI_API SaciU64 psaci_lua_get_array_length_in_stack(PSaciLuaState* lua);

SACI_API SaciU64 psaci_lua_get_array_length(PSaciLuaState* lua, const char* path_to_array);

#endif // SACI_PLATFORM_CONFIG_LUA_H
