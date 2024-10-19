#include "saci-core/sc-texture.h"

#include <glad/glad.h>
#include <stdio.h>
#include "saci-utils/su-debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_Bool __sc_isImageLoaded(saci_u8* data);
saci_s32 __sc_determineTextureFormat(int nrChannels);
void __sc_setupTexture(saci_TextureID id, saci_Bool useMipmaps);

//----------------------------------------------------------------------------//

void sc_Texture_LoadData(const char* path, saci_Bool flipImg, sc_TextureData* texData) {
    // @note flipImg is used with a ! operator because stbi automatically flips the image
    stbi_set_flip_vertically_on_load(!flipImg);
    texData->data =
        stbi_load(path, &texData->width, &texData->height, &texData->nrChannels, 0);
    if (texData->width <= 0 || texData->height <= 0) {
        SACI_LOG_PRINT(
            SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
            "Texture coudn't be loaded: Texture Width or Height is equal to 0");
    }
}

saci_TextureID sc_Texture_Load(const char* path, saci_Bool flipImg) {
    sc_TextureData texData = {0};
    sc_Texture_LoadData(path, flipImg, &texData);

    if (!__sc_isImageLoaded(texData.data)) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
                       "Texture coudn't be loaded: Image could not be loaded");
        return 0;
    }

    saci_s32 format = __sc_determineTextureFormat(texData.nrChannels);
    if (format == 0) {
        stbi_image_free(texData.data);
        SACI_LOG_PRINT(SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
                       "Texture coudn't be loaded: Unsupported number of channels");
        return 0;
    }

    saci_TextureID id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    if (texData.width <= 0 || texData.height <= 0) {
        SACI_LOG_PRINT(
            SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
            "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        stbi_image_free(texData.data);
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, texData.width, texData.height, 0, format,
                 GL_UNSIGNED_BYTE, texData.data);

    int glWidth = 0, glHeight = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &glWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &glHeight);

    if (glWidth <= 0 || glHeight <= 0) {
        SACI_LOG_PRINT(
            SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
            "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        stbi_image_free(texData.data);
        return 0;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texData.data);
#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_TEXTURE)
    char debugMsg[256];
    snprintf(debugMsg, sizeof(debugMsg),
             "Loaded texture with %d width, %d height from %s", texData.width,
             texData.height, path);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_DEBUG, SACI_LOG_CONTEXT_OPENGL, debugMsg);
#endif
    return id;
}

void sc_Texture_Free(saci_TextureID textureID) {
#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_TEXTURE)
    char debugMsg[128];
    snprintf(debugMsg, sizeof(debugMsg), "Freed texture, id: %d", textureID);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_DEBUG, SACI_LOG_CONTEXT_OPENGL, debugMsg);
#endif
    glDeleteTextures(1, &textureID);
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_Bool __sc_isImageLoaded(saci_u8* data) {
    if (!data) {
        SACI_LOG_PRINT(
            SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
            "Texture coudn't be laoded: Image data is invalid or couldn't be loaded'");
        return SACI_FALSE;
    }
    return SACI_TRUE;
}

saci_s32 __sc_determineTextureFormat(int nrChannels) {
    if (nrChannels == 3) return GL_RGB;
    if (nrChannels == 4) return GL_RGBA;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
                   "Texture coudn't be laoded: Unsupported number of channels");
    return 0;
}

void __sc_setupTexture(saci_TextureID id, saci_Bool useMipmaps) {
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (useMipmaps) {
        // Ensure dimensions are valid before generating mipmaps
        GLint width, height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        if (width > 0 && height > 0) {
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            SACI_LOG_PRINT(
                SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_OPENGL,
                "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        }
    }
}
