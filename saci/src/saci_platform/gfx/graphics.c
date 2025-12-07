#include "saci_platform/gfx/graphics.h"
#include "./internal/gfx.h"

SACI_API void psaci_graphics_clear_color(const SaciColor color)
{
    psaci_gfx_clear_color(color);
}

SACI_API void psaci_sb_graphics_clear_depth_buffer(void)
{
    psaci_gfx_clear_depth_buffer();
}
