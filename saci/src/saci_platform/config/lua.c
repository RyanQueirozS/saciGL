// TODO remove the if(!lua) and use dummy checks
#include "saci_platform/config/config.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lua.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "saci_util/internal/log.h"
#include "saci_util/types.h"

#define PSACI_G_PATH_BUFFER_MAX_WORDS (int)32
#define PSACI_G_PATH_BUFFER_WORD_MAX_LETERS (int)128

// Type only used here, but it saves a lot of annoying uselessly-used space
typedef char PSaciCfgPathBufferArray[PSACI_G_PATH_BUFFER_MAX_WORDS][PSACI_G_PATH_BUFFER_WORD_MAX_LETERS];

SACI_STATIC PSaciCfgPathBufferArray psaci_g_path_buffer = {0};

SACI_INTERNAL void psaci__lua_path_to_buffer(
    const char* path,
    PSaciCfgPathBufferArray path_buffer_out,
    int* words_out);

SACI_INTERNAL SaciDataType psaci__lua_get_datatype_with_lua_type(int lua_type);
SACI_INTERNAL SaciBool psaci__lua_get_value_through_type(PSaciLuaState* lua, PSaciLuaValue* value_out);
SACI_INTERNAL SaciBool psaci__lua_traverse_to_path(PSaciLuaState* lua, const PSaciCfgPathBufferArray path_buffer, const int word_count);

// lua.h

void psaci_lua_pop(PSaciLuaState* lua, int pops)
{
    lua_pop(lua, pops);
}

SaciBool psaci_lua_push_to_stack(PSaciLuaState* lua, const char* path_to_value)
{
    if (!lua) {
        return SACI_FALSE;
    }

    int word_count = 0;
    psaci__lua_path_to_buffer(path_to_value, psaci_g_path_buffer, &word_count);

    if (!word_count) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Empty path to value %s", path_to_value);
        return SACI_FALSE;
    }

    psaci__lua_traverse_to_path(lua, psaci_g_path_buffer, word_count);

    return SACI_TRUE;
}

SaciBool psaci_lua_push_array_entry_to_stack(PSaciLuaState* lua, const SaciU64 idx)
{
    if (!lua) {
        return SACI_FALSE;
    }

    lua_rawgeti(lua, -1, (SaciS64)idx);

    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_HIGH,
            SACI_LOG_CONTEXT_CORE_CONFIG,
            "Index " SACI_FMTU64 " out of range", idx);
        return SACI_FALSE;
    }

    return SACI_TRUE;
}

SaciBool psaci_lua_array_itter(PSaciLuaState* lua, const char* path_to_array, PSaciLuaArrayItter array_itter, void* user_data)
{
    psaci_lua_push_to_stack(lua, path_to_array);
    const SaciU64 length = psaci_lua_get_array_length_in_stack(lua);
    if (!length) {
        return SACI_TRUE;
    }
    for (SaciU64 i = 0; i < length; ++i) {
        array_itter(lua, i, user_data);
    }
    return SACI_TRUE;
}

SaciBool psaci_lua_get_value_in_stack(PSaciLuaState* lua, PSaciLuaValue* value_out, const SaciDataType expected_type)
{
    if (!lua) {
        return SACI_FALSE;
    }

    SaciDataType type = psaci__lua_get_datatype_with_lua_type(lua_type(lua, -1));
    if (type != expected_type) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Type in config (%d) does not match what is expected (%d)",
                          type, expected_type);
    }
    value_out->type = type;
    if (!psaci__lua_get_value_through_type(lua, value_out)) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not find type for value in lua stack");
        return SACI_FALSE;
    }

    return SACI_TRUE;
}

SaciU64 psaci_lua_get_array_length_in_stack(PSaciLuaState* lua)
{
    if (!lua) {
        return 0;
    }

    return lua_rawlen(lua, -1);
}

// internal/config.h

PSaciLuaState* psaci_lua_load(const char* file_path)
{
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    if (luaL_dofile(lua_state, file_path) != LUA_OK) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_CONFIG, "Failed to load config %s", lua_tostring(lua_state, -1));
        lua_close(lua_state);
        return NULL;
    }
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_CORE_CONFIG, "Loaded config at %s", file_path);
    return lua_state;
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

// Internal

SACI_INTERNAL void psaci__lua_path_to_buffer(
    const char* path,
    PSaciCfgPathBufferArray path_buffer_out,
    int* words_out)
{
    int word = 0;
    int idx = 0;

    for (SaciU64 i = 0; path[i] != '\0'; ++i) {
        if (path[i] == '.') {
            path_buffer_out[word][idx] = '\0';
            word++;
            idx = 0;
            if (word >= PSACI_G_PATH_BUFFER_MAX_WORDS) {
                break;
            }
            continue;
        }

        if (idx < PSACI_G_PATH_BUFFER_WORD_MAX_LETERS) {
            path_buffer_out[word][idx++] = path[i];
        }
    }

    if (idx > 0 && word < PSACI_G_PATH_BUFFER_MAX_WORDS) {
        path_buffer_out[word][idx] = '\0';
        word++;
    }

    *words_out = word;
}

SACI_INTERNAL SaciDataType psaci__lua_get_datatype_with_lua_type(int ltype)
{
    switch (ltype) {
    case LUA_TNIL:
        return SACI_TYPE_UNKOWN;

    case LUA_TBOOLEAN:
        return SACI_TYPE_BOOL;

    case LUA_TNUMBER:
        return SACI_TYPE_DOUBLE;

    case LUA_TSTRING:
        return SACI_TYPE_STRING;

    case LUA_TTABLE:
        return SACI_TYPE_UNKOWN;

    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        return SACI_TYPE_UNKOWN;

    case LUA_TFUNCTION:
    case LUA_TTHREAD:
    default:
        return SACI_TYPE_UNKOWN;
    }
}

SACI_INTERNAL SaciBool psaci__lua_get_value_through_type(PSaciLuaState* lua, PSaciLuaValue* value_out)
{
    switch (lua_type(lua, -1)) {
    case LUA_TNUMBER:
        value_out->type = SACI_TYPE_DOUBLE;
        value_out->data.number = lua_tonumber(lua, -1);
        return SACI_TRUE;

    case LUA_TBOOLEAN:
        value_out->type = SACI_TYPE_BOOL;
        value_out->data.boolean = lua_toboolean(lua, -1);
        return SACI_TRUE;

    case LUA_TSTRING:
        value_out->type = SACI_TYPE_STRING;
        const char* s = lua_tostring(lua, -1);
        value_out->data.string = strdup(s); // TODO redo allocation
        return SACI_TRUE;

    case LUA_TUSERDATA:
        value_out->type = SACI_TYPE_UNKOWN;
        value_out->data.userdata = lua_touserdata(lua, -1);
        return SACI_TRUE;

    default:
        return SACI_FALSE;
    }
}

SACI_INTERNAL SaciBool psaci__lua_traverse_to_path(PSaciLuaState* lua, const PSaciCfgPathBufferArray path_buffer, const int word_count)
{
    lua_getglobal(lua, path_buffer[0]);
    if (lua_isnil(lua, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG, "Global table is NULL %s",
                          path_buffer[0]);
        lua_pop(lua, 1);
        return SACI_FALSE;
    }

    for (int i = 1; i < word_count; ++i) {
        if (!lua_istable(lua, -1)) {
            SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                              SACI_LOG_CONTEXT_CORE_CONFIG, "Trying to access value in a non table '%s'",
                              path_buffer[i - 1]);
            lua_pop(lua, i);
            return SACI_FALSE;
        }
        lua_getfield(lua, -1, path_buffer[i]);
    }
    if (lua_isnil(lua, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG, "Value '%s' is NULL in config",
                          path_buffer[word_count]);
        lua_pop(lua, word_count);
        return SACI_FALSE;
    }
    return SACI_TRUE;
}
