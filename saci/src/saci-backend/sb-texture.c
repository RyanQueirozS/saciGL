#include "saci-backend/sb-texture.h"

#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-backend/sb-config-manager.h"
#include <saci-backend/sb-gfx.h>

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
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Texture Width or Height is equal to 0");
    }
}

union sb_GFXTexture sb_texture_load(const char* path, su_Bool flip_img) {
    int width = 0;
    int height = 0;
    int nr_channels = 0;
    su_U8* data = NULL;
    sb_texture_load_data(path, flip_img, &width, &height, &nr_channels, &data);

    if (!data) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Image could not be loaded");
        return (union sb_GFXTexture){0};
    }

    su_U32 format = sb__texture_determine_format(nr_channels);
    if (format == 0) {
        su_FREE_M(data);
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Unsupported number of channels");
        return (union sb_GFXTexture){0};
    }

    union sb_GFXTexture tex;
    tex = sb_gfx_gen_texture();

    sb_gfx_upload_texture_2d(tex, su_SCAST_TO_M(int)(format), width, height, data);

    sb_gfx_get_texture_size(tex, &width, &height);

    if (width <= 0 || height <= 0) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_OPENGL,
                             "Texture coudn't be loaded: Texture Width or Height is equal to 0");
        su_FREE_M(data);
        return tex;
    }

    sb_gfx_generate_mipmap(tex);

    su_FREE_M(data);
    su_LOG_DEBUG_PRINT_M(su_LOG_DEBUG_TYPE_TEXTURE, su_LOG_CONTEXT_OPENGL, "Loaded texture");
    return tex;
}

void sb_texture_free(union sb_GFXTexture texture) {
    su_LOG_DEBUG_PRINT_M(su_LOG_DEBUG_TYPE_TEXTURE, su_LOG_CONTEXT_OPENGL, "Freed texture");
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
