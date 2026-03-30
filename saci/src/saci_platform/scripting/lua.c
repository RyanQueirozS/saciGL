// TODO check if when using `psaci__lua_traverse_to_path` word_count should be 1
// smaller (won't access the last value, as it is not a table)
// TODO remove the if(!lua) and use dummy checks
#include "saci_platform/scripting/lua.h"

#include "saci_util/defines.h"
#include "saci_util/internal/defaults.h"
#include "saci_util/internal/log.h"
#include "saci_util/internal/max_values.h"
#include "saci_util/log.h"
#include "saci_util/memory.h"
#include "saci_util/types.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lua.h>

#include <saci_platform/scripting/scripting.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PSACI_G_PATH_BUFFER_MAX_WORDS (int)32
#define PSACI_G_PATH_BUFFER_WORD_MAX_LETERS (int)128

/* === Internal Declarations === */

// **IMPORTANT** LUA CUSTOM ALLOCATOR
SACI_INTERNAL void* psaci__lua_allocator(void* ud, void* ptr, size_t osize, size_t nsize);

SACI_INTERNAL void psaci__lua_register_modules(lua_State* lua);

// Other funcs:

// Type only used here, but it saves a lot of annoying uselessly-used space
typedef char PSaciCfgPathBufferArray[PSACI_G_PATH_BUFFER_MAX_WORDS][PSACI_G_PATH_BUFFER_WORD_MAX_LETERS];

SACI_STATIC PSaciCfgPathBufferArray psaci_g_path_buffer = {0};

SACI_INTERNAL void psaci__lua_load_default_states(void);

SACI_INTERNAL void psaci__lua_path_to_buffer(
    const char* path,
    PSaciCfgPathBufferArray path_buffer_out,
    int* words_out);

SACI_INTERNAL SaciDataType psaci__lua_get_type(PSaciLuaState* lua, int idx);

SACI_INTERNAL SaciBool psaci__lua_goto_value(PSaciLuaState* lua, const char* path_to_value, int* word_count);

SACI_INTERNAL SaciBool psaci__lua_get_value_through_type(
    PSaciLuaState* lua,
    struct PSaciScriptingValue* value_out);

SACI_INTERNAL SaciBool psaci__lua_traverse_to_path(
    PSaciLuaState* lua,
    const PSaciCfgPathBufferArray path_buffer,
    const int word_count);

SACI_INTERNAL struct PSaciScriptingTable* psaci__lua_get_table(PSaciLuaState* lua, int idx);

/* === Header Implementation === */

SACI_STATIC struct {
    const struct PSaciLuaModule* module_array;
    size_t module_count;
    SaciBool was_initialized;
} psaci_g_lua_ctx;

void psaci_lua_clear_stack(PSaciLuaState* lua)
{
    lua_settop(lua, 0);
}

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

    if (!psaci__lua_traverse_to_path(lua, psaci_g_path_buffer, word_count)) {
        return SACI_FALSE;
    }

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

SaciBool psaci_lua_table_iter(PSaciLuaState* lua, const char* path_to_array, PSaciLuaArrayIter array_iter, void* user_data)
{
    int word_count = 0;
    if (path_to_array) {
        psaci__lua_path_to_buffer(path_to_array, psaci_g_path_buffer, &word_count);

        if (!word_count) {
            SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                              SACI_LOG_CONTEXT_CORE_CONFIG,
                              "Empty path to array %s", path_to_array);
            return SACI_FALSE;
        }

        psaci__lua_traverse_to_path(lua, psaci_g_path_buffer, word_count);
    }

    lua_pushnil(lua); // first key
    SaciU64 i = 0;
    struct PSaciScriptingField field = {0};
    while (lua_next(lua, (int)i) != 0) {
        psaci_lua_push_array_entry_to_stack(lua, i);

        if (!psaci_lua_get_field(lua, NULL, &field, SACI_TYPE_UNKNOWN)) {
            ++i;
            continue;
        }

        array_iter(lua, i, field, user_data);
        psaci_lua_pop(lua, 1);
        ++i;
    }
    psaci_lua_pop(lua, word_count);
    return SACI_TRUE;
}

SaciBool psaci_lua_get_value(PSaciLuaState* lua, const char* path_to_value, struct PSaciScriptingValue* value_out, const SaciDataType expected_type)
{
    if (!lua) {
        return SACI_FALSE;
    }

    int word_count = 0;
    if (!psaci__lua_goto_value(lua, path_to_value, &word_count)) {
        return SACI_FALSE;
    }

    SaciDataType type = psaci__lua_get_type(lua, -1);
    if (type != expected_type && expected_type != SACI_TYPE_UNKNOWN) {
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

    if (path_to_value) {
        psaci_lua_pop(lua, word_count);
    }

    return SACI_TRUE;
}

SACI_API SaciBool psaci_lua_get_field(PSaciLuaState* lua, const char* path_to_field, struct PSaciScriptingField* field_out, const SaciDataType expected_type)
{
    if (!lua) {
        return SACI_FALSE;
    }

    struct PSaciScriptingValue val = {0};
    int word_count = 0;
    if (!psaci__lua_goto_value(lua, path_to_field, &word_count)) {
        return SACI_FALSE;
    }

    SaciDataType type = psaci__lua_get_type(lua, -1);
    if (type != expected_type && expected_type != SACI_TYPE_UNKNOWN) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Type in config (%d) does not match what is expected (%d)",
                          type, expected_type);
    }
    val.type = type;
    if (!psaci__lua_get_value_through_type(lua, &val)) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Could not find type for value in lua stack");
        return SACI_FALSE;
    }

    if (lua_type(lua, -2) != LUA_TSTRING) {
        return SACI_FALSE;
    }
    const char* key = lua_tostring(lua, -2);

    field_out->value = val;
    field_out->name = key;

    if (path_to_field) {
        psaci_lua_pop(lua, word_count);
    }

    return SACI_TRUE;
}

SaciBool psaci_lua_has_value(PSaciLuaState* lua, const char* path_to_value, const SaciDataType expected_type)
{

    int word_count = 0;
    if (!psaci__lua_goto_value(lua, path_to_value, &word_count)) {
        return SACI_FALSE;
    }
    SaciDataType type = psaci__lua_get_type(lua, -1);
    if (type != expected_type && expected_type != SACI_TYPE_UNKNOWN) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_CONFIG,
                          "Value %s's type (%d) does not match expected type (%d)",
                          psaci_g_path_buffer[word_count], type, expected_type);
    }
    psaci_lua_pop(lua, word_count);
    return SACI_TRUE;
}

SaciU64 psaci_lua_get_array_length(PSaciLuaState* lua, const char* path_to_array)
{
    if (!lua) {
        return 0;
    }

    int word_count = 0;
    if (path_to_array) {
        psaci__lua_path_to_buffer(path_to_array, psaci_g_path_buffer, &word_count);

        if (!word_count) {
            SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                              SACI_LOG_CONTEXT_CORE_CONFIG,
                              "Empty path to value %s", path_to_array);
            return 0;
        }

        if (!psaci__lua_traverse_to_path(lua, psaci_g_path_buffer, word_count)) {
            return 0;
        }
    }
    SaciU64 arr_len = psaci_lua_get_key_value_count_in_stack(lua);
    psaci_lua_pop(lua, word_count);
    return arr_len;
}

SaciU64 psaci_lua_get_key_value_count_in_stack(PSaciLuaState* lua)
{
    SaciU64 field_count = 0;

    lua_pushnil(lua);
    while (lua_next(lua, -1) != 0) {
        field_count++;
        lua_pop(lua, 1);
    }

    return field_count;
}

SaciU64 psaci_lua_get_key_value_count(PSaciLuaState* lua, const char* path_to_table)
{
    int word_count = 0;
    if (!psaci__lua_goto_value(lua, path_to_table, &word_count)) {
        return SACI_FALSE;
    }

    SaciU64 count = psaci_lua_get_key_value_count_in_stack(lua);
    psaci_lua_pop(lua, word_count);
    return count;
}

SACI_API SaciBool psaci_lua_query_field_type(PSaciLuaState* lua, const char* path_to_value, SaciDataType* type_out)
{
    if (!lua) {
        return SACI_FALSE;
    }
    int word_count;
    if (!psaci__lua_goto_value(lua, path_to_value, &word_count)) {
        return SACI_FALSE;
    }
    *type_out = psaci__lua_get_type(lua, -1);
    psaci_lua_pop(lua, word_count);
    return SACI_TRUE;
}

SACI_API void psaci_lua_init(const struct PSaciLuaModule* module_array,
                             SaciU64 module_count)
{
    if (psaci_g_lua_ctx.was_initialized) {
        return; // TODO log error
    }
    psaci_g_lua_ctx.module_array = module_array;
    psaci_g_lua_ctx.module_count = module_count;
}

PSaciLuaState* psaci_lua_load(const char* file_path)
{
    lua_State* lua = lua_newstate(psaci__lua_allocator, NULL);
    luaL_openlibs(lua);

    psaci__lua_register_modules(lua);

    if (luaL_dofile(lua, file_path) != LUA_OK) {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_DEV,
            SACI_LOG_ERROR_SEVERITY_MEDIUM,
            SACI_LOG_CONTEXT_CORE_CONFIG,
            "Failed to load config %s",
            lua_tostring(lua, -1));
        lua_close(lua);
        return NULL;
    }

    return lua;
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

/* === Internal Implementation === */

SACI_INTERNAL void* psaci__lua_allocator(void* ud, void* ptr, size_t osize, size_t nsize)
{
    (void)ud;
    (void)osize;

    if (nsize == 0) {
        if (ptr) {
            saci_mem_chunk_free(ptr);
        }
        return NULL;
    }

    if (!ptr) {
        SaciMemChunk* chunk =
            saci_mem_chunk_alloc_size(SACI_MEM_CONTEXT_LUA, nsize);
        return chunk ? saci_mem_chunk_get_ptr_offset(chunk, 0) : NULL;
    }

    SaciMemChunk* chunk =
        saci_mem_chunk_from_data(ptr);

    chunk = saci_mem_chunk_realloc(chunk, nsize);

    return chunk ? saci_mem_chunk_get_ptr_offset(chunk, 0) : NULL;
}

SACI_INTERNAL int psaci__lua_module_loader(lua_State* lua)
{
    const struct PSaciLuaModule* mod =
        (const struct PSaciLuaModule*)lua_touserdata(lua, lua_upvalueindex(1));

    if (luaL_loadbuffer(lua,
                        (const char*)mod->bytecode,
                        mod->size,
                        mod->name) != LUA_OK) {
        return lua_error(lua);
    }

    if (lua_pcall(lua, 0, 1, 0) != LUA_OK) {
        return lua_error(lua);
    }

    return 1;
}

SACI_INTERNAL void psaci__lua_register_modules(lua_State* lua)
{

    lua_getglobal(lua, "package");
    lua_getfield(lua, -1, "preload");

    for (size_t i = 0; i < psaci_g_lua_ctx.module_count; i++) {
        const struct PSaciLuaModule* mod = &psaci_g_lua_ctx.module_array[i];

        lua_pushlightuserdata(lua, (void*)mod);
        lua_pushcclosure(lua, psaci__lua_module_loader, 1);

        lua_setfield(lua, -2, mod->name);
    }

    lua_pop(lua, 2);
}

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

SACI_INTERNAL SaciDataType psaci__lua_get_type(PSaciLuaState* lua, int idx)
{
    int ltype = lua_type(lua, idx);

    switch (ltype) {
    case LUA_TNIL:
        return SACI_TYPE_NULL;

    case LUA_TBOOLEAN:
        return SACI_TYPE_BOOL;

    case LUA_TNUMBER:
        if (lua_isinteger(lua, idx)) {
            return SACI_TYPE_S64;
        }
        return SACI_TYPE_FLOAT;

    case LUA_TSTRING:
        return SACI_TYPE_STRING;

    case LUA_TTABLE:
        return SACI_TYPE_STRUCT;

    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        return SACI_TYPE_VOIDPTR;

    case LUA_TFUNCTION:
        return SACI_TYPE_FUNCTION_LUA;

    case LUA_TTHREAD:
        return SACI_TYPE_THREAD_LUA;

    default:
        return SACI_TYPE_UNKNOWN;
    }
}

SACI_INTERNAL SaciBool psaci__lua_get_value_through_type(PSaciLuaState* lua, struct PSaciScriptingValue* value_out)
{
    switch (value_out->type) {
    case SACI_TYPE_NULL:
        {
            return SACI_FALSE;
        }
    case SACI_TYPE_BOOL:
        {
            value_out->data.boolean = lua_toboolean(lua, -1);
            return SACI_TRUE;
        }
    case SACI_TYPE_S64:
        {
            value_out->data.s64 = lua_tonumber(lua, -1);
            return SACI_TRUE;
        }
    case SACI_TYPE_DOUBLE:
        {
            value_out->data.f64 = lua_tonumber(lua, -1);
            return SACI_TRUE;
        }
    case SACI_TYPE_STRING:
        {
            const char* s = lua_tostring(lua, -1);
            value_out->data.string =
                saci_mem_chunk_strdup(SACI_MEM_CONTEXT_LUA,
                                      s, SACI_MAX_CONFIG_FIELD_STRING_SIZE);
            return SACI_TRUE;
        }

    case SACI_TYPE_STRUCT:
        {
            value_out->data.table = psaci__lua_get_table(lua, -1);
            return SACI_TRUE;
        }
    case SACI_TYPE_VOIDPTR:
        {
            value_out->data.voidptr = lua_touserdata(lua, -1);
            return SACI_TRUE;
        }
    case SACI_TYPE_FUNCTION_LUA:
        {
            value_out->data.func_ref = luaL_ref(lua, LUA_REGISTRYINDEX);
            return SACI_TRUE;
        }
    case SACI_TYPE_THREAD_LUA:
        {
            value_out->data.thread_ref = luaL_ref(lua, LUA_REGISTRYINDEX);
            return SACI_TRUE;
        }
    default:
        {
            return SACI_FALSE;
        }
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

SACI_INTERNAL struct PSaciScriptingTable* psaci__lua_get_table(PSaciLuaState* lua, int idx)
{
    if (!lua_istable(lua, idx)) {
        return NULL;
    }

    idx = lua_absindex(lua, idx);

    lua_pushnil(lua);

    // Get the size to alloc value_array
    SaciU64 value_count = 0;
    while (lua_next(lua, idx) != 0) {
        value_count++;
        psaci_lua_pop(lua, 1); // Pop value
    }
    SaciMemChunk* chunk = saci_mem_chunk_alloc_size(
        SACI_MEM_CONTEXT_LUA,
        sizeof(struct PSaciScriptingTable) +
            (sizeof(struct PSaciScriptingValue) * value_count));
    struct PSaciScriptingTable* table = saci_mem_chunk_get_ptr_offset(chunk, 0);
    table->field_array = saci_mem_chunk_get_ptr_offset(
        chunk, sizeof(table->field_array));
    table->field_count = value_count;
    table->chunk = chunk;

    while (lua_next(lua, idx) != 0) {
        if (lua_type(lua, -2) != LUA_TSTRING) {
            lua_pop(lua, 1);
            continue;
        }

        const char* key = lua_tostring(lua, -2);

        struct PSaciScriptingValue* value =
            &table->field_array[table->field_count].value;

        table->field_array[table->field_count].name = key;

        value->type = psaci__lua_get_type(lua, -1);

        psaci__lua_get_value_through_type(lua, value);

        table->field_count++;

        psaci_lua_pop(lua, 1); // Pop value
    }

    return table;
}

SACI_INTERNAL SaciBool psaci__lua_goto_value(PSaciLuaState* lua, const char* path_to_value, int* word_count)
{
    if (path_to_value) {
        psaci__lua_path_to_buffer(path_to_value, psaci_g_path_buffer, word_count);

        if (!word_count) {
            SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                              SACI_LOG_CONTEXT_CORE_CONFIG,
                              "Empty path to value %s", path_to_value);
            return SACI_FALSE;
        }

        if (!psaci__lua_traverse_to_path(lua, psaci_g_path_buffer, *word_count)) {
            return SACI_FALSE;
        }
    }
    return SACI_TRUE;
}
