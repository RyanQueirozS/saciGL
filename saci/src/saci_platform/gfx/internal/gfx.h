#ifndef SACI_BACKEND_SB_GFX_H
#define SACI_BACKEND_SB_GFX_H

#include "saci_platform/gfx/graphics.h"

#include "saci_platform/config/config_renderer.h"
#include "saci_util/internal/general.h"
#include "saci_util/types.h"

#define SACI_MAX_INSTANCE_BUFFERS 16

union PSaciGFXUniformValue {
    // Scalar types
    SaciU8 u8;
    SaciU16 u16;
    SaciU32 u32;
    SaciU64 u64;
    SaciS8 s8;
    SaciS16 s16;
    SaciS32 s32;
    SaciS64 s64;
    SaciBool boolean;

    SaciUv uv;
    SaciVec2 vec2;
    SaciVec3 vec3;
    SaciVec4 vec4;

    // Color
    SaciColor color;

    // TODO NEED TO BE ADDED
    SaciMat4 mat4;

    // float mat2[2][2];
    // float mat3[3][3];
    // float mat2x3[2][3];
    // float mat2x4[2][4];
    // float mat3x2[3][2];
    // float mat3x4[3][4];
    // float mat4x2[4][2];
    // float mat4x3[4][3];
};

struct PSaciGFXInfoConstructionData {
    SaciU64 idx_capacity, idx_element_size,
        vtx_capacity, vtx_element_size;

    struct {
        SaciU64 element_array_length;
        struct PSaciGFXInfoVertexAttributeElementData {
            SaciDataType type;
            SaciU32 location;
            SaciU32 offset;
        }* element_array; // Assumes it is in order
        SaciU64 total_size_bytes;
    } vertex_attribute;

    struct PSaciGFXInfoInstanceBufferData {
        SaciU64 total_size_byte;
        SaciU64 layout_array_length;
        struct PSaciGFXInfoInstanceBufferLayoutData {
            SaciDataType type;
            SaciU64 offset;
            SaciU32 location;
        }* layout_array;
        SaciU64 capacity;
    }* instance_buffer_array;
    SaciU8 instance_buffer_array_length;
};

union PSaciGFXInfo {
    struct {
        SaciShaderId shader_program;
        SaciBufferId ibo, vbo, vao;
        SaciBufferId instance_buffer[SACI_MAX_INSTANCE_BUFFERS]; // needs to be a darray of buffers each named with a su_String
    } gl_data;
    // struct {} dx_data;
    // struct {} vk_data;
};

struct PSaciGFXUniformData {
    SaciDataType type;
    SaciS32 location;
    union PSaciGFXUniformValue value;
};

struct PSaciGFXInstanceData {
    SaciU32 location;
    SaciU32 data_size;
    void* instance_data_structure;
};

struct PSaciGFXDrawData {
    struct {
        void* array; // unkown size at compiletime
        SaciU64 struct_size;
        SaciU64 count;
    } vertex_data;

    struct {
        void* array;
        SaciU64 struct_size;
        SaciU64 count;
    } index_data;

    struct PSaciGFXInstanceData* instance_data_array;
    SaciU64 instance_data_array_count;

    struct PSaciGFXUniformData* uniform_data_array;
    SaciU64 uniform_data_array_count;

    union PSaciTexture texture_array[SACI_MAX_TEXTURES];
    SaciU32 texture_array_loc[SACI_MAX_TEXTURES];

    SaciBufferId instance_buffer_id;
};

SaciBool psaci_gfx_load(void);

typedef void* (*PSaciGfxProcAddress)(const char*);
void psaci_gfx_load_proc(PSaciGfxProcAddress addrs);

void psaci_gfx_init_shader(
    union PSaciGFXInfo* info_out,
    const char* vshader_code,
    const char* fshader_code,
    const char* gshader_code);

void psaci_gfx_fill_info(union PSaciGFXInfo* info_out, const struct PSaciGFXInfoConstructionData construction_data);

void psaci_gfx_clear_color(const SaciColor color);

void psaci_gfx_clear_depth_buffer(void);

void psaci_gfx_draw(const union PSaciGFXInfo* gfx_info, const struct PSaciGFXDrawData* data);

SaciS32 psaci_gfx_get_uniform_loc(const union PSaciGFXInfo* info, const char* name);

union PSaciTexture psaci_gfx_gen_texture(void);
void psaci_gfx_upload_texture_2d(union PSaciTexture texture_id,
                                 SaciS32 format,
                                 int width, int height,
                                 const void* data);
void psaci_gfx_get_texture_size_2d(union PSaciTexture texture_id, int* width_out, int* height_out);
void psaci_gfx_generate_mipmap_2d(union PSaciTexture texture_id);
void psaci_gfx_delete_texture(union PSaciTexture texture_id);

void psaci_gfx_initialize_renderer_debugger(void* debug_func);

#endif // SACI_BACKEND_SB_GFX_H
