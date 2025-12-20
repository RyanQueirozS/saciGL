#include "saci_platform/config/config.h"
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

#define PSACI_G_PATH_BUFFER_MAX_WORDS (int)32
#define PSACI_G_PATH_BUFFER_WORD_MAX_LETERS (int)128

SACI_INTERNAL void psaci__cfg_path_to_buffer(
    const char* path,
    char path_buffer_out[PSACI_G_PATH_BUFFER_MAX_WORDS][PSACI_G_PATH_BUFFER_WORD_MAX_LETERS],
    int* words_out);

SACI_INTERNAL SaciDataType psaci__cfg_get_datatype_with_lua_type(int lua_type);
SACI_INTERNAL void psaci__cfg_get_value_through_type(PSaciLuaState* lua, PSaciCfgValue* value_out, const char* path_to_value);

// config.h

// TODO reduce complexity
PSaciCfgValue psaci_cfg_get_value(const char* file_path, const char* path_to_value)
{
    PSaciCfgValue result = {0};

    PSaciLuaState* lua = psaci_lua_load(file_path);
    if (!lua) {
        return result;
    }

    SACI_STATIC char psaci__path_buffer
        [PSACI_G_PATH_BUFFER_MAX_WORDS]
        [PSACI_G_PATH_BUFFER_WORD_MAX_LETERS] = {0};

    int word_count = 0;
    psaci__cfg_path_to_buffer(path_to_value, psaci__path_buffer, &word_count);

    if (!word_count) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Empty config path %s", file_path);
        return result;
    }

    lua_getglobal(lua, psaci__path_buffer[0]);
    if (lua_isnil(lua, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG, "Global table is NULL %s",
                          psaci__path_buffer[0]);
        lua_pop(lua, 1);
        return result;
    }

    for (int i = 1; i < word_count; ++i) {
        if (!lua_istable(lua, -1)) {
            SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                              SACI_LOG_CONTEXT_CORE_CONFIG, "Trying to access value in a non table '%s'",
                              psaci__path_buffer[i - 1]);
            lua_pop(lua, i);
            return result;
        }
        lua_getfield(lua, -1, psaci__path_buffer[i]);
    }

    if (lua_isnil(lua, -1)) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG, "Value '%s' is NULL in config",
                          psaci__path_buffer[word_count]);
        lua_pop(lua, word_count);
        return result;
    }

    result.type = psaci__cfg_get_datatype_with_lua_type(lua_type(lua, -1));
    psaci__cfg_get_value_through_type(lua, &result, path_to_value);

    lua_pop(lua, word_count);
    return result;
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

// Internal

SACI_INTERNAL void psaci__cfg_path_to_buffer(
    const char* path,
    char path_buffer_out[PSACI_G_PATH_BUFFER_MAX_WORDS][PSACI_G_PATH_BUFFER_WORD_MAX_LETERS],
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

SACI_INTERNAL SaciDataType psaci__cfg_get_datatype_with_lua_type(int ltype)
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

SACI_INTERNAL void psaci__cfg_get_value_through_type(PSaciLuaState* lua, PSaciCfgValue* value_out, const char* path_to_value)
{
    switch (lua_type(lua, -1)) {
    case LUA_TNUMBER:
        value_out->type = SACI_TYPE_DOUBLE;
        value_out->data.number = lua_tonumber(lua, -1);
        break;

    case LUA_TBOOLEAN:
        value_out->type = SACI_TYPE_BOOL;
        value_out->data.boolean = lua_toboolean(lua, -1);
        break;

    case LUA_TSTRING:
        {
            value_out->type = SACI_TYPE_STRING;
            const char* s = lua_tostring(lua, -1);
            value_out->data.string = strdup(s); // TODO redo allocation
            break;
        }

    case LUA_TUSERDATA:
        value_out->type = SACI_TYPE_UNKOWN;
        value_out->data.userdata = lua_touserdata(lua, -1);
        break;

    default:
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_HIGH,
            SACI_LOG_CONTEXT_CORE_CONFIG,
            "Unsupported Lua type for '%s'",
            path_to_value);
        break;
    }
}
