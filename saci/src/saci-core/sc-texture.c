#include "saci-core/sc-texture.h"

#include <glad/glad.h>
#include <stdio.h>
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

/* === Helper Func declarations === */

sa_u32 __sc_Texture_Determine_Format(int nrChannels);

/* === Header impl === */

void sc_Texture_Load_Data(const char* path, sa_bool flipImg, int* width_out, int* height_out, int* nr_channels_out, sa_u8** data_out) {
    // NOTE: flipImg is used with a `!` operator because stbi automatically flips the image
    stbi_set_flip_vertically_on_load(!flipImg);
    *data_out = stbi_load(path, width_out, height_out, nr_channels_out, 0);
    if ((*width_out) <= 0 || (*height_out) <= 0) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Texture Width or Height is equal to 0");
    }
}

sa_textureId sc_Texture_Load(const char* path, sa_bool flipImg) {
    int width = 0;
    int height = 0;
    int nr_channels = 0;
    sa_u8* data = NULL;
    sc_Texture_Load_Data(path, flipImg, &width, &height, &nr_channels, &data);

    if (!data) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Image could not be loaded");
        return 0;
    }

    sa_u32 format = __sc_Texture_Determine_Format(nr_channels);
    if (format == 0) {
        sa_Free_m(data);
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Unsupported number of channels");
        return 0;
    }

    sa_textureId id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, sa_Scast_To_m(int)(format), width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    int glWidth = 0, glHeight = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &glWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &glHeight);

    if (glWidth <= 0 || glHeight <= 0) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        sa_Free_m(data);
        return 0;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    sa_Free_m(data);
    sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_TEXTURE, sa_LOG_CONTEXT_OPENGL, "Loaded texture");
    return id;
}

void sc_Texture_Free(sa_textureId textureID) {
    sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_TEXTURE, sa_LOG_CONTEXT_OPENGL, "Freed texture");
    glDeleteTextures(1, &textureID);
}

/* === Helper Func impl === */

sa_u32 __sc_Texture_Determine_Format(int nrChannels) {
    if (nrChannels == 3)
        return GL_RGB;
    if (nrChannels == 4)
        return GL_RGBA;
    return 0;
}
