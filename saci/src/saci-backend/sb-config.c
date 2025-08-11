#include "saci-backend/sb-config.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lua.h>
#include <saci-utils/su-general.h>
#include <stdbool.h>

#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

sb_ConfigState* sb_config_load(const char* file_path) {
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    if (luaL_dofile(lua_state, file_path) != LUA_OK) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG, "Failed to load config %s", lua_tostring(lua_state, -1));
        lua_close(lua_state);
        return NULL;
    }
    return lua_state;
}

su_Bool sb_config_load_table(sb_ConfigState* state, const char* table_name) {
    if (!state) {
        return false;
    }
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG, "Table name(%s) is wrong or config is invalid", table_name);
        lua_close(state);
        return false;
    }
    return true;
}

su_S8 sb_config_get_bool(sb_ConfigState* state, const char* b_name) {
    if (!state) {
        return -1;
    }
    lua_getfield(state, -1, b_name);
    if (lua_isboolean(state, -1)) {
        su_S8 val = su_SCAST_TO_M(su_S8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG, "Missing or invalid boolean: %s", b_name);
    lua_pop(state, 1);
    return -1;
}

su_U8 sb_config_get_uint8(sb_ConfigState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        su_U8 val = su_SCAST_TO_M(su_U8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

su_U32 sb_config_get_uint32(sb_ConfigState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        su_U32 val = su_SCAST_TO_M(su_U32)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

su_U64 sb_config_get_uint64(sb_ConfigState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        su_U64 val = su_SCAST_TO_M(su_U64)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

const char* sb_config_get_str(sb_ConfigState* state, const char* s_name) {
    if (!state) {
        return NULL;
    }
    lua_getfield(state, -1, s_name);
    if (lua_isstring(state, -1)) {
        const char* val = (lua_tostring(state, -1));
        lua_pop(state, 1);
        return val;
    }
    su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG,
                          "Missing or invalid string: %s", s_name);
    lua_pop(state, 1);
    return NULL;
}

su_Bool sb_config_push_global_table(sb_ConfigState* state, const char* table_name) {
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG,
                              "Global table '%s' not found or invalid", table_name);
        lua_pop(state, 1);
        return false;
    }
    return true;
}

su_Bool sb_config_push_field_table(sb_ConfigState* state, const char* field_name) {
    lua_getfield(state, -1, field_name);
    if (!lua_istable(state, -1)) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG,
                              "Field '%s' not found or not a table", field_name);
        lua_pop(state, 1);
        return false;
    }
    return true;
}

void sb_config_pop(sb_ConfigState* state, int count) {
    lua_pop(state, count);
}

void sb_config_close(sb_ConfigState* state) {
    if (!state) {
        return;
    }
    lua_close(state);
}
