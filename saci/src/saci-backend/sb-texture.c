#include "saci-backend/sb-texture.h"

#include <glad/glad.h>
#include <stdio.h>
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

/* === Helper Func declarations === */

SA_INTERNAL su_U32 sb__texture_determine_format(int nr_channels);

/* === Header impl === */

void sb_texture_load_data(const char* path, su_Bool flip_img, int* width_out, int* height_out, int* nr_channels_out, su_U8** data_out) {
    // NOTE: flipImg is used with a `!` operator because stbi automatically flips the image
    stbi_set_flip_vertically_on_load(!flip_img);
    *data_out = stbi_load(path, width_out, height_out, nr_channels_out, 0);
    if ((*width_out) <= 0 || (*height_out) <= 0) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Texture Width or Height is equal to 0");
    }
}

su_TextureId sb_texture_load(const char* path, su_Bool flip_img) {
    int width = 0;
    int height = 0;
    int nr_channels = 0;
    su_U8* data = NULL;
    sb_texture_load_data(path, flip_img, &width, &height, &nr_channels, &data);

    if (!data) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Image could not be loaded");
        return 0;
    }

    su_U32 format = sb__texture_determine_format(nr_channels);
    if (format == 0) {
        su_FREE_M(data);
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Unsupported number of channels");
        return 0;
    }

    su_TextureId id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, su_SCAST_TO_M(int)(format), width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    int gl_width = 0, gl_height = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &gl_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &gl_height);

    if (gl_width <= 0 || gl_height <= 0) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        su_FREE_M(data);
        return 0;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    su_FREE_M(data);
    su_LOG_DEBUG_PRINT_M(su_LOG_DEBUG_TYPE_TEXTURE, su_LOG_CONTEXT_OPENGL, "Loaded texture");
    return id;
}

void sb_texture_free(su_TextureId texture_id) {
    su_LOG_DEBUG_PRINT_M(su_LOG_DEBUG_TYPE_TEXTURE, su_LOG_CONTEXT_OPENGL, "Freed texture");
    glDeleteTextures(1, &texture_id);
}

/* === Helper Func impl === */

su_U32 sb__texture_determine_format(int nr_channels) {
    if (nr_channels == 3) {
        return GL_RGB;
    }
    if (nr_channels == 4) {
        return GL_RGBA;
    }
    return 0;
}
