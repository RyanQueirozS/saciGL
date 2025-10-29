#ifndef SACI_BACKEND_SB_GFX_H
#define SACI_BACKEND_SB_GFX_H

#include "saci-utils/config/su-config-manager.h"
#include "saci-utils/memory/su-darray.h"
#include "saci-utils/memory/su-string.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"

#include "saci-backend/sb-backend-common.h"
#include "saci-utils/math/su-math-types.h"

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

union sb_GFXInfo {
    struct {
        su_ShaderId shader_program;
        su_BufferId ibo, vbo, vao;
        su_BufferId instance_buffer; // needs to be a darray of buffers each named with a su_String
    } gl_data;
    // struct {} dx_data;
    // struct {} vk_data;
};

struct sb_GFXUniformData {
    su_DataType type;
    su_S32 location;
    union sb_GFXUniformValue value;
};

struct sb_GFXInstanceData {
    su_U32 location;
    su_U32 data_size;
    void* instance_data_structure;
};

struct sb_GFXDrawData {
    su_DArray* vertex_array; // Darray<unkown_size_at_compiletime>
    su_U64 vertex_struct_size;

    su_DArray* index_array; // DArray<unkown_size_at_compiletime>
    su_U64 index_struct_size;

    su_DArray* instance_data_array; // DArray<GFXInstanceData>

    su_DArray* uniform_data_array; // DArray<GFXUniformData>

    union sb_Texture texture_array[SACI_MAX_TEXTURES];
    su_U32 texture_array_loc[SACI_MAX_TEXTURES];
};

void sb_gfx_load(void);

typedef void* (*sb_GfxProcAddress)(const char*);
void sb_gfx_load_proc(sb_GfxProcAddress addrs);

void sb_gfx_init_shader(union sb_GFXInfo* info_out, const struct su_RendererConfig cfg);

void sb_gfx_create(union sb_GFXInfo* info_out, const struct su_RendererConfig cfg);

void sb_gfx_clear_color(const su_Color color);

void sb_gfx_clear_depth_buffer(void);

void sb_gfx_load_debugger(void* debug_func);

void sb_gfx_draw(const union sb_GFXInfo* gfx_info, const struct sb_GFXDrawData* data);

su_S32 sb_gfx_get_uniform_loc(const union sb_GFXInfo* info, const su_String* name);

su_S32 sb_gfx_get_uniform_loc_cstr(const union sb_GFXInfo* info, const char* name);

union sb_Texture sb_gfx_gen_texture(void);
void sb_gfx_upload_texture_2d(union sb_Texture texture_id,
                              su_S32 format,
                              int width, int height,
                              const void* data);
void sb_gfx_get_texture_size(union sb_Texture texture_id, int* width_out, int* height_out);
void sb_gfx_generate_mipmap(union sb_Texture texture_id);
void sb_gfx_delete_texture(union sb_Texture texture_id);

#endif // SACI_BACKEND_SB_GFX_H
