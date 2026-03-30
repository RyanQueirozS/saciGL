#include "saci_core/config/config_manager.h"

#include "saci_platform/config/internal/config_sys.h"

#include "saci_platform/scripting/lua.h"
#include <saci_util/internal/general.h>

#include "saci_gen/saci_api.h"
#include "saci_gen/saci_core_api.h"
#include "saci_gen/saci_platform_api.h"

/* === Internal === */

SACI_STATIC struct {
    struct PSaciLuaModule modules[3];
} csaci_g_config_context = {
    .modules = {
        {
            .name = "saci",
            .bytecode = saci_api_luac,
            .size = SACI_ARRLEN_M(saci_api_luac),
        },
        {
            .name = "saci_core",
            .bytecode = saci_core_api_luac,
            .size = SACI_ARRLEN_M(saci_core_api_luac),
        },
        {
            .name = "saci_platform",
            .bytecode = saci_platform_api_luac,
            .size = SACI_ARRLEN_M(saci_platform_api_luac),
        },
    },
};

/* === Header Implementation === */

void csaci_cfg_manager_load(void)
{
    psaci_config_system_init();
    psaci_lua_init(csaci_g_config_context.modules,
                   SACI_ARRLEN_M(csaci_g_config_context.modules));
}

/* === Internal === */
