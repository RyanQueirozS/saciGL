/**
 * @file sc_texture.h
 * @brief This header defines functions and structures for loading and managing textures.
 *
 * @details
 * This file includes the necessary functions and structures for loading texture data from
 * a file, managing texture memory, and providing utilities to handle textures in the SACI
 * engine.
 */

#ifndef __SACI_CORE_SC_TEXTURE_H__
#define __SACI_CORE_SC_TEXTURE_H__

#include "saci-utils/su-types.h"

/* === Texture Loading Functions === */

sa_textureId sc_Texture_Load(const char* path, sa_bool_t flipImg);

void         sc_Texture_Load_Data(const char* path, sa_bool_t flipImg, int* width_out, int* height_out, int* nr_channels_out, sa_u8_t** data_out);

void         sc_Texture_Free(sa_textureId textureID);

#endif
