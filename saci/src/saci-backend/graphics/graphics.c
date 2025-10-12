#include "saci-backend/graphics/sb-graphics.h"
#include "./sb-gfx.h"

SA_API void sb_graphics_clear_color(const su_Color color) {
    sb_gfx_clear_color(color);
}

SA_API void sb_graphics_clear_depth_buffer(void) {
    sb_gfx_clear_depth_buffer();
}
