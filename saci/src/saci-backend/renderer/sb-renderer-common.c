#include "./sb-renderer-common.h"

sb_Renderer* sb_renderer_new(const enum sb_RendererType type) {
    sb_Renderer* rendr = malloc(sizeof(struct sb_Renderer));
    rendr->type = type;
    switch (type) {
    case sb_RENDERER_STATIC:
    case sb_RENDERER_DYNAMIC:
    case sb_RENDERER_INSTANCE:
        rendr->type = sb_RENDERER_INSTANCE;
        rendr->interface = &sb__RENDERER_INSTANCE_INTERFACE_DEFAULT_INITIALIZER;
        rendr->interface->new(rendr);
        break;
    }
    return rendr;
}

void sb_renderer_begin(struct sb_Renderer* rendr) {
    rendr->interface->begin(rendr);
}

void sb_renderer_bind_texture(struct sb_Renderer* rendr, su_TextureId tex_id) {
    rendr->interface->bind_texture(rendr, tex_id);
}

su_S32 sb_renderer_get_uniform_id(struct sb_Renderer* rendr,
                                  const char* const uniform_name) {
    return rendr->interface->get_uniform_id(rendr, uniform_name);
}

void sb_renderer_set_uniform(struct sb_Renderer* rendr,
                             const su_S32 uniform_id,
                             const void* const value,
                             const su_DataType type) {
    rendr->interface->set_uniform(rendr, uniform_id, value, type);
}

void sb_renderer_bind_index_buffer(struct sb_Renderer* rendr,
                                   const su_DArray* new_indices) {
    rendr->interface->bind_index_buffer(rendr, new_indices);
}

void sb_renderer_push_mesh(struct sb_Renderer* rendr,
                           const su_DArray* pos_array,
                           const su_DArray* uv_array,
                           const su_DArray* color_array) {
    rendr->interface->push_mesh(rendr, pos_array, uv_array, color_array);
}

void sb_renderer_draw(const struct sb_Renderer* rendr) {
    rendr->interface->draw(rendr);
}

void sb_renderer_free(struct sb_Renderer* rendr) {
    rendr->interface->free(rendr);
}

void sb_renderer_free_opts(struct sb_Renderer* rendr, int free_opts) {
    rendr->interface->free_opts(rendr, free_opts);
}

void sb_renderer_init_bound(struct sb_RendererBound* bound_out, const struct su_RendererConfig cfg) {
    bound_out->index_array = su_darray_create(
        cfg.bound.index_cfg.capacity,
        sizeof(su_U32),
        cfg.batch.index_cfg.fixed_size);

    // TODO
    bound_out->uniform_data_array = su_darray_create(
        cfg.uniform_array_length,
        sizeof(struct sb_GFXUniformData),
        cfg.batch.index_cfg.fixed_size);
}

void sb_init_uniforms(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
    SA_INTERNAL struct su_RendererCfgUniform uniform_array[6] = {
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_MAT4},
        {.name = NULL, .location = 0, .type = su_TYPE_S32},
        {.name = NULL, .location = 0, .type = su_TYPE_VEC4},
        {.name = NULL, .location = 0, .type = su_TYPE_VEC4},
    };

    if (!uniform_array[0].name) {
        uniform_array[0].name = "u_model_matrix";
        uniform_array[1].name = "u_view_matrix";
        uniform_array[2].name = "u_projection_matrix";
        uniform_array[3].name = "u_flags";
        uniform_array[4].name = "u_lighting";
        uniform_array[5].name = "u_use_texture";

        for (su_U64 i = 0; i < su_ARRLEN_M(uniform_array); ++i) {
            uniform_array[i].location = sb_gfx_get_uniform_loc_cstr(gfx_info, uniform_array[i].name);
        }
    }

    if (!cfg_out->uniform_array) {
        // TODO
#if 0
        cfg_out->uniform_array = su_darray_create(
            su_ARRLEN_M(uniform_array),
            sizeof(struct su_RendererCfgUniform),
            su_TRUE);
        for (su_U64 i = 0; i < su_ARRLEN_M(uniform_array); ++i) {
            su_darray_push(cfg_out->uniform_array, &uniform_array[i]);
        }
#endif
    }
}

void sb_init_samplers(struct su_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
    SA_INTERNAL struct su_RendererCfgSampler sampler_array[1] = {
        {.name = NULL, .binding = 0, .type = su_TYPE_SAMPLER2D}};

    if (!sampler_array[0].name) {
        sampler_array[0].name = "u_texture";
        sampler_array[0].binding = sb_gfx_get_uniform_loc_cstr(gfx_info, sampler_array[0].name);
        sampler_array[0].unit = 0;
    }

    if (!cfg_out->sampler_array) {
        // cfg_out->sampler_array = su_darray_create(
        //     su_ARRLEN_M(sampler_array),
        //     sizeof(struct su_RendererCfgSampler),
        //     su_TRUE);
        // for (su_U64 i = 0; i < su_ARRLEN_M(sampler_array); ++i) {
        //     su_darray_push(cfg_out->sampler_array, &sampler_array[i]);
        // }
    }
}

void sb_init_vertex_layout(struct su_RendererConfig* cfg_out) {
    SA_STATIC struct su_RendererCfgVertexLayout v_layout[3] = {
        {.name = "pos", .type = su_TYPE_VEC3, .location = 0, .offset = 0},
        {.name = "color", .type = su_TYPE_VEC4, .location = 1, .offset = 12},
        {.name = "uv", .type = su_TYPE_UV, .location = 2, .offset = 38},
    };
    su_U64 size_of_elements =
        su_SIZE_OF_TYPE[v_layout[0].type] +
        su_SIZE_OF_TYPE[v_layout[1].type] +
        su_SIZE_OF_TYPE[v_layout[2].type];

    cfg_out->vertex_data.layout_array = v_layout;
    cfg_out->vertex_data.layout_array_length = su_ARRLEN_M(v_layout);
    cfg_out->vertex_data.element_size_internal = size_of_elements;
}

void sb_init_shaders(struct su_RendererConfig* cfg, union sb_GFXInfo* info_out) {
    sb_gfx_init_shader(info_out, *cfg);
}

union sb_GFXUniformValue sb_renderer_uniform_value_from_type(su_DataType type, const void* value) {
    union sb_GFXUniformValue result = {0};

    switch (type) {
    case su_TYPE_U8:
        result.u8 = *(const su_U8*)value;
        break;
    case su_TYPE_U16:
        result.u16 = *(const su_U16*)value;
        break;
    case su_TYPE_U32:
        result.u32 = *(const su_U32*)value;
        break;
    case su_TYPE_U64:
        result.u64 = *(const su_U64*)value;
        break;

    case su_TYPE_S8:
        result.s8 = *(const su_S8*)value;
        break;
    case su_TYPE_S16:
        result.s16 = *(const su_S16*)value;
        break;
    case su_TYPE_S32:
        result.s32 = *(const su_S32*)value;
        break;
    case su_TYPE_S64:
        result.s64 = *(const su_S64*)value;
        break;

    case su_TYPE_BOOL:
        result.boolean = *(const su_Bool*)value;
        break;

    case su_TYPE_UV:
        result.uv = *(const su_Uv*)value;
        break;
    case su_TYPE_VEC2:
        result.vec2 = *(const su_Vec2*)value;
        break;
    case su_TYPE_VEC3:
        result.vec3 = *(const su_Vec3*)value;
        break;
    case su_TYPE_VEC4:
        result.vec4 = *(const su_Vec4*)value;
        break;

    case su_TYPE_COLOR:
        result.color = *(const su_Color*)value;
        break;

    case su_TYPE_MAT2:
    case su_TYPE_MAT3:
        break;
    case su_TYPE_MAT4:
        result.mat4 = *(const su_Mat4*)value;
        break;

    case su_TYPE_MAT2X3:
    case su_TYPE_MAT2X4:
    case su_TYPE_MAT3X2:
    case su_TYPE_MAT3X4:
    case su_TYPE_MAT4X2:
    case su_TYPE_MAT4X3:
        break;

    default:
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                        "Invalid type %d for uniform", type);
        break;
    }

    return result;
}
