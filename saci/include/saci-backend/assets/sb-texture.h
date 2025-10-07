#ifndef SACI_BACKEND_ASSETS_SB_TEXTURE_H
#define SACI_BACKEND_ASSETS_SB_TEXTURE_H

#include "../sb-backend-common.h"

#include "saci-utils/su-types-common.h"

#include <stdint.h>

/* === Texture Loading Functions === */

union sb_Texture sb_texture_load(const char* path, su_Bool flip_img);

void sb_texture_load_data(const char* path, su_Bool flip_img, int* width_out, int* height_out,
                          int* nr_channels_out, su_U8** data_out);

void sb_texture_free(union sb_Texture texture_id);

#endif // SACI_BACKEND_ASSETS_SB_TEXTURE_H
