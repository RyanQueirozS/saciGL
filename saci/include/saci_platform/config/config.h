#ifndef SACI_PLATFORM_CONFIG_CONFIG_H
#define SACI_PLATFORM_CONFIG_CONFIG_H

#include "saci_util/types.h"

enum PSaciRenderApi {
    PSACI_RENDERER_API_OPENGL = 0,
    PSACI_RENDERER_API_VULKAN,
};

enum PSaciRenderApiLoader {
    PSACI_RENDERER_LOADER_GLAD = 0,
};

enum PSaciWindowApi {
    PSACI_WINDOW_API_GLFW = 0,
};

struct PSaciRendererCfgUniform {
    char* name; // Debug
    SaciDataType type;
    SaciS32 location;
};

struct PSaciRendererCfgSampler {
    char* name; // Debug
    SaciDataType type;
    SaciS32 binding;
    SaciU64 unit;
};

struct PSaciRendererCfgVertexLayout {
    char* name;
    SaciDataType type;
    SaciU64 offset;
    SaciU32 location;
};

struct PSaciRendererCfgInstanceBufferLayout {
    char* name;
    SaciDataType type;
    SaciU64 offset;
    SaciU32 location;
};

struct PSaciRendererCfgInstanceBuffer {
    char* name;
    SaciU64 size_byte_internal;
    SaciU64 layout_array_length;
    struct su_RendererCfgInstanceBufferLayout* layout_array;
};

struct PSaciRendererCfgBatch {
    SaciU64 capacity;
    SaciBool fixed_capacity;

    struct {
        SaciU64 capacity;
    } index_cfg;
    struct {
        SaciU64 capacity;
    } vertex_cfg;
    struct {
        SaciU64 capacity;
    } instance_cfg;
};

struct PSaciRendererCfgBound {
    struct {
        SaciU64 capacity;
    } index_cfg;
    struct {
        SaciU64 capacity;
    } instance_cfg;
};

enum PSaciRendererPrimitives {
    PSACI_PRIMITIVES_TRIANGLES = 1,
};

enum PSaciRendererCullMode {
    PSACI_RENDERER_CULL_MODE_BACK = 1,
};

enum PSaciRendererFrontFace {
    PSACI_RENDERER_FRONT_FACE_CCW = 1,
};

struct PSaciRendererCfgDraw {
    enum PSaciRendererPrimitives primitive;
    enum PSaciRendererCullMode cull_mode;
    enum PSaciRendererFrontFace front_face;
};

struct PSaciRendererCfgBlend {
    SaciBool enabled;
    SaciU64 opts;
};

struct PSaciRendererCfgPipeline {
    SaciBool depth_test;
    struct PSaciRendererCfgBlend blend;
};

struct PSaciRendererConfig {
    char* name; // Debug
    struct {
        char* frag;
        char* vert;
        char* geom;
    } shaders;
    SaciU64 uniform_array_length;
    struct su_RendererCfgUniform* uniform_array;
    SaciU64 sampler_array_length;
    struct su_RendererCfgSampler* sampler_array;

    struct PSaciRendererCfgVertexData {
        SaciU64 element_size_internal;
        SaciU64 layout_array_length;
        struct su_RendererCfgVertexLayout* layout_array;
    } vertex_data;

    struct {
        SaciU64 element_size_internal;
    } index_data;

    struct {
        SaciU64 buffer_array_length;
        struct su_RendererCfgInstanceBuffer* buffer_array;
    } instance_data;

    struct PSaciRendererCfgBatch batch;
    struct PSaciRendererCfgBound bound;
    struct PSaciRendererCfgDraw draw;
    struct PSaciRendererCfgPipeline pipeline;
};

#endif // SACI_PLATFORM_CONFIG_CONFIG_H
