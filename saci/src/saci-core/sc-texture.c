#include "saci-core/sc-texture.h"

#include <glad/glad.h>
#include <stdio.h>

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
    saci_u8* data = stbi_load(path, &texData->width, &texData->height, &texData->nrChannels, 0);
    assert(texData->width > 0);
    assert(texData->height > 0);
    return data;
}
saci_TextureID sc_TextureLoad(const char* path, saci_Bool flipImg) {
    sc_TextureData texData = {0};
    texData.data = sc_TextureLoadData(path, flipImg, &texData);

    if (!__sc_isImageLoaded(texData.data)) {
        return 0;
    }

    saci_s32 format = __sc_determineTextureFormat(texData.nrChannels);
    if (format == 0) {
        stbi_image_free(texData.data);
        printf("Error: Unsupported number of channels: %d\n", texData.nrChannels);
        return 0;
    }

    saci_TextureID id;
    glGenTextures(1, &id);

    // Ensure the texture is properly bound
    glBindTexture(GL_TEXTURE_2D, id);

    // Ensure the texture dimensions are valid
    if (texData.width <= 0 || texData.height <= 0) {
        printf("Error: Invalid texture dimensions: width=%d, height=%d\n", texData.width, texData.height);
        stbi_image_free(texData.data);
        return 0;
    }

    // Upload the texture data to OpenGL
    glTexImage2D(
        GL_TEXTURE_2D, 0, format,
        texData.width, texData.height, 0,
        format, GL_UNSIGNED_BYTE, texData.data);

    // Check for OpenGL errors after setting texture data
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("OpenGL error: %x after glTexImage2D\n", err);
        stbi_image_free(texData.data);
        return 0;
    }

    // Verify texture dimensions in OpenGL after glTexImage2D
    GLint glWidth = 0, glHeight = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &glWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &glHeight);

    if (glWidth == 0 || glHeight == 0) {
        printf("Error: OpenGL texture dimensions are zero after glTexImage2D. Width=%d, Height=%d\n", glWidth, glHeight);
        stbi_image_free(texData.data);
        return 0;
    }

    // Safely generate mipmaps only if dimensions are valid
    glGenerateMipmap(GL_TEXTURE_2D);
    err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        printf("Error: GL_INVALID_OPERATION when generating mipmaps\n");
    } else if (err != GL_NO_ERROR) {
        printf("OpenGL error: %x after glGenerateMipmap\n", err);
    }
    stbi_image_free(texData.data);
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
        printf("Error: Image data not loaded correctly.\n");
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (useMipmaps) {
        // Ensure dimensions are valid before generating mipmaps
        GLint width, height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        if (width > 0 && height > 0) {
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            printf("Warning: Cannot generate mipmaps for zero-sized base image. Width=%d, Height=%d\n", width, height); // TODO Debug
        }
    }
}
