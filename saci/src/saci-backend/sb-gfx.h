#ifndef SACI_BACKEND_SB_GFX_H
#define SACI_BACKEND_SB_GFX_H

#include "saci-utils/su-types.h"

union sb_GFXUniformValue {
    // Scalar types
    su_U8 u8;
    su_U16 u16;
    su_U32 u32;
    su_U64 u64;
    su_S8 s8;
    su_S16 s16;
    su_S32 s32;
    su_S64 s64;
    su_Bool boolean;

    su_Uv uv;
    su_Vec2 vec2;
    su_Vec3 vec3;
    su_Vec4 vec4;

    // Color
    su_Color color;

    // TODO NEED TO BE ADDED
    su_Mat4 mat4;

    // float mat2[2][2];
    // float mat3[3][3];
    // float mat2x3[2][3];
    // float mat2x4[2][4];
    // float mat3x2[3][2];
    // float mat3x4[3][4];
    // float mat4x2[4][2];
    // float mat4x3[4][3];
};

struct sb_GFXUniformData {
    su_DataType type;
    su_S32 location;
    union sb_GFXUniformValue value;
};

union sb_GFXInfo {
    struct {
        su_ShaderId shader_program;
        su_BufferId ibo, vbo, vao;
    } gl_data;
    // struct {} dx_data;
    // struct {} vk_data;
};

union sb_GFXProperties {
    struct {
        su_String* shader_code;
    } gl_data;
};

struct sb_GFXDrawData {
    const su_DArray* vertex_array;
    su_U64 vertex_struct_size;

    const su_DArray* index_array;
    su_U64 index_struct_size;

    const su_DArray* instance_array_array;
    const su_DArray* instance_location_array;
    su_U64 instance_struct_size;

    const su_DArray* uniform_array_array;
    const su_DArray* uniform_location_array;

    su_TextureId texture_array_array[SACI_MAX_TEXTURES];
    su_U32 texture_array_loc[SACI_MAX_TEXTURES];
};

void sb_gfx_load(void);

void sb_gfx_create(union sb_GFXInfo* info_out, const union sb_GFXProperties props);

void sb_gfx_draw(union sb_GFXInfo gfx_info, const struct sb_GFXDrawData* data);

#endif // SACI_BACKEND_SB_GFX_H
