#include "saci_platform/config/config.h"

#include "saci_platform/config/internal/lua.h"
#include "saci_platform/config/internal/renderer_prealloc.h"
#include "saci_platform/config/internal/renderer_populate.h"

#include "saci_util/memory.h"

#include <string.h>

/* === config.h === */

void psaci_cfg_renderer_get(const char* name, struct PSaciConfigRenderer* cfg_out, const char* cfg_file_path)
{
    PSaciLuaState* lua = psaci_lua_load(cfg_file_path);

    SaciMemPool* pool = NULL;

    psaci_cfg_renderer_prealloc(lua, cfg_out, name, &pool);
    psaci_cfg_renderer_populate_fields(lua, cfg_out);

    psaci_lua_close(lua);
}

SaciU64 psaci_cfg_renderer_cfg_size(const struct PSaciConfigRenderer* cfg)
{
    SaciU64 total_size = 0;
    return total_size;
}

void psaci_cfg_renderer_cleanup_cfg(struct PSaciConfigRenderer* cfg)
{
}
