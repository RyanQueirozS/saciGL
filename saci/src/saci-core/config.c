#include "config.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lua.h>

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

SA_API void sc_Config_Load_Table(sc_configState* state, const char* table_name) {
    if (!state) {
        return;
    }
    lua_getglobal(state, table_name);
    if (!lua_istable(state, -1)) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Table name(%s) is wrong or config is invalid", table_name);
        lua_close(state);
        return;
    }
}

SA_API sa_u8 sc_Config_Get_Int8(sc_configState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isinteger(state, -1)) {
        return lua_tointeger(state, -1);
    }
    sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

SA_API sa_u32 sc_Config_Get_Int(sc_configState* state, const char* i_name) {
    if (!state) {
        return 0;
    }
    lua_getfield(state, -1, i_name);
    if (lua_isinteger(state, -1)) {
        return lua_tointeger(state, -1);
    }
    sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_CONFIG, "Missing or invalid integer: %s", i_name);
    lua_pop(state, 1);
    return 0;
}

SA_API void sc_Config_Close(sc_configState* state) {
    if (!state) {
        return;
    }
    lua_close(state);
}
