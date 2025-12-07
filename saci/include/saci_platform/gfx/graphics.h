#ifndef SACI_BACKEND_RENDERER_SB_GRAPHICS_H
#define SACI_BACKEND_RENDERER_SB_GRAPHICS_H

#include "saci_util/defines.h"
#include "saci_util/types.h"

union PSaciTexture {
    struct {
        SaciU32 texture;
        SaciBool is_empty;
    } gl;
};

SACI_API void psaci_graphics_clear_color(const SaciColor color);

SACI_API void psaci_graphics_clear_depth_buffer(void);

#endif // SACI_BACKEND_RENDERER_SB_GRAPHICS_H
