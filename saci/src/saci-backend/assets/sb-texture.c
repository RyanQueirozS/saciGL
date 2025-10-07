#include "saci-backend/assets/sb-texture.h"

#include "saci-utils/su-log.h"
#include "saci-utils/su-general.h"
#include "saci-backend/graphics/sb-gfx.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

#include <stdio.h>

enum TextureFormat {
    TEX_FORMAT_NONE = 0,
    TEX_FORMAT_RGB = 0x1907,  // GL_RGB
    TEX_FORMAT_RGBA = 0x1908, // GL_RGBA
};

enum su_GL_Enums {
    SU_GL_TEXTURE_2D = 0x0DE1,
    SU_GL_RGB = 0x1907,
    SU_GL_RGBA = 0x1908,
    SU_GL_UNSIGNED_BYTE = 0x1401,
    SU_GL_TEXTURE_WIDTH = 0x1000,
    SU_GL_TEXTURE_HEIGHT = 0x1001,
};

/* === Helper Func declarations === */

SA_INTERNAL su_U32 sb__texture_determine_format(int nr_channels);

/* === Header impl === */

void sb_texture_load_data(const char* path, su_Bool flip_img, int* width_out, int* height_out, int* nr_channels_out, su_U8** data_out) {
    // NOTE: flipImg is used with a `!` operator because stbi automatically flips the image
    stbi_set_flip_vertically_on_load(!flip_img);
    *data_out = stbi_load(path, width_out, height_out, nr_channels_out, 0);
    if ((*width_out) <= 0 || (*height_out) <= 0) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_TEXTURE_LOADING,
                       "Texture coudn't be loaded: Texture Width or Height is equal to 0");
    }
}

union sb_Texture sb_texture_load(const char* path, su_Bool flip_img) {
    int width = 0;
    int height = 0;
    int nr_channels = 0;
    su_U8* data = NULL;
    sb_texture_load_data(path, flip_img, &width, &height, &nr_channels, &data);

    if (!data) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_TEXTURE_LOADING,
                       "Texture coudn't be loaded: Image could not be loaded");
        return (union sb_Texture){0};
    }

    su_U32 format = sb__texture_determine_format(nr_channels);
    if (format == 0) {
        free(data);
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_TEXTURE_LOADING,
                       "Texture coudn't be loaded: Unsupported number of channels");
        return (union sb_Texture){0};
    }

    union sb_Texture tex;
    tex = sb_gfx_gen_texture();

    sb_gfx_upload_texture_2d(tex, su_CAST_M(int)(format), width, height, data);

    sb_gfx_get_texture_size(tex, &width, &height);

    if (width <= 0 || height <= 0) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_TEXTURE_LOADING,
                       "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        free(data);
        return tex;
    }

    sb_gfx_generate_mipmap(tex);

    free(data);
    su_LOG_INFO_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_TEXTURE_LOADING, "Loaded texture");
    return tex;
}

void sb_texture_free(union sb_Texture texture) {
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_TEXTURE_LOADING, "Freed texture");
    sb_gfx_delete_texture(texture);
}

/* === Helper Func impl === */

su_U32 sb__texture_determine_format(int nr_channels) {
    if (nr_channels == 3) {
        return SU_GL_RGB;
    }
    if (nr_channels == 4) {
        return SU_GL_RGBA;
    }
    return 0;
}
