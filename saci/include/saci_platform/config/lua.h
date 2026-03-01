#ifndef SACI_PLATFORM_CONFIG_LUA_H
#define SACI_PLATFORM_CONFIG_LUA_H

#include "saci_util/defines.h"
#include "saci_util/memory.h"
#include "saci_util/types.h"

#include <lua5.4/lua.h>

/* === Lua CFG === */

enum PSaciLuaType {
    PSACI_LUA_TYPE_NIL,
    PSACI_LUA_TYPE_NUMBER,
    PSACI_LUA_TYPE_BOOLEAN,
    PSACI_LUA_TYPE_STRING,
    PSACI_LUA_TYPE_TABLE,
    PSACI_LUA_TYPE_USERDATA,
    PSACI_LUA_TYPE_FUNTION,
};

struct PSaciLuaValue {
    enum PSaciLuaType type;
    union {
        double number;
        int boolean;
        SaciMemChunk* string; /* Caller must free */
        void* userdata;       /* Lua-owned */
    } data;
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
SACI_API SaciBool psaci_lua_get_value(PSaciLuaState* lua, const char* path_to_value, struct PSaciLuaValue* value_out, const enum PSaciLuaType expected_type);

SACI_API SaciBool psaci_lua_has_value(PSaciLuaState* lua, const char* path_to_value, enum PSaciLuaType expected_type);

SACI_API SaciU64 psaci_lua_get_array_length_in_stack(PSaciLuaState* lua);

SACI_API SaciU64 psaci_lua_get_array_length(PSaciLuaState* lua, const char* path_to_array);

#endif // SACI_PLATFORM_CONFIG_LUA_H
