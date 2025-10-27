#include "saci-backend/renderer/sb-renderer.h"
#include "./sb-renderer-common.h"

#include "saci-utils/su-log.h"
#include "saci-utils/memory/su-memory.h"

#include "saci-utils/su-types-common.h"

#include <stdio.h>

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

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr);

SA_INTERNAL void sb__renderer_init_bound_extra(struct sb_InstanceBoundExtra* bound_extra, const struct su_RendererConfig cfg, su_MemPool* mem);

/* --- PUBLIC FUNCS --- */

void sb_renderer_instanced_new(struct sb_Renderer* self, su_MemPool* mem, struct su_RendererConfig* cfg, union sb_GFXInfo* info)
{
    su_LOG_ASSERT_M(self && mem && cfg && info, su_LOG_CONTEXT_RENDERER_INSTANCE, "Empty or null parameters for new isntance renderer");

    struct sb_InstanceRenderer* rendr = malloc(sizeof(struct sb_InstanceRenderer));
    self->rendr.instance_renderer = rendr;
    self->interface = &sc_INSTANCE_RENDERER_DEFAULT_INTERFACE;

    sb__renderer_init_instance_batch(rendr);
    sb_renderer_cfg_copy_and_cleanup(&rendr->cfg, cfg, mem);
    sb_gfx_create(&rendr->gfx, rendr->cfg);
    sb_renderer_init_bound(&rendr->bound, rendr->cfg, mem);
    sb__renderer_init_bound_extra(&rendr->bound_extra, rendr->cfg, mem);
}

void sb_renderer_push_instance_data(struct sb_Renderer* rendr, const struct sb_GFXInstanceData* instance_data)
{
    // TODO change to if (condition) error clauses
    su_LOG_ASSERT_M(rendr->type == sb_RENDERER_INSTANCE, su_LOG_CONTEXT_RENDERER_INSTANCE, "Trying to set instance transforms in non instance renderer");
    su_LOG_ASSERT_M(instance_data, su_LOG_CONTEXT_RENDERER_INSTANCE, "instances with different sizes");
    su_LOG_ASSERT_M(instance_data->instance_data_structure && instance_data->data_size, su_LOG_CONTEXT_RENDERER_INSTANCE, "Seting instances with different sizes");

    su_LOG_INFOF_M(
        su_LOG_TYPE_USER,
        su_LOG_CONTEXT_RENDERER_INSTANCE,
        "Binding instance data at %u location",
        instance_data->location);

    if (!su_darray_clear(rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array))
    {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_RENDERER_INSTANCE,
            "Could not clear instance array");
    }
    if (!su_darray_push(rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array, instance_data))
    {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_RENDERER_INSTANCE,
            "Could not push to instance array");
    }
}

SA_API void sb_renderer_set_instance_data_array(struct sb_Renderer* rendr, const su_DArray* instance_data_array)
{
    // TODO change to if error
    su_LOG_ASSERT_M(rendr->type == sb_RENDERER_INSTANCE, su_LOG_CONTEXT_RENDERER_INSTANCE, "Trying to set instance transforms in non instance renderer");
    su_LOG_ASSERT_M(instance_data_array, su_LOG_CONTEXT_RENDERER_INSTANCE, "Seting instances with different sizes");
    su_LOG_ASSERT_M(su_darray_get_elem_size(instance_data_array) == sizeof(struct sb_GFXInstanceData), su_LOG_CONTEXT_RENDERER_INSTANCE, "Pushing instances without the struct sb_GFXInstanceData size");

    su_LOG_INFOF_M(
        su_LOG_TYPE_USER,
        su_LOG_CONTEXT_RENDERER_INSTANCE,
        "Binding %lu instances",
        su_darray_length(instance_data_array));
    if (!su_darray_clear(rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array))
    {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER_INSTANCE, "Could not clear instance data array");
    }
    if (!su_darray_append(rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array, instance_data_array))
    {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_RENDERER_INSTANCE,
            "Could not push to instance array");
    }
}

/* --- LOCAL FUNCS --- */

SA_INTERNAL void sb__renderer_instance_begin(const struct sb_Renderer* self)
{
    if (!su_darray_clear(self->rendr.instance_renderer->bound.index_array))
    {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                       "Could not clear bound index array");
    }
    if (!su_darray_clear(self->rendr.instance_renderer->bound.uniform_data_array))
    {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER_INSTANCE, "Could not clear bound uniform data array");
    }
    self->rendr.instance_renderer->bound.texture = sb_TEXTURE_INVALID;

    self->rendr.instance_renderer->batch_info.in_use = 0;
    for (su_U8 i = 0; i < self->rendr.instance_renderer->cfg.batch.capacity; ++i)
    {
        struct sb_GFXDrawData* batch = &(self->rendr.instance_renderer->batch_array[i]);
        for (int j = 0; j < SACI_MAX_TEXTURES; ++j)
        {
            batch->texture_array[j].gl_texture.texture = sb_TEXTURE_INVALID;
            batch->texture_array_loc[j] = 0;
        }
        if (!su_darray_clear(batch->vertex_array))
        {
            su_LOG_ERRORF_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                su_LOG_CONTEXT_RENDERER_INSTANCE,
                "Could not clear %dith batch's vertex array", i);
        }
        if (!su_darray_clear(batch->index_array))
        {
            su_LOG_ERRORF_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                su_LOG_CONTEXT_RENDERER_INSTANCE,
                "Could not clear %dith batch's index array", i);
        }
        if (!su_darray_clear(batch->instance_data_array))
        {
            su_LOG_ERRORF_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                su_LOG_CONTEXT_RENDERER_INSTANCE,
                "Could not clear %dith batch's instance buffer array", i);
        }
        if (!su_darray_clear(batch->uniform_data_array))
        {
            su_LOG_ERRORF_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                su_LOG_CONTEXT_RENDERER_INSTANCE,
                "Could not clear %dith batch's uniform array array", i);
        }
    }
}

SA_INTERNAL void sb__renderer_instance_bind_texture(struct sb_Renderer* self, su_TextureId tex_id)
{
    self->rendr.instance_renderer->bound.texture = tex_id;
}

SA_INTERNAL su_S32 sb__renderer_instance_get_uniform_id(struct sb_Renderer* self,
                                                        const char* const uniform_name)
{
    return sb_gfx_get_uniform_loc_cstr(
        &self->rendr.instance_renderer->gfx,
        uniform_name);
}

SA_INTERNAL void sb__renderer_instance_set_uniform(struct sb_Renderer* self,
                                                   const su_S32 uniform_id,
                                                   const void* const value,
                                                   const su_DataType type)
{
    // TODO check if is needed
    su_DArray** uniform_data_array = &self->rendr.instance_renderer->bound.uniform_data_array;

    if (uniform_id < 0 || uniform_id >= su_TYPE_MAX ||
        uniform_id == su_TYPE_BUFFERID || uniform_id == su_TYPE_SHADERID ||
        uniform_id == su_TYPE_TEXTUREID)
    {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                       "Trying to bind uniform with invalid ID");
        return;
    }
    if (su_CAST_M(int)(type) == 0 || su_CAST_M(int)(type) > 26)
    {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_RENDERER,
                       "Trying to bind uniform with invalid type");
        return;
    }
    struct sb_GFXUniformData new_uniform = {0};
    new_uniform.type = type;
    new_uniform.value = sb_renderer_uniform_value_from_type(type, value);
    new_uniform.location = uniform_id;

    for (su_U64 i = 0; i < su_darray_length(*uniform_data_array); ++i)
    {
        struct sb_GFXUniformData uniform_data = *(struct sb_GFXUniformData*)su_darray_get(*uniform_data_array, i);
        if (uniform_data.location == uniform_id)
        {
            su_darray_set(*uniform_data_array, i, &new_uniform, sizeof(new_uniform));
            return;
        }
    }
    if (!su_darray_push(*uniform_data_array, &new_uniform))
    {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_RENDERER,
                        "Could not set uniform in renderer");
    }
}

SA_INTERNAL void sb__renderer_instance_bind_index_buffer(struct sb_Renderer* self, const su_DArray* new_indices)
{
    if (!new_indices)
    {
        su_LOG_WARN_M(su_LOG_TYPE_USER, su_LOG_WARN_SEVERITY_MEDIUM,
                      su_LOG_CONTEXT_RENDERER,
                      "Indices are NULL and cannot be bound");
        return;
    }
    su_LOG_INFOF_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_RENDERER_INSTANCE,
                   "Bound %lu indices", su_darray_length(new_indices));

    su_darray_clear(self->rendr.instance_renderer->bound.index_array);
    su_darray_append(self->rendr.instance_renderer->bound.index_array, new_indices);
}

SA_INTERNAL void sb__renderer_instance_push_mesh(struct sb_Renderer* self,
                                                 const su_DArray* pos_array,
                                                 const su_DArray* uv_array,
                                                 const su_DArray* color_array)
{
    struct sb_InstanceRenderer* rendr = self->rendr.instance_renderer;
    // s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);
    // TODO validate instance_transform

    su_LOG_INFOF_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_RENDERER_INSTANCE,
                   "Attempting to push %lu vertices to instance batch",
                   su_darray_length(pos_array));
    su_LOG_INFOF_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_RENDERER_INSTANCE,
                   "Attempting to push %lu indices to instance batch",
                   su_darray_length(rendr->bound.index_array));
    struct sb_GFXDrawData* batch = &(rendr->batch_array)[rendr->batch_info.in_use];
    su_Color default_color = {0, 0, 0, 0};
    su_Uv default_uv = {0, 0};
    for (su_U64 i = 0; i < su_darray_length(pos_array); ++i)
    {
        su_Vec3 pos = *(su_Vec3*)su_darray_get(pos_array, i);
        su_Color color = default_color;
        su_Uv uv = default_uv;

        if (uv_array)
        {
            uv = *(su_Uv*)su_darray_get(uv_array, i);
        }
        if (color_array)
        {
            color = *(su_Color*)su_darray_get(color_array, i);
        }
        struct sb_Vertex vertex = (struct sb_Vertex){pos, color, uv};
        if (!su_darray_push(batch->vertex_array, &vertex))
        {
            su_LOG_ERROR_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                su_LOG_CONTEXT_RENDERER,
                "Could not push vertex to instance batch");
        }
    }
    if (!su_darray_append(batch->index_array, rendr->bound.index_array))
    {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_RENDERER, "Could not push index to batch");
    }
    if (!su_darray_append(batch->uniform_data_array, rendr->bound.uniform_data_array))
    {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_RENDERER, "Could not push uniform to batch");
    }
    if (!su_darray_append(batch->instance_data_array, rendr->bound_extra.bound_instance_data_array))
    {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_RENDERER, "Could not push instances to batch");
    }
    rendr->batch_info.in_use++;
}

SA_INTERNAL void sb__renderer_draw_instance_batch(const struct sb_Renderer* self)
{
    const struct sb_InstanceRenderer* rendr = self->rendr.instance_renderer;
    su_LOG_INFOF_M(
        su_LOG_TYPE_USER,
        su_LOG_CONTEXT_RENDERER_INSTANCE,
        "Flushing %d instance batches",
        rendr->batch_info.in_use);

    for (su_U8 i = 0; i < rendr->batch_info.in_use; ++i)
    {
        struct sb_GFXDrawData* batch = &(rendr->batch_array)[i];
        sb_gfx_draw(&self->rendr.instance_renderer->gfx, batch);
    }
}

#if 0
SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr) {
    struct su_RendererConfig cfg = rendr->cfg;

    su_U64 index_size = cfg.index_data.element_size_internal * cfg.batch.index_cfg.capacity + su_SIZE_OF_DARRAY + 1024;
    su_U64 vertex_size = cfg.vertex_data.element_size_internal * cfg.batch.vertex_cfg.capacity + su_SIZE_OF_DARRAY + 1024;
    su_U64 instance_size = cfg.batch.instance_cfg.capacity * sizeof(struct sb_GFXInstanceData) + su_SIZE_OF_DARRAY + 1024;
    su_U64 uniform_size = su_darray_length(cfg.uniform_array) * sizeof(struct sb_GFXUniformData) + su_SIZE_OF_DARRAY + 1024;

    rendr->batch_ptr_array = calloc(cfg.batch.capacity, sizeof(struct sb_GFXDrawData*));

    for (su_U8 i = 0; i < cfg.batch.capacity; ++i) {
        void *drawdata_mem = NULL,
             *index_mem = NULL,
             *vertex_mem = NULL,
             *instance_mem = NULL,
             *uniform_mem = NULL;
        su_mem_alloc(su_MEM_CONTEXT_RENDERER, 1,
                     sizeof(struct sb_GFXDrawData) /* , &drawdata_mem */);
        su_mem_alloc(su_MEM_CONTEXT_RENDERER, 1,
                     index_size /* , &index_mem */);
        su_mem_alloc(su_MEM_CONTEXT_RENDERER, 1,
                     vertex_size /* , &vertex_mem */);
        su_mem_alloc(su_MEM_CONTEXT_RENDERER, 1,
                     instance_size /* , &instance_mem */);
        su_mem_alloc(su_MEM_CONTEXT_RENDERER, 1,
                     uniform_size /* , &uniform_mem */);
        if (!drawdata_mem || !index_mem || !vertex_mem || !instance_mem || !uniform_mem) {
            su_LOG_ERROR_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH,
                su_LOG_CONTEXT_RENDERER_INSTANCE, "Could not initialize memory for renderer");
        }

        struct sb_GFXDrawData* drawdata = su_CAST_M(struct sb_GFXDrawData*)(drawdata_mem);

        // drawdata->vertex_array = su_darray_create_ctx(
        //     vertex_mem,
        //     vertex_size,
        //     cfg.batch.vertex_cfg.capacity,
        //     cfg.vertex_data.element_size_internal,
        //     cfg.batch.vertex_cfg.fixed_size);
        drawdata->vertex_struct_size = sizeof(struct sb_Vertex);
        if (!drawdata->vertex_array) {
            su_LOG_ERROR_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH,
                su_LOG_CONTEXT_RENDERER_INSTANCE,
                "Could not create a batch's vertex array");
        }

        // drawdata->index_array = su_darray_create_ctx(
        //     index_mem,
        //     index_size,
        //     cfg.batch.index_cfg.capacity,
        //     sizeof(su_U32),
        //     cfg.batch.index_cfg.fixed_size);
        drawdata->index_struct_size = sizeof(su_U32);
        if (!drawdata->index_array) {
            su_LOG_ERROR_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH,
                su_LOG_CONTEXT_RENDERER_INSTANCE,
                "Could not create a batch's index array");
        }

        // TODO not well made, needs to contain the data for the arrays inside itself
        // drawdata->instance_buffer_array = su_darray_create_ctx(
        //     instance_mem,
        //     instance_size,
        //     cfg.batch.instance_cfg.capacity,
        //     sizeof(struct sb_GFXInstanceData),
        //     cfg.batch.instance_cfg.fixed_size);
        // if (!drawdata->instance_buffer_array) {
        //     su_LOG_ERROR_M(
        //         su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH,
        //         su_LOG_CONTEXT_RENDERER_INSTANCE,
        //         "Could not create a batch's instance array");
        // }

        // drawdata->uniform_data_array = su_darray_create_ctx(
        //     uniform_mem,
        //     uniform_size,
        //     su_darray_length(cfg.uniform_array),
        //     sizeof(struct sb_GFXUniformData),
        //     su_TRUE);
        if (!drawdata->uniform_data_array) {
            su_LOG_ERROR_M(su_LOG_TYPE_USER,
                           su_LOG_ERROR_SEVERITY_CRASH,
                           su_LOG_CONTEXT_RENDERER_INSTANCE,
                           "Could not create a batch's uniform array");
        }

        for (su_U32 t = 0; t < SACI_MAX_TEXTURES; ++t) {
            // TODO when changing renderer api
            drawdata->texture_array[t].gl_texture.texture = sb_TEXTURE_INVALID;
            drawdata->texture_array_loc[t] = 0;
        }

        rendr->batch_ptr_array[i] = drawdata;
    }
}
#endif

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr)
{
    struct su_RendererConfig cfg = rendr->cfg;

    const su_U64 index_size = cfg.index_data.element_size_internal * cfg.batch.index_cfg.capacity;
    const su_U64 vertex_size = cfg.vertex_data.element_size_internal * cfg.batch.vertex_cfg.capacity;
    const su_U64 instance_size = cfg.batch.instance_cfg.capacity * sizeof(struct sb_GFXInstanceData);
    const su_U64 uniform_size = cfg.uniform_array_length * sizeof(struct sb_GFXUniformData);
    const su_U64 batch_capacity = cfg.batch.capacity;

    // Memory just for the draw data structure
    struct su_MemChunk* drawdata_chunk = su_mem_alloc_chunk(su_MEM_CONTEXT_RENDERER, batch_capacity, sizeof(struct sb_GFXDrawData));
    rendr->batch_array = (struct sb_GFXDrawData*)su_mem_chunk_get_ptr(drawdata_chunk, 0);

    struct su_MemChunk* index_chunk = su_mem_alloc_chunk(su_MEM_CONTEXT_RENDERER, batch_capacity, index_size);
    struct su_MemChunk* vertex_chunk = su_mem_alloc_chunk(su_MEM_CONTEXT_RENDERER, batch_capacity, vertex_size);
    struct su_MemChunk* instance_chunk = su_mem_alloc_chunk(su_MEM_CONTEXT_RENDERER, batch_capacity, instance_size);
    struct su_MemChunk* uniform_chunk = su_mem_alloc_chunk(su_MEM_CONTEXT_RENDERER, batch_capacity, uniform_size);

    for (su_U8 i = 0; i < cfg.batch.capacity; ++i)
    {
        rendr->batch_array[i].index_array = su_mem_chunk_get_ptr(index_chunk, i);
        rendr->batch_array[i].vertex_array = su_mem_chunk_get_ptr(vertex_chunk, i);
        rendr->batch_array[i].instance_data_array = su_mem_chunk_get_ptr(instance_chunk, i);
        rendr->batch_array[i].uniform_data_array = su_mem_chunk_get_ptr(uniform_chunk, i);

        rendr->batch_array[i].index_struct_size = cfg.index_data.element_size_internal;
        rendr->batch_array[i].vertex_struct_size = cfg.vertex_data.element_size_internal;

        for (su_U8 j = 0; j < SACI_MAX_TEXTURES; ++j)
        {
            rendr->batch_array[i].texture_array[j].gl_texture.texture = 0;
            rendr->batch_array[i].texture_array[j].gl_texture.is_empty = su_TRUE;
        }
    }
}

SA_INTERNAL void sb__renderer_init_bound_extra(struct sb_InstanceBoundExtra* bound_extra, const struct su_RendererConfig cfg, su_MemPool* mem)
{
    bound_extra->bound_instance_data_array = su_darray_create(cfg.bound.instance_cfg.capacity, sizeof(struct sb_GFXInstanceData), su_TRUE);
}
