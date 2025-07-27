#ifndef SACI_BACKEND_SB_TEXTURE_H
#define SACI_BACKEND_SB_TEXTURE_H

#include "saci-utils/su-types.h"

#include <stdint.h>

#define SC_TEXTURE_INVALID (UINT32_MAX)

/* === Texture Loading Functions === */

su_textureId sb_texture_load(const char* path, su_bool flip_img);

void sb_texture_load_data(const char* path, su_bool flip_img, int* width_out, int* height_out,
                          int* nr_channels_out, su_u8** data_out);

void sb_texture_free(su_textureId texture_id);

#endif // SACI_BACKEND_SC_TEXTURE_H
