#ifndef SACI_CORE_CONFIG_CONFIG_MANAGER_H
#define SACI_CORE_CONFIG_CONFIG_MANAGER_H

#include "saci_platform/scripting/scripting.h"
#include "saci_platform/scripting/lua.h"

struct PSaciConfigRenderer* csaci_cfg_manager_renderer_new(
    const char* renderer_type,
    const char* file_path);

#endif // SACI_CORE_CONFIG_CONFIG_MANAGER_H
