#ifndef __SACI_CORE_SC_TEXTURE_H__
#define __SACI_CORE_SC_TEXTURE_H__

#include "saci-utils/su-types.h"

typedef struct sc_TextureData {
    int width, height;
    int nrChannels;
    uint8_t* data;
} sc_TextureData;

saci_u8* sc_TextureLoadData(const char* path, saci_Bool flipImg, sc_TextureData* texData);
saci_TextureID sc_TextureLoad(const char* path, saci_Bool flipImg);
void sc_TextureFree(saci_TextureID textureID);

#endif
