#include "saci_platform/config/internal/config.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lua.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "saci_util/internal/log.h"
#include "saci_util/internal/general.h"
#include "saci_util/types.h"

PSaciLuaState* psaci_lua_load(const char* file_path)
{
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    if (luaL_dofile(lua_state, file_path) != LUA_OK) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Failed to load config %s", lua_tostring(lua_state, -1));
        lua_close(lua_state);
        return NULL;
    }
    return lua_state;
}

SaciBool psaci_lua_load_table(PSaciLuaState* state, const char* table_name)
{
    if (!state) {
        return false;
    }
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Table name(%s) is wrong or config is invalid", table_name);
        lua_close(state);
        return false;
    }
    return true;
}

SaciS8 psaci_lua_get_bool(PSaciLuaState* state, const char* b_name)
{
    if (!state) {
        return -1;
    }
    lua_getfield(state, -1, b_name);
    if (lua_isboolean(state, -1)) {
        SaciS8 val = SACI_CAST_M(SaciS8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid boolean: %s", b_name);
    lua_pop(state, 1);
    return -1;
}

SaciU8 psaci_lua_get_uint8(PSaciLuaState* state, const char* i_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        SaciU8 val = SACI_CAST_M(SaciU8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

SaciU32 psaci_lua_get_uint32(PSaciLuaState* state, const char* i_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        SaciU32 val = SACI_CAST_M(SaciU32)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

SaciU64 psaci_lua_get_uint64(PSaciLuaState* state, const char* i_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        SaciU64 val = SACI_CAST_M(SaciU64)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

SaciU64 psaci_lua_get_array_length(PSaciLuaState* state)
{
    if (!state) {
        return 0;
    }
    if (!lua_istable(state, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Expected array table on stack for length");
        return 0;
    }
    SaciU64 len = (SaciU64)lua_rawlen(state, -1);
    return len;
}

const char* psaci_lua_get_str(PSaciLuaState* state, const char* s_name)
{
    if (!state) {
        return NULL;
    }
    lua_getfield(state, -1, s_name);
    if (lua_isstring(state, -1)) {
        size_t len;
        const char* lua_str = lua_tolstring(state, -1, &len);
        char* copy = malloc(sizeof(char) * (len + 1)); // TODO remove
        memcpy(copy, lua_str, len + 1);
        lua_pop(state, 1);
        return copy;
    }

    SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid string: %s", s_name);
    lua_pop(state, 1);
    return NULL;
}

SaciS64 psaci_lua_get_enum(PSaciLuaState* state, const char* e_name)
{
    if (!state) {
        return -1;
    }
    lua_getfield(state, -1, e_name);
    if (lua_isinteger(state, -1)) {
        SaciS64 val = (SaciS64)lua_tointeger(state, -1);
        lua_pop(state, 1);
        return val;
    }
    SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid enum: %s", e_name);
    lua_pop(state, 1);
    return -1;
}

SaciBool psaci_lua_push_global_table(PSaciLuaState* state, const char* table_name)
{
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Global table '%s' not found or invalid", table_name);
        lua_pop(state, 1);
        return false;
    }
    return true;
}

SaciBool psaci_lua_push_field_table(PSaciLuaState* state, const char* field_name)
{
    lua_getfield(state, -1, field_name);
    if (!lua_istable(state, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Field '%s' not found or not a table", field_name);
        lua_pop(state, 1);
        return false;
    }
    return true;
}

SaciU64 psaci_lua_push_field_array(PSaciLuaState* state, const char* array_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, array_name);
    if (!lua_istable(state, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid array: %s", array_name);
        lua_pop(state, 1);
        return 0;
    }
    SaciU64 len = (SaciU64)lua_rawlen(state, -1);
    return len;
}

// 0 based
SaciU64 psaci_lua_push_array_entry(PSaciLuaState* state, const SaciU64 index)
{
    if (!state) {
        return 0;
    }
    if (!lua_istable(state, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Expected array table on stack for entry index %lu", index);
        return 0;
    }
    lua_rawgeti(state, -1, (int)(index + 1));
    if (lua_isnil(state, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Array entry at index %lu is missing or nil", index);
        lua_pop(state, 1);
        return 0;
    }
    return 1;
}

void psaci_lua_pop(PSaciLuaState* state, int count)
{
    lua_pop(state, count);
}

void psaci_lua_close(PSaciLuaState* state)
{
    if (!state) {
        return;
    }
    lua_close(state);
}

void psaci_lua_dump_stack(PSaciLuaState* l)
{

    int top = lua_gettop(l);
    printf("---- SACI Lua stack (top=%d) ----\n", top);
    for (int i = 1; i <= top; i++) {
        int t = lua_type(l, i);
        printf("%d: %s - ", i, lua_typename(l, t));
        switch (t) {
        case LUA_TSTRING:
            printf("'%s'\n", lua_tostring(l, i));
            break;
        case LUA_TBOOLEAN:
            printf(lua_toboolean(l, i) ? "true\n" : "false\n");
            break;
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(l, i));
            break;
        case LUA_TTABLE:
            printf("{table}\n");
            break;
        case LUA_TFUNCTION:
            printf("{function}\n");
            break;
        default:
            printf("%p\n", lua_topointer(l, i));
            break;
        }
    }
    printf("---------------------------\n");
}
