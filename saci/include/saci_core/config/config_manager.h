#ifndef SACI_CORE_CONFIG_CONFIG_MANAGER_H
#define SACI_CORE_CONFIG_CONFIG_MANAGER_H

#include "saci_platform/config/config_renderer.h"

enum CSaciConfigRendererType {
    CSACI_CONFIG_RENDERER_DEFAULT_INSTANCE_3D,
};

struct PSaciConfigRenderer* csaci_cfg_manager_renderer_new(
    enum CSaciConfigRendererType renderer_type,
    const char* file_path);

#endif // SACI_CORE_CONFIG_CONFIG_MANAGER_H
