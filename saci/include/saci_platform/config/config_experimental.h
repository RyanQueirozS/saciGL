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

struct PSaciConfigRendererInterface {
    void (*preallocate)(struct PSaciConfigRenderer* self, SaciMemPool* mem, const struct PSaciConfigPreallocEntity* prealloc_table, void** cfg_data);

    void (*populate)(struct PSaciConfigRenderer* self, const struct PSaciConfigPopulateEntity* populate_table, void** cfg_data);

    void (*reset)(struct PSaciConfigRenderer* self);

    void (*free)(struct PSaciConfigRenderer* self);
};

SACI_API struct PSaciConfigRenderer* psaci_config_renderer_new(
    const struct PSaciConfigRendererInterface* config_renderer_interface,
    const struct PSaciConfigPreallocEntity* prealloc_table,
    const struct PSaciConfigPopulateEntity* populate_table,
    SaciMemPool* pool);

SACI_API SaciBool psaci_config_renderer_reset(struct PSaciConfigRenderer* cfg_renderer);

SACI_API SaciBool psaci_config_renderer_free(struct PSaciConfigRenderer* cfg_renderer);

SACI_API SaciBool psaci_config_renderer_fetch(struct PSaciConfigRenderer* cfg_renderer);

SACI_API const void* psaci_config_renderer_get_data(const struct PSaciConfigRenderer* cfg_renderer);

SACI_API SaciMemPool* psaci_config_renderer_get_pool(struct PSaciConfigRenderer* cfg_renderer);
