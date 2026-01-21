#include "saci_core/startup/startup.h"

#include "saci_platform/gfx/internal/gfx.h"
#include "saci_platform/dependencies/dependency.h"

#ifdef __ESMCRIPTEN__
SaciBool csaci_startup_dependencies_load(void)
{
    return SACI_TRUE;
}

SaciBool csaci_startup_gfx_load(void)
{
    return SACI_TRUE;
}
#else

SaciBool csaci_startup_dependencies_load(void)
{
    // psaci_dependencies_load();
    return SACI_TRUE;
}

SaciBool csaci_startup_gfx_load(void)
{
    SaciBool success = psaci_gfx_load();
    psaci_gfx_initialize_renderer_debugger(NULL);
    return success;
}
#endif
