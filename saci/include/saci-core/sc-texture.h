#ifndef __SACI_CORE_SC_TEXTURE_H__
#define __SACI_CORE_SC_TEXTURE_H__

#include "saci-utils/su-types.h"

typedef struct sc_TextureData {
    int width, height;
    int nrChannels;
    uint8_t* data;
} sc_TextureData;

sc_TextureData sc_LoadImage(const char* path);

saci_TextureID sc_BindTexture2D(sc_TextureData textureData, int mipmapLevel);

void sc_FreeTextureData(sc_TextureData textureData);

#endif
