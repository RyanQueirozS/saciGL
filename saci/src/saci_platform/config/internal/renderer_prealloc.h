#ifndef SACI_PLATFORM_CONFIG_INTERNAL_RENDERER_PREALLOC_H
#define SACI_PLATFORM_CONFIG_INTERNAL_RENDERER_PREALLOC_H

#include "saci_platform/config/config.h"

#include "saci_util/memory.h"

void psaci_cfg_renderer_prealloc(PSaciLuaState* lua, struct PSaciConfigRenderer* cfg_out, const char* renderer_name, SaciMemPool** pool_out);

#endif // SACI_PLATFORM_CONFIG_INTERNAL_RENDERER_PREALLOC_H
