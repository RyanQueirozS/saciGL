#ifndef SACI_PLATFORM_CONFIG_CONFIG_H
#define SACI_PLATFORM_CONFIG_CONFIG_H

#include "saci_util/defines.h"
#include "saci_util/memory.h"
#include "saci_util/types.h"

/* === Renderer CFG === */

// TODO evaluate if these should be exposed
struct PSaciRendererCfgUniform {
    char* name; // NOT Debug
    SaciDataType type;
    SaciS32 location_internal;
};

struct PSaciRendererCfgSampler {
    char* name; // Debug
    SaciDataType type;
    SaciS32 binding;
    SaciU64 unit;
};

struct PSaciRendererCfgVertexElement {
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
    struct PSaciRendererCfgInstanceBufferLayout* layout_array;
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

struct PSaciConfigRenderer {
    char* name; // Debug
    struct {
        char* frag;
        char* vert;
        char* geom;
    } shaders;
    SaciU64 uniform_array_length;
    struct PSaciRendererCfgUniform* uniform_array;
    SaciU64 sampler_array_length;
    struct PSaciRendererCfgSampler* sampler_array;

    struct PSaciRendererCfgVertexAttributes {
        SaciU64 element_size_internal;
        SaciU64 element_array_length;
        struct PSaciRendererCfgVertexElement* element_array;
    } vertex_attributes;

    struct {
        SaciU64 element_size_internal;
    } index_data;

    struct {
        SaciU64 buffer_array_length;
        struct PSaciRendererCfgInstanceBuffer* buffer_array;
    } instance_data;

    struct PSaciRendererCfgBatch batch;
    struct PSaciRendererCfgBound bound;
    struct PSaciRendererCfgDraw draw;
    struct PSaciRendererCfgPipeline pipeline;
};

SACI_API void psaci_cfg_renderer_get(const char* name, struct PSaciConfigRenderer* cfg_out, const char* cfg_file_path);

SACI_API SaciU64 psaci_cfg_renderer_cfg_size(const struct PSaciConfigRenderer* cfg);

SACI_API void psaci_cfg_renderer_cleanup(struct PSaciConfigRenderer* cfg);

enum {
    PSACI_CONFIG_RENDERER_PRINT_OPT_SHADERS = 1 << 0,
    PSACI_CONFIG_RENDERER_PRINT_OPT_UNIFORMS = 1 << 1,
    PSACI_CONFIG_RENDERER_PRINT_OPT_SAMPLERS = 1 << 2,
    PSACI_CONFIG_RENDERER_PRINT_OPT_VERTEX_ATTRIB = 1 << 3,
    PSACI_CONFIG_RENDERER_PRINT_OPT_INDEX_DATA = 1 << 4,
    PSACI_CONFIG_RENDERER_PRINT_OPT_INSTANCE_DATA = 1 << 5,
    PSACI_CONFIG_RENDERER_PRINT_OPT_BATCH = 1 << 6,
    PSACI_CONFIG_RENDERER_PRINT_OPT_BOUND = 1 << 7,
    PSACI_CONFIG_RENDERER_PRINT_OPT_DRAW_DATA = 1 << 8,
    PSACI_CONFIG_RENDERER_PRINT_OPT_PIPELINE = 1 << 9,
};

SACI_API void psaci_cfg_renderer_print(const struct PSaciConfigRenderer* cfg, int print_opts);

#endif // SACI_PLATFORM_CONFIG_CONFIG_H
