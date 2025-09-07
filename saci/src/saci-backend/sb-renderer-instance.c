#include "./sb-renderer-common.h"

#include "saci-utils/su-debug.h"

/* --- LOCAL --- */

SA_INTERNAL void sb__renderer_instance_begin(const struct sb_Renderer* self);

SA_INTERNAL void sb__renderer_instance_bind_texture(struct sb_Renderer* self, su_TextureId tex_id);

SA_INTERNAL su_S32 sb__renderer_instance_get_uniform_id(struct sb_Renderer* self,
                                                        const char* const uniform_name);
SA_INTERNAL void sb__renderer_instance_set_uniform(struct sb_Renderer* self,
                                                   const su_S32 uniform_id,
                                                   const void* const value,
                                                   const su_DataType type);

SA_INTERNAL void sb__renderer_instance_bind_index_buffer(struct sb_Renderer* self, const su_DArray* new_indices);

SA_INTERNAL void sb__renderer_instance_push_mesh(struct sb_Renderer* self,
                                                 const su_DArray* pos_array,
                                                 const su_DArray* uv_array,
                                                 const su_DArray* color_array);

SA_INTERNAL void sb__renderer_draw_instance_batch(const struct sb_Renderer* rendr);

SA_INTERNAL const struct sb_RendererInterface sc_INSTANCE_RENDERER_DEFAULT_INTERFACE =
    {
        .new = sb_renderer_instanced_new,
        .begin = sb__renderer_instance_begin,
        .bind_texture = sb__renderer_instance_bind_texture,
        .get_uniform_id = sb__renderer_instance_get_uniform_id,
        .set_uniform = sb__renderer_instance_set_uniform,
        .bind_index_buffer = sb__renderer_instance_bind_index_buffer,
        .push_mesh = sb__renderer_instance_push_mesh,
        .draw = sb__renderer_draw_instance_batch,
        .free = NULL,
        .free_opts = NULL,
};

/* --- Helper --- */

SA_INTERNAL void sb__init_instance_buffers(struct sb_RendererConfig* cfg_out);

SA_INTERNAL void sb__renderer_instance_fill_default(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info);

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr);

/* --- PUBLIC FUNCS --- */

void sb_renderer_instanced_new(sb_Renderer* self) {
    struct sb_InstanceRenderer* rendr = su_MALLOC_M(sizeof(struct sb_InstanceRenderer));
    self->rendr.instance_renderer = rendr;
    self->interface = &sc_INSTANCE_RENDERER_DEFAULT_INTERFACE;
    struct sb_RendererConfig cfg = sb_CFG_DEFAULT_INSTANCE;
    sb_cfg_manager_get_renderer(su_string_create("instance", su_REALLOCATION_KIND_FIXED_SIZE), &cfg);
    sb_gfx_init_shader(&rendr->gfx, cfg);
    sb__renderer_instance_fill_default(&cfg, &rendr->gfx);
    sb_gfx_create(&rendr->gfx, cfg);
    sb__renderer_init_instance_batch(rendr);
    sb_renderer_init_bound(&rendr->bound, &rendr->bound_info, cfg);
}

void sb_renderer_set_instance_data(struct sb_Renderer* rendr, su_DArray* transform_array, su_DArray* color_array) {
    su_LOG_ASSERT_MESSAGE_M(rendr->type == sb_RENDERER_INSTANCE, "Trying to set instance transforms in non instance renderer");
    su_LOG_ASSERT_MESSAGE_M(su_darray_length(transform_array) == su_darray_length(color_array), "Seting instances with different sizes");

    su_LOG_DEBUGF_PRINT_M(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Binding %lu transforms and colors",
        su_darray_length(transform_array));
    su_DArray* data_array = su_darray_create(su_darray_length(transform_array), sizeof(struct sb_InstanceData), su_TRUE);
    for (su_U64 i = 0; i < su_darray_length(transform_array); ++i) {
        struct sb_InstanceData data = {0};
        su_darray_get(transform_array, i, &data.model);
        su_darray_get(color_array, i, &data.color);
        su_darray_push(data_array, &data);
    }

    su_darray_clear(rendr->rendr.instance_renderer->bound_extra.bound_instance_array);
    su_darray_append(rendr->rendr.instance_renderer->bound_extra.bound_instance_array, data_array);
}

/* --- LOCAL FUNCS --- */

SA_INTERNAL void sb__renderer_instance_begin(const struct sb_Renderer* self) {
    su_darray_clear(self->rendr.instance_renderer->bound.index_array);
    su_darray_clear(self->rendr.instance_renderer->bound.uniform_data_array);
    self->rendr.instance_renderer->bound.texture = sb_TEXTURE_INVALID;

    self->rendr.instance_renderer->batch_info.in_use = 0;
    for (su_U8 i = 0; i < self->rendr.instance_renderer->batch_info.batch_capacity; ++i) {
        struct sb_GFXDrawData* batch = self->rendr.instance_renderer->batch_ptr_array[i];
        for (int j = 0; j < SACI_MAX_TEXTURES; ++j) {
            batch->texture_array[j].gl_texture.texture = sb_TEXTURE_INVALID;
            batch->texture_array_loc[j] = 0;
        }
        su_darray_clear(batch->vertex_array);
        su_darray_clear(batch->index_array);
        su_darray_clear(batch->instance_array_array);
        su_darray_clear(batch->instance_location_array);
        su_darray_clear(batch->uniform_array_array);
        su_darray_clear(batch->uniform_location_array);
    }
}

SA_INTERNAL void sb__renderer_instance_bind_texture(struct sb_Renderer* self, su_TextureId tex_id) {
    self->rendr.instance_renderer->bound.texture = tex_id;
}

SA_INTERNAL su_S32 sb__renderer_instance_get_uniform_id(struct sb_Renderer* self,
                                                        const char* const uniform_name) {
    return sb_gfx_get_uniform_loc_cstr(
        &self->rendr.instance_renderer->gfx,
        uniform_name);
}

SA_INTERNAL void sb__renderer_instance_set_uniform(struct sb_Renderer* self,
                                                   const su_S32 uniform_id,
                                                   const void* const value,
                                                   const su_DataType type) {
    // TODO check if is needed
    su_DArray** uniform_data_array = &self->rendr.instance_renderer->bound.uniform_data_array;

    if (uniform_id < 0 || uniform_id >= su_TYPE_MAX ||
        uniform_id == su_TYPE_BUFFERID || uniform_id == su_TYPE_SHADERID ||
        uniform_id == su_TYPE_TEXTUREID) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid ID");
        return;
    }
    if (su_SCAST_TO_M(int)(type) == 0 || su_SCAST_TO_M(int)(type) > 26) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid type");
        return;
    }
    struct sb_GFXUniformData new_uniform = {0};
    new_uniform.type = type;
    new_uniform.value = sb_renderer_uniform_value_from_type(type, value);
    new_uniform.location = uniform_id;

    struct sb_GFXUniformData* uniform_data = su_MALLOC_M(sizeof(struct sb_GFXUniformData));
    for (su_U64 i = 0; i < su_darray_length(*uniform_data_array); ++i) {
        su_darray_get(*uniform_data_array, i, uniform_data);
        if (uniform_data->location == uniform_id) {
            su_FREE_M(uniform_data);
            su_darray_set(*uniform_data_array, i, &new_uniform);
            return;
        }
    }
    if (!su_darray_push(*uniform_data_array, &new_uniform)) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                              "Could not set uniform in renderer");
    }
    su_FREE_M(uniform_data);
}

SA_INTERNAL void sb__renderer_instance_bind_index_buffer(struct sb_Renderer* self, const su_DArray* new_indices) {
    if (!new_indices) {
        su_LOG_WARN_PRINT_M(su_LOG_SEVERITY_MEDIUM,
                            su_LOG_CONTEXT_RENDERER,
                            "Indices are NULL and cannot be bound");
        return;
    }
    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, su_LOG_CONTEXT_RENDERER,
                          "Bound %lu indices", su_darray_length(new_indices));

    su_darray_clear(self->rendr.instance_renderer->bound.index_array);
    su_darray_append(self->rendr.instance_renderer->bound.index_array, new_indices);
}

SA_INTERNAL void sb__renderer_instance_push_mesh(struct sb_Renderer* self,
                                                 const su_DArray* pos_array,
                                                 const su_DArray* uv_array,
                                                 const su_DArray* color_array) {
    struct sb_InstanceRenderer* rendr = self->rendr.instance_renderer;
    // s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);
    // TODO validate instance_transform

    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu vertices to instance batch",
                          su_darray_length(pos_array));
    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          su_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu indices to instance batch",
                          su_darray_length(rendr->common.bound.index_array));
    struct sb_GFXDrawData* batch = rendr->batch_ptr_array[rendr->batch_info.in_use];
    su_Color default_color = {0, 0, 0, 0};
    su_Uv default_uv = {0, 0};
    for (su_U64 i = 0; i < su_darray_length(pos_array); ++i) {
        su_Vec3 pos;
        su_Color color = default_color;
        su_Uv uv = default_uv;
        su_darray_get(pos_array, i, &pos);
        if (uv_array) {
            su_darray_get(uv_array, i, &uv);
        }
        if (color_array) {
            su_darray_get(color_array, i, &color);
        }
        struct sb_Vertex vertex = (struct sb_Vertex){pos, color, uv};
        if (!su_darray_push(batch->vertex_array, &vertex)) {
            su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER, "Could not push vertex to instance batch");
        }
    }
    su_darray_append(batch->index_array, rendr->bound.index_array);
    su_darray_append(batch->uniform_array_array, rendr->bound.uniform_data_array);
    su_darray_append(batch->instance_array_array, rendr->bound_extra.bound_instance_array);
    rendr->batch_info.in_use++;
}

SA_INTERNAL void sb__renderer_draw_instance_batch(const struct sb_Renderer* self) {
    const struct sb_InstanceRenderer* rendr = self->rendr.instance_renderer;
    su_LOG_DEBUGF_PRINT_M(
        su_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        su_LOG_CONTEXT_RENDERER,
        "Flushing %d instance batches",
        rendr->batch_info.in_use);

    static struct {
        su_TextureId texture;
    } previous_instance_batch = {0};

    for (su_U8 i = 0; i < rendr->batch_info.in_use; ++i) {
        struct sb_GFXDrawData* batch = rendr->batch_ptr_array[i];
        sb_gfx_draw(&self->rendr.instance_renderer->gfx, batch);
    }
}

SA_INTERNAL void sb__init_instance_buffers(struct sb_RendererConfig* cfg_out) {
    if (!cfg_out->batch.instance_cfg.buffer_array) {
        cfg_out->batch.instance_cfg.buffer_array =
            su_darray_create(1, sizeof(struct sb_RendererCfgInstanceBuffer), su_TRUE);

        struct sb_RendererCfgInstanceBufferLayout model_mat = {
            .name = su_string_create("i_model_matrix", su_REALLOCATION_KIND_FIXED_SIZE),
            .type = su_TYPE_MAT4,
            .offset = 0,
            .location = 3,
        };
        struct sb_RendererCfgInstanceBufferLayout color = {
            .name = su_string_create("i_color", su_REALLOCATION_KIND_FIXED_SIZE),
            .type = su_TYPE_COLOR,
            .offset = 64,
            .location = 7,
        };

        struct sb_RendererCfgInstanceBuffer buffer = {
            .name = su_string_create("default string buffer", su_REALLOCATION_KIND_FIXED_SIZE),
            .layout_array = su_darray_create(2, sizeof(struct sb_RendererCfgInstanceBufferLayout), su_TRUE),
        };

        su_darray_push(buffer.layout_array, &model_mat);
        su_darray_push(buffer.layout_array, &color);
        su_darray_push(cfg_out->batch.instance_cfg.buffer_array, &buffer);
    }
}

SA_INTERNAL void sb__renderer_instance_fill_default(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info) {
    sb_init_uniforms(cfg_out, gfx_info);
    sb_init_samplers(cfg_out, gfx_info);
    sb_init_vertex_layout(cfg_out);
    sb_init_shaders(cfg_out);
    sb__init_instance_buffers(cfg_out);
}

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr) {
    struct sb_InstanceBatchInfo instance_info = rendr->batch_info;
    struct sb_RendererBoundInfo bound_info = rendr->bound_info;

    su_U64 index_size = instance_info.index_array_capacity * sizeof(su_U32);
    su_U64 vertex_size = instance_info.vertex_array_capacity * sizeof(struct sb_Vertex);
    su_U64 instance_size = instance_info.transform_array_capacity * sizeof(struct sb_InstanceData);
    su_U64 uniform_size = bound_info.uniform_array_capacity * sizeof(struct sb_GFXUniformData);

    su_U64 batch_arena_element_size =
        index_size + vertex_size + instance_size + uniform_size +
        sizeof(struct sb_GFXDrawData);

    rendr->batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->batch_ptr_array = su_CALLOC_M(instance_info.batch_capacity, sizeof(struct sb_GFXDrawData*));

    ArenaInit(
        &rendr->batch_arena,
        batch_arena_element_size * instance_info.batch_capacity + 1024);

    for (su_U8 i = 0; i < instance_info.batch_capacity; ++i) {
        void* drawdata_mem = ArenaPush(&rendr->batch_arena, sizeof(struct sb_GFXDrawData));
        void* index_mem = ArenaPush(&rendr->batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->batch_arena, vertex_size);
        void* instance_mem = ArenaPush(&rendr->batch_arena, instance_size);
        void* uniform_mem = ArenaPush(&rendr->batch_arena, uniform_size);

        struct sb_GFXDrawData* drawdata = su_SCAST_TO_M(struct sb_GFXDrawData*)(drawdata_mem);

        drawdata->vertex_array = su_darray_create_ctx(
            vertex_mem,
            vertex_size + su_SIZE_OF_DARRAY,
            instance_info.vertex_array_capacity,
            sizeof(struct sb_Vertex),
            su_TRUE);
        drawdata->vertex_struct_size = sizeof(struct sb_Vertex);

        drawdata->index_array = su_darray_create_ctx(
            index_mem,
            index_size + su_SIZE_OF_DARRAY,
            instance_info.index_array_capacity,
            sizeof(su_U32),
            su_TRUE);
        drawdata->index_struct_size = sizeof(su_U32);

        drawdata->instance_array_array = su_darray_create_ctx(
            instance_mem,
            instance_size + su_SIZE_OF_DARRAY,
            instance_info.transform_array_capacity,
            sizeof(struct sb_InstanceData),
            su_TRUE);
        drawdata->instance_location_array = NULL; // optional
        drawdata->instance_struct_size = sizeof(struct sb_InstanceData);

        drawdata->uniform_array_array = su_darray_create_ctx(
            uniform_mem,
            uniform_size + su_SIZE_OF_DARRAY,
            bound_info.uniform_array_capacity,
            sizeof(struct sb_GFXUniformData),
            su_TRUE);
        drawdata->uniform_location_array = NULL;

        for (su_U32 t = 0; t < SACI_MAX_TEXTURES; ++t) {
            // TODO when changing renderer api
            drawdata->texture_array[t].gl_texture.texture = sb_TEXTURE_INVALID;
            drawdata->texture_array_loc[t] = 0;
        }

        rendr->batch_ptr_array[i] = drawdata;
    }
}
