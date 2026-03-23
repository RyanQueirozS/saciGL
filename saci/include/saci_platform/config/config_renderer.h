#ifndef SACI_PLATFORM_CONFIG_CONFIG_RENDERER_H
#define SACI_PLATFORM_CONFIG_CONFIG_RENDERER_H

#include "saci_platform/config/lua.h"
#include "saci_util/memory.h"
#include "saci_util/defines.h"
#include "saci_util/types.h"

struct PSaciConfigPreallocEntity {
    const char* lua_path_to_value;

    void** allocation_point;
    SaciU64 (*get_size_function)(PSaciLuaState* lua);
};

struct PSaciConfigPopulateEntity {
    const char* lua_path_to_value;

    void** unpopulated_data;
    SaciU64 (*populate_function)(PSaciLuaState* lua);
};

struct PSaciConfigRenderer;

typedef void (*PSaciConfigRendererPreallocFunc)(
    PSaciLuaState* lua, SaciMemPool* mem,
    const struct PSaciConfigPreallocEntity* prealloc_table, void** cfg_data);

typedef void (*PSaciConfigRendererPopulateFunc)(
    PSaciLuaState* lua,
    const struct PSaciConfigPopulateEntity* populate_table,
    void** cfg_data_ptr);

typedef const void* (*PSaciConfigRendererGetFieldFunc)(const void* cfg_data, const void* query);

typedef void (*PSaciConfigRendererResetFunc)(void* cfg_data);

typedef void (*PSaciConfigRendererFreeFunc)(struct PSaciConfigRenderer* self);

struct PSaciConfigRendererInterface {
    PSaciConfigRendererPreallocFunc preallocate;

    PSaciConfigRendererPopulateFunc populate;

    PSaciConfigRendererGetFieldFunc get_field;

    PSaciConfigRendererResetFunc reset;

    PSaciConfigRendererFreeFunc free;
};

SACI_API struct PSaciConfigRenderer* psaci_config_renderer_new(
    PSaciLuaState* lua,
    const struct PSaciConfigRendererInterface* config_renderer_interface,
    const struct PSaciConfigPreallocEntity* prealloc_table,
    const struct PSaciConfigPopulateEntity* populate_table,
    SaciMemPool* pool);

SACI_API SaciBool psaci_config_renderer_reset(
    struct PSaciConfigRenderer* cfg_renderer);

SACI_API SaciBool psaci_config_renderer_free(
    struct PSaciConfigRenderer* cfg_renderer);

SACI_API SaciBool psaci_config_renderer_fetch(
    PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_renderer);

SACI_API const void* psaci_config_renderer_get_data(
    const struct PSaciConfigRenderer* cfg_renderer);

SACI_API const void* psaci_config_renderer_get_field(
    const struct PSaciConfigRenderer* cfg_renderer,
    const void* query);

SACI_API SaciMemPool* psaci_config_renderer_get_pool(struct PSaciConfigRenderer*
                                                         cfg_renderer);

#endif // SACI_PLATFORM_CONFIG_CONFIG_RENDERER_H
