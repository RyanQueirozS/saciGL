#include "saci_core/startup/startup.h"

#include "saci_platform/gfx/internal/gfx.h"
#include "saci_core/config/config_manager.h"
#include "saci_platform/dependencies/dependency.h"

typedef struct PSaciDependencyLoaderContext CSaciDependencyLoaderContext;

SaciBool csaci_startup_dependencies_init(void)
{
    csaci_cfg_manager_init();

    psaci_dependencies_load(PSACI_G_DEPENDENCY_CONTEXT_DEFAULT);

    SaciBool success = psaci_gfx_load();
    psaci_gfx_initialize_renderer_debugger(NULL);

    return success;
}
