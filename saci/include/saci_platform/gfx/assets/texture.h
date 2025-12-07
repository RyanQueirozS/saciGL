#ifndef SACI_BACKEND_ASSETS_SB_TEXTURE_H
#define SACI_BACKEND_ASSETS_SB_TEXTURE_H

#include "saci_platform/gfx/graphics.h"

#include "saci_util/defines.h"
#include "saci_util/types.h"

#include <stdint.h>

/* === Texture Loading Functions === */

SACI_API union PSaciTexture psaci_texture_load(const char* path, SaciBool flip_img);

SACI_API void psaci_texture_load_data(const char* path, SaciBool flip_img, int* width_out, int* height_out,
                                      int* nr_channels_out, SaciU8** data_out);

SACI_API void psaci_texture_free(union PSaciTexture texture_id);

#endif // SACI_BACKEND_ASSETS_SB_TEXTURE_H
