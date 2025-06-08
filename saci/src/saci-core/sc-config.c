#include "saci-core/sc-config.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lua.h>
#include <saci-utils/su-general.h>
#include <stdbool.h>

#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

SA_API sc_configState* sc_Config_Load(const char* file_path) {
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    if (luaL_dofile(lua_state, file_path) != LUA_OK) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Failed to load config %s", lua_tostring(lua_state, -1));
        lua_close(lua_state);
        return NULL;
    }
    return lua_state;
}

SA_API sa_bool sc_Config_Load_Table(sc_configState* state, const char* table_name) {
    if (!state) {
        return false;
    }
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Table name(%s) is wrong or config is invalid", table_name);
        lua_close(state);
        return false;
    }
    return true;
}

SA_API sa_s8 sc_Config_Get_Bool(sc_configState* state, const char* b_name) {
    if (!state) {
        return -1;
    }
    lua_getfield(state, -1, b_name);
    if (lua_isboolean(state, -1)) {
        sa_s8 val = sa_Scast_To_m(sa_s8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Missing or invalid boolean: %s", b_name);
    lua_pop(state, 1);
    return -1;
}

SA_API sa_u8 sc_Config_Get_Uint8(sc_configState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        sa_u8 val = sa_Scast_To_m(sa_u8)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}
SA_API sa_u32 sc_Config_Get_Uint32(sc_configState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        sa_u32 val = sa_Scast_To_m(sa_u32)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

SA_API sa_u64 sc_Config_Get_Uint64(sc_configState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isnumber(state, -1)) {
        sa_u32 val = sa_Scast_To_m(sa_u64)(lua_tointeger(state, -1));
        lua_pop(state, 1);
        return val;
    }
    sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

SA_API const char* sc_Config_Get_Str(sc_configState* state, const char* s_name) {
    if (!state) {
        return NULL;
    }
    lua_getfield(state, -1, s_name);
    if (lua_isstring(state, -1)) {
        const char* val = (lua_tostring(state, -1));
        lua_pop(state, 1);
        return val;
    }
    sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG,
                          "Missing or invalid string: %s", s_name);
    lua_pop(state, 1);
    return NULL;
}

SA_API void sc_Config_Close(sc_configState* state) {
    if (!state) {
        return;
    }
    lua_close(state);
}
