#include "saci-core/sc-texture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_Bool __sc_isImageLoaded(saci_u8* data);
saci_s32 __sc_determineTextureFormat(int nrChannels);
void __sc_setupTexture(saci_TextureID id, saci_Bool useMipmaps);

//----------------------------------------------------------------------------//

saci_u8* sc_TextureLoadData(const char* path, saci_Bool flipImg, sc_TextureData* texData) {
    stbi_set_flip_vertically_on_load(!flipImg);
    return stbi_load(path, &texData->width, &texData->height, &texData->nrChannels, 0);
}

saci_TextureID sc_TextureLoad(const char* path, saci_Bool flipImg) {
    sc_TextureData texData = {0};
    saci_u8* data = sc_TextureLoadData(path, flipImg, &texData);
    if (!__sc_isImageLoaded(data)) {
        return 0;
    }

    saci_TextureID id;
    glGenTextures(1, &id);

    saci_s32 format = __sc_determineTextureFormat(texData.nrChannels);
    if (format == 0) {
        stbi_image_free(data);
        return 0; // Unsupported format
    }

    __sc_setupTexture(id, SACI_FALSE);

    glTexImage2D(GL_TEXTURE_2D, 0, (texData.nrChannels == 4) ? GL_RGBA8 : GL_RGB8, texData.width, texData.height, 0, format, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texData.width, texData.height, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return id;
}

void sc_TextureFree(saci_TextureID textureID) {
    glDeleteTextures(1, &textureID);
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_Bool __sc_isImageLoaded(saci_u8* data) {
    if (!data) {
        // Log error or handle failure
        return SACI_FALSE;
    }
    return SACI_TRUE;
}

saci_s32 __sc_determineTextureFormat(int nrChannels) {
    if (nrChannels == 3) return GL_RGB;
    if (nrChannels == 4) return GL_RGBA;
    return 0; // Unsupported format
}

void __sc_setupTexture(saci_TextureID id, saci_Bool useMipmaps) {
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (useMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}
