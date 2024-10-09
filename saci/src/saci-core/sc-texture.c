#include "saci-core/sc-texture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>

sc_TextureData sc_LoadImage(const char* path) {
    sc_TextureData tex = {0};
    tex.data = stbi_load(path, &tex.width, &tex.height, &tex.nrChannels, 0);
    return tex;
}

saci_TextureID sc_BindTexture2D(sc_TextureData textureData, int mipmapLevel) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, mipmapLevel, GL_RGB, textureData.width, textureData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}

void sc_FreeTextureData(sc_TextureData textureData) {
    stbi_image_free(textureData.data);
}
