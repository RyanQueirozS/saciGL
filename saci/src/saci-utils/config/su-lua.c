#ifndef __EMSCRIPTEN__
#  include "saci-utils/config/su-lua.h"

#  include <lua5.4/lauxlib.h>
#  include <lua5.4/lualib.h>
#  include <lua5.4/lua.h>

#  include <stdbool.h>
#  include <stdlib.h>
#  include <string.h>

#  include "saci-utils/su-general.h"
#  include "saci-utils/su-log.h"
#  include "saci-utils/su-types-common.h"

su_LuaState* su_lua_load(const char* file_path)
{
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    if (luaL_dofile(lua_state, file_path) != LUA_OK) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Failed to load config %s", lua_tostring(lua_state, -1));
        lua_close(lua_state);
        return NULL;
    }
    return lua_state;
}

su_Bool su_lua_load_table(su_LuaState* state, const char* table_name)
{
    if (!state) {
        return false;
    }
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Table name(%s) is wrong or config is invalid", table_name);
        lua_close(state);
        return false;
    }
    return true;
}

su_S8 su_lua_get_bool(su_LuaState* state, const char* b_name)
{
    if (!state) {
        return -1;
    }
    lua_getfield(state, -1, b_name);
    if (lua_isboolean(state, -1)) {
        su_S8 val = su_CAST_M(su_S8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid boolean: %s", b_name);
    lua_pop(state, 1);
    return -1;
}

su_U8 su_lua_get_uint8(su_LuaState* state, const char* i_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        su_U8 val = su_CAST_M(su_U8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

su_U32 su_lua_get_uint32(su_LuaState* state, const char* i_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        su_U32 val = su_CAST_M(su_U32)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

su_U64 su_lua_get_uint64(su_LuaState* state, const char* i_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        su_U64 val = su_CAST_M(su_U64)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

su_U64 su_lua_get_array_length(su_LuaState* state)
{
    if (!state) {
        return 0;
    }
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG,
                        "Expected array table on stack for length");
        return 0;
    }
    su_U64 len = (su_U64)lua_rawlen(state, -1);
    return len;
}

const char* su_lua_get_str(su_LuaState* state, const char* s_name)
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

    su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid string: %s", s_name);
    lua_pop(state, 1);
    return NULL;
}

su_S64 su_lua_get_enum(su_LuaState* state, const char* e_name)
{
    if (!state) {
        return -1;
    }
    lua_getfield(state, -1, e_name);
    if (lua_isinteger(state, -1)) {
        su_S64 val = (su_S64)lua_tointeger(state, -1);
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid enum: %s", e_name);
    lua_pop(state, 1);
    return -1;
}

su_Bool su_lua_push_global_table(su_LuaState* state, const char* table_name)
{
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG,
                        "Global table '%s' not found or invalid", table_name);
        lua_pop(state, 1);
        return false;
    }
    return true;
}

su_Bool su_lua_push_field_table(su_LuaState* state, const char* field_name)
{
    lua_getfield(state, -1, field_name);
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Field '%s' not found or not a table", field_name);
        lua_pop(state, 1);
        return false;
    }
    return true;
}

su_U64 su_lua_push_field_array(su_LuaState* state, const char* array_name)
{
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, array_name);
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Missing or invalid array: %s", array_name);
        lua_pop(state, 1);
        return 0;
    }
    su_U64 len = (su_U64)lua_rawlen(state, -1);
    return len;
}

// 0 based
su_U64 su_lua_push_array_entry(su_LuaState* state, const su_U64 index)
{
    if (!state) {
        return 0;
    }
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Expected array table on stack for entry index %lu", index);
        return 0;
    }
    lua_rawgeti(state, -1, (int)(index + 1));
    if (lua_isnil(state, -1)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Array entry at index %lu is missing or nil", index);
        lua_pop(state, 1);
        return 0;
    }
    return 1;
}

void su_lua_pop(su_LuaState* state, int count)
{
    lua_pop(state, count);
}

void su_lua_close(su_LuaState* state)
{
    if (!state) {
        return;
    }
    lua_close(state);
}

void su_lua_dump_stack(su_LuaState* L)
{

    int top = lua_gettop(L);
    printf("---- SACI Lua stack (top=%d) ----\n", top);
    for (int i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        printf("%d: %s - ", i, lua_typename(L, t));
        switch (t) {
        case LUA_TSTRING:
            printf("'%s'\n", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf(lua_toboolean(L, i) ? "true\n" : "false\n");
            break;
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(L, i));
            break;
        case LUA_TTABLE:
            printf("{table}\n");
            break;
        case LUA_TFUNCTION:
            printf("{function}\n");
            break;
        default:
            printf("%p\n", lua_topointer(L, i));
            break;
        }
    }
    printf("---------------------------\n");
}
#endif // __EMSCRIPTEN__
