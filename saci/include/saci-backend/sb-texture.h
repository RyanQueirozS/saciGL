#ifndef SACI_BACKEND_SB_TEXTURE_H
#define SACI_BACKEND_SB_TEXTURE_H

#include "saci-backend/sb-gfx.h"

#include "saci-utils/su-types.h"

#include <stdint.h>

#define sb_TEXTURE_INVALID (UINT32_MAX)

/* === Texture Loading Functions === */

union sb_GFXTexture sb_texture_load(const char* path, su_Bool flip_img);

void sb_texture_load_data(const char* path, su_Bool flip_img, int* width_out, int* height_out,
                          int* nr_channels_out, su_U8** data_out);

void sb_texture_free(union sb_GFXTexture texture_id);

#endif // SACI_BACKEND_SC_TEXTURE_H
