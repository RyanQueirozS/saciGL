#include "saci_core/config/config_manager.h"

#include "saci_platform/config/config_renderer.h"
#include "saci_platform/scripting/lua.h"
#include "saci_util/internal/defaults.h"

SACI_STATIC const struct PSaciConfigRendererInterface CSACI_G_RENDERER_INTERFACE_INSTANCE = {0};

SACI_STATIC const struct PSaciConfigPreallocEntity* const CSACI_G_PREALLOC_TABLE_INSTANCE = {0};

SACI_STATIC const struct PSaciConfigPopulateEntity* const CSACI_G_POPULATE_TABLE_INSTANCE = {0};

struct PSaciConfigRenderer* csaci_cfg_manager_renderer_new(
    const char* renderer_type,
    const char* file_path)
{
    struct PSaciConfigRenderer* new_cfg = NULL;
    // PSaciLuaState* lua = psaci_lua_load(file_path ? file_path : SACI_CONFIG_PATH);

    // switch (renderer_type) {
    // case CSACI_CONFIG_RENDERER_DEFAULT_INSTANCE_3D:
    //     {
    //         new_cfg = psaci_config_renderer_new(lua, &CSACI_G_RENDERER_INTERFACE_INSTANCE, CSACI_G_PREALLOC_TABLE_INSTANCE, CSACI_G_POPULATE_TABLE_INSTANCE, NULL);
    //         break;
    //     }
    // }

    return new_cfg;
}
