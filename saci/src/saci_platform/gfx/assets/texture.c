// TODO redo this whole file
#include "saci_platform/gfx/assets/texture.h"

#include "saci_util/log.h"
#include "saci_util/internal/log.h"
#include "saci_util/internal/general.h"

#include "saci_platform/gfx/internal/gfx.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

#include <stdio.h>

enum PSaciTextureFormat {
    PSACI_TEX_FORMAT_NONE = 0,
    PSACI_TEX_FORMAT_RGB = 0x1907,  // GL_RGB
    PSACI_TEX_FORMAT_RGBA = 0x1908, // GL_RGBA
};

enum PSaciGLEnums {
    PSACI_GL_TEXTURE_2D = 0x0DE1,
    PSACI_GL_RGB = 0x1907,
    PSACI_GL_RGBA = 0x1908,
    PSACI_GL_UNSIGNED_BYTE = 0x1401,
    PSACI_GL_TEXTURE_WIDTH = 0x1000,
    PSACI_GL_TEXTURE_HEIGHT = 0x1001,
};

/* === Helper Func declarations === */

SACI_INTERNAL SaciU32 psaci__texture_determine_format(int nr_channels);

/* === Header impl === */

void psaci_texture_load_data(const char* path, SaciBool flip_img, int* width_out, int* height_out, int* nr_channels_out, SaciU8** data_out)
{
    // NOTE: flipImg is used with a `!` operator because stbi automatically flips the image
    stbi_set_flip_vertically_on_load(!flip_img);
    *data_out = stbi_load(path, width_out, height_out, nr_channels_out, 0);
    if ((*width_out) <= 0 || (*height_out) <= 0) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_TEXTURE_LOADING,
                         "Texture coudn't be loaded: Texture Width or Height is equal to 0");
    }
}

union PSaciTexture psaci_texture_load(const char* path, SaciBool flip_img)
{
    int width = 0;
    int height = 0;
    int nr_channels = 0;
    SaciU8* data = NULL;
    psaci_texture_load_data(path, flip_img, &width, &height, &nr_channels, &data);

    if (!data) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_TEXTURE_LOADING,
                         "Texture coudn't be loaded: Image could not be loaded");
        return (union PSaciTexture){0};
    }

    SaciU32 format = psaci__texture_determine_format(nr_channels);
    if (format == 0) {
        free(data);
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH, SACI_LOG_CONTEXT_TEXTURE_LOADING,
                         "Texture coudn't be loaded: Unsupported number of channels");
        return (union PSaciTexture){0};
    }

    union PSaciTexture tex;
    tex = psaci_gfx_gen_texture();

    psaci_gfx_upload_texture_2d(tex, SACI_CAST_M(int)(format), width, height, data);

    psaci_gfx_get_texture_size_2d(tex, &width, &height);

    if (width <= 0 || height <= 0) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_TEXTURE_LOADING,
                         "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        free(data);
        return tex;
    }

    psaci_gfx_generate_mipmap_2d(tex);

    free(data);
    SACI_LOG_INFO_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_TEXTURE_LOADING, "Loaded texture");
    return tex;
}

void psaci_texture_free(union PSaciTexture texture)
{
    SACI_LOG_INFO_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_TEXTURE_LOADING, "Freed texture");
    psaci_gfx_delete_texture(texture);
}

/* === Helper Func impl === */

SaciU32 psaci__texture_determine_format(int nr_channels)
{
    if (nr_channels == 3) {
        return PSACI_GL_RGB;
    }
    if (nr_channels == 4) {
        return PSACI_GL_RGBA;
    }
    return 0;
}
