#ifndef SACI_BACKEND_SB_BACKEND_COMMON_H
#define SACI_BACKEND_SB_BACKEND_COMMON_H

#include "saci-utils/su-types-common.h"

union sb_Texture {
    struct {
        su_TextureId texture;
        su_Bool is_empty;
    } gl_texture;
};

struct sb_DrawInstanceData {
    su_U32 location;
    su_U32 data_size;
    void* instance_data_structure;
};

#endif // SACI_BACKEND_SB_BACKEND_COMMON_H
