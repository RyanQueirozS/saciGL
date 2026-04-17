#include "saci_core/config/config_manager.h"

#include "saci_platform/config/internal/config_sys.h"
#include "saci_platform/scripting/lua.h"

#include "saci_gen/saci_api.h"
#include "saci_gen/saci_core_api.h"
#include "saci_gen/saci_platform_api.h"

#include "saci_util/internal/defaults.h"
#include "saci_util/internal/general.h"
#include <saci_util/types.h>

/* === Internal === */

SACI_STATIC const struct PSaciLuaRegFunc CSACI_G_CONFIG_FUNCS[] = {
    {
        .c_func = NULL,
        .func_name = "",
        .nested_table_array = (const char*[]){"", ""},
        .nested_table_count = 0,

        .root_table = "saci",
    },
};

SACI_STATIC const struct PSaciLuaModule CSACI_G_CONFIG_MODULES[3] = {
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
};

/* === Header Implementation === */

void csaci_cfg_manager_init(void)
{
    psaci_config_system_init();

    PSaciLuaState* lua = psaci_lua_load(SACI_CONFIG_PATH);

    for (SaciU64 i = 0; i < SACI_ARRLEN_M(CSACI_G_CONFIG_FUNCS); ++i) {
        psaci_lua_register_cfunc(lua, CSACI_G_CONFIG_FUNCS[i]);
    }

    psaci_lua_init_modules(CSACI_G_CONFIG_MODULES,
                           SACI_ARRLEN_M(CSACI_G_CONFIG_MODULES));

    psaci_lua_register_modules(lua);
}

/* === Internal === */
