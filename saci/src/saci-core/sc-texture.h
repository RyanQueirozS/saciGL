#ifndef SACI_CORE_SC_TEXTURE_H
#define SACI_CORE_SC_TEXTURE_H

#include "saci-utils/su-types.h"

/* === Texture Loading Functions === */

sa_textureId sc_Texture_Load(const char* path, sa_bool flip_img);

void sc_Texture_Load_Data(const char* path, sa_bool flip_img, int* width_out, int* height_out,
                          int* nr_channels_out, sa_u8** data_out);

void sc_Texture_Free(sa_textureId texture_id);

#endif // SACI_CORE_SC_TEXTURE_H
