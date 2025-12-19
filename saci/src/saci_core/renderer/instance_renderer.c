#include "saci_core/renderer/renderer.h"
#include "saci_core/renderer/internal/renderer.h"

#include "saci_util/internal/log.h"
#include "saci_util/log.h"
#include "saci_util/math.h"
#include "saci_util/memory.h"
#include "saci_util/darray.h"
#include "saci_util/types.h"

#include "saci_platform/gfx/internal/gfx.h"

#include "saci_platform/dependencies/dependency.h"
#include <stdio.h>
#include <stdlib.h>

/* --- LOCAL --- */

SACI_INTERNAL void csaci__renderer_instance_begin(const struct CSaciRenderer* self);

SACI_INTERNAL void csaci__renderer_instance_bind_texture(struct CSaciRenderer* self, union PSaciTexture tex_id);

SACI_INTERNAL SaciS32 csaci__renderer_instance_get_uniform_id(struct CSaciRenderer* self,
                                                              const char* const uniform_name);
SACI_INTERNAL void csaci__renderer_instance_set_uniform(struct CSaciRenderer* self,
                                                        const SaciS32 uniform_id,
                                                        const void* const value,
                                                        const SaciDataType type);

SACI_INTERNAL void csaci__renderer_instance_bind_index_buffer(struct CSaciRenderer* rendr,
                                                              const CSaciIndex* data, const SaciU64 count);

SACI_INTERNAL void csaci__renderer_instance_push_mesh(struct CSaciRenderer* rendr,
                                                      const CSaciVertex* data, const SaciU64 count);

SACI_INTERNAL void csaci__renderer_draw_instance_batch(const struct CSaciRenderer* rendr);

SACI_INTERNAL const struct CSaciRendererInterface CSACI_G_INSTANCE_RENDERER_DEFAULT_INTERFACE =
    {
        .new = csaci_renderer_instanced_new,
        .begin = csaci__renderer_instance_begin,
        .bind_texture = csaci__renderer_instance_bind_texture,
        .get_uniform_id = csaci__renderer_instance_get_uniform_id,
        .set_uniform = csaci__renderer_instance_set_uniform,
        .bind_index_buffer = csaci__renderer_instance_bind_index_buffer,
        .push_mesh = csaci__renderer_instance_push_mesh,
        .draw = csaci__renderer_draw_instance_batch,
        .free = NULL,
        .free_opts = NULL,
};

/* --- Helper --- */

SACI_INTERNAL void csaci__renderer_init_instance_batch(struct CSaciInstanceRenderer* rendr);

SACI_INTERNAL void csaci__renderer_init_bound_extra(struct CSaciInstanceBoundExtra* bound_extra, const struct PSaciRendererConfig cfg, SaciMemPool* mem);

/* --- PUBLIC FUNCS --- */

void csaci_renderer_instanced_new(struct CSaciRenderer* self, SaciMemPool* mem, struct PSaciRendererConfig* cfg, union PSaciGFXInfo* info)
{
    SACI_LOG_ASSERT_M(self && mem && cfg && info, SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Empty or null parameters for new isntance renderer");

    struct CSaciInstanceRenderer* rendr = saci_mem_pool_alloc(mem, sizeof(struct CSaciInstanceRenderer));
    self->rendr.instance_renderer = rendr;
    self->rendr.instance_renderer->gfx = *info;
    self->interface = &CSACI_G_INSTANCE_RENDERER_DEFAULT_INTERFACE;

    psaci_cfg_cleanup_renderer_cfg(&rendr->cfg);
    csaci__renderer_init_instance_batch(rendr);
    psaci_gfx_create(&rendr->gfx, rendr->cfg);
    csaci_renderer_init_bound(&rendr->bound, rendr->cfg, mem);
    csaci__renderer_init_bound_extra(&rendr->bound_extra, rendr->cfg, mem);
}

// TODO
void csaci_renderer_push_instance_data(struct CSaciRenderer* rendr, const void* data, SaciU32 loc, SaciU32 data_size)
{
    // TODO change to if (condition) error clauses
    SACI_LOG_ASSERT_M(rendr->type == CSACI_RENDERER_INSTANCE, SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Trying to set instance transforms in non instance renderer");
    SACI_LOG_ASSERT_M(data, SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Data is NULL");
    SACI_LOG_ASSERT_M(loc, SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Location is 0");
    SACI_LOG_ASSERT_M(data_size, SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Instance size is 0");

    SACI_LOG_INFOF_M(
        SACI_LOG_TYPE_USER,
        SACI_LOG_CONTEXT_RENDERER_INSTANCE,
        "Binding instance data at %u location",
        loc);

    SaciU64 count = rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array_count++;
    if (count >= rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array_amount) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_HIGH,
            SACI_LOG_CONTEXT_RENDERER_INSTANCE,
            "Instance array overflow, flush or increase bound instance array size");
        return;
    }
    rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array[count].location = loc;
    rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array[count].data_size = data_size;

    // void* instance_data_src = rendr->rendr.instance_renderer->bound_extra.bound_instance_data_array[count].instance_data_structure;
}

// TODO
void csaci_renderer_set_instance_data(struct CSaciRenderer* rendr, SaciU32* location_array, SaciU32 location_array_count, const void* data_array, SaciU64 data_size, SaciU32 data_count)
{
    SACI_LOG_ASSERT_M(rendr->type == CSACI_RENDERER_INSTANCE, SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                      "Trying to set instance transforms in non instance renderer");
    SACI_LOG_ASSERT_M(location_array_count == data_count, SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                      "Seting instances with different amount of locations and data blocks");
    SACI_LOG_ASSERT_M(location_array && data_array,
                      SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                      "Location or data is NULL");

    SACI_LOG_INFOF_M(
        SACI_LOG_TYPE_USER,
        SACI_LOG_CONTEXT_RENDERER_INSTANCE,
        "Binding " SACI_FMTU32 " instances",
        data_count);
}

/* --- LOCAL FUNCS --- */

SACI_INTERNAL void csaci__renderer_instance_begin(const struct CSaciRenderer* self)
{
    self->rendr.instance_renderer->bound.index_count = 0;
    self->rendr.instance_renderer->bound.uniform_count = 0;
#ifdef __EMSCRIPTEN__
    self->rendr.instance_renderer->bound.texture.gl.texture = 0;
    self->rendr.instance_renderer->bound.texture.gl.is_empty = SACI_TRUE;
#else
    switch (psaci_dependencies_get_render_api()) {
    case PSACI_RENDERER_API_OPENGL4:
        self->rendr.instance_renderer->bound.texture.gl.texture = 0;
        self->rendr.instance_renderer->bound.texture.gl.is_empty = SACI_TRUE;
    case PSACI_RENDERER_API_OPENGLES3:
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
#endif

    self->rendr.instance_renderer->batch_info.in_use = 0;
    for (SaciU8 i = 0; i < self->rendr.instance_renderer->cfg.batch.capacity; ++i) {
        struct PSaciGFXDrawData* batch = &(self->rendr.instance_renderer->batch_array[i]);
        for (int j = 0; j < SACI_MAX_TEXTURES; ++j) {
            switch (psaci_dependencies_get_render_api()) {
            case PSACI_RENDERER_API_OPENGL4:
                batch->texture_array[j].gl.texture = 0;
                batch->texture_array[j].gl.is_empty = SACI_TRUE;
            case PSACI_RENDERER_API_OPENGLES3:
                break;
            case PSACI_RENDERER_API_VULKAN:
                break;
            }
            batch->texture_array_loc[j] = 0;
        }
        batch->vertex_data.count = 0;
        batch->index_data.count = 0;
        batch->instance_data_array_count = 0;
        batch->uniform_data_array_count = 0;
    }
}

SACI_INTERNAL void csaci__renderer_instance_bind_texture(struct CSaciRenderer* self, union PSaciTexture tex_id)
{
    self->rendr.instance_renderer->bound.texture = tex_id;
}

SACI_INTERNAL SaciS32 csaci__renderer_instance_get_uniform_id(struct CSaciRenderer* self,
                                                              const char* const uniform_name)
{
    return psaci_gfx_get_uniform_loc(
        &(self->rendr.instance_renderer->gfx),
        uniform_name);
}

SACI_INTERNAL void csaci__renderer_instance_set_uniform(struct CSaciRenderer* self,
                                                        const SaciS32 uniform_id,
                                                        const void* const value,
                                                        const SaciDataType type)
{
    struct PSaciGFXUniformData* uniform_array = self->rendr.instance_renderer->bound.uniform_data_array;
    SaciU64 uniform_array_count = self->rendr.instance_renderer->bound.uniform_count;

    if (uniform_id < 0 ||
        uniform_id == SACI_TYPE_BUFFERID || uniform_id == SACI_TYPE_SHADERID ||
        uniform_id == SACI_TYPE_TEXTUREID) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                         "Trying to bind uniform with invalid ID");
        return;
    }
    if (SACI_CAST_M(int)(type) == 0 || SACI_CAST_M(int)(type) > SACI_TYPE_MAX) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                         "Trying to bind uniform with invalid type");
        return;
    }
    struct PSaciGFXUniformData new_uniform = {0};
    new_uniform.type = type;
    new_uniform.value = csaci_renderer_uniform_value_from_type(type, value);
    new_uniform.location = uniform_id;

    // Checks if uniform already exists in bound structure
    for (SaciU64 i = 0; i < uniform_array_count; ++i) {
        struct PSaciGFXUniformData uniform_data = uniform_array[i];
        if (uniform_data.location == uniform_id) {
            uniform_array[i] = new_uniform;
            return;
        }
    }
    if (uniform_array_count + 1 == self->rendr.instance_renderer->bound.uniform_amount) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                         "Overflowing bound uniform data");
        return;
    }
    // If not it continues after the loop
    uniform_array[++uniform_array_count] = new_uniform;
}

SACI_INTERNAL void csaci__renderer_instance_bind_index_buffer(
    struct CSaciRenderer* self, const CSaciIndex* data, const SaciU64 count)
{
    if (!data) {
        SACI_LOG_WARN_M(SACI_LOG_TYPE_USER, SACI_LOG_WARN_SEVERITY_MEDIUM,
                        SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                        "Indices are NULL and cannot be bound");
        return;
    }
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                     "Bound " SACI_FMTU64 " indices", saci_darray_length(data));

    if (count > self->rendr.instance_renderer->bound.index_amount) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Overflowing bound index buffer");
        return;
    }
    self->rendr.instance_renderer->bound.index_count = count;
    // TODO memcpy
}

SACI_INTERNAL void csaci__renderer_instance_push_mesh(struct CSaciRenderer* self,
                                                      const CSaciVertex* data, const SaciU64 count)
{
    struct CSaciInstanceRenderer* rendr = self->rendr.instance_renderer;
    // s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);
    // TODO validate instance_transform

    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                     "Attempting to push " SACI_FMTU64 " vertices to instance batch",
                     count);
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER_INSTANCE,
                     "Attempting to push " SACI_FMTU64 " indices to instance batch",
                     rendr->bound.index_amount);
    struct PSaciGFXDrawData* batch = &(rendr->batch_array)[rendr->batch_info.in_use];
    for (SaciU64 i = 0; i < count; ++i) {
    }
    // if (!saci_darray_append(batch->index_array, self->bound.index_array)) {
    //     SACI_LOG_ERROR_M(
    //         SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
    //         SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Could not push index to batch");
    // }
    // if (!saci_darray_append(batch->uniform_data_array, self->bound.uniform_data_array)) {
    //     SACI_LOG_ERROR_M(
    //         SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
    //         SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Could not push uniform to batch");
    // }
    // if (!saci_darray_append(batch->instance_data_array, self->bound_extra.bound_instance_data_array)) {
    //     SACI_LOG_ERROR_M(
    //         SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
    //         SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Could not push instances to batch");
    // }
    rendr->batch_info.in_use++;
}

SACI_INTERNAL void csaci__renderer_draw_instance_batch(const struct CSaciRenderer* self)
{
    const struct CSaciInstanceRenderer* rendr = self->rendr.instance_renderer;
    SACI_LOG_INFOF_M(
        SACI_LOG_TYPE_USER,
        SACI_LOG_CONTEXT_RENDERER_INSTANCE,
        "Flushing %d instance batches",
        rendr->batch_info.in_use);

    // for (SaciU64 i = 0; i < saci_darray_length(rendr->bound_extra.bound_instance_data_array); ++i) {
    //     const struct PSaciGFXInstanceData* data = saci_darray_get(rendr->bound_extra.bound_instance_data_array, i);
    //     if (data->data_size == sizeof(su_Color))
    //         continue;
    //     printf(su_MAT4_FMT, su_MAT4_FMT_ARGS(*(su_Mat4*)data->instance_data_structure));
    // }
    for (SaciU8 i = 0; i < rendr->batch_info.in_use; ++i) {
        struct PSaciGFXDrawData* batch = &(rendr->batch_array)[i];
        psaci_gfx_draw(&self->rendr.instance_renderer->gfx, batch);
    }
}

SACI_INTERNAL void csaci__renderer_init_instance_batch(struct CSaciInstanceRenderer* rendr)
{
    struct PSaciRendererConfig cfg = rendr->cfg;

    const SaciU64 index_size = cfg.index_data.element_size_internal * cfg.batch.index_cfg.capacity;
    const SaciU64 vertex_size = cfg.vertex_data.element_size_internal * cfg.batch.vertex_cfg.capacity;
    const SaciU64 instance_size = cfg.batch.instance_cfg.capacity * sizeof(struct PSaciGFXInstanceData);
    const SaciU64 uniform_size = cfg.uniform_array_length * sizeof(struct PSaciGFXUniformData);
    const SaciU64 batch_capacity = cfg.batch.capacity;

    // Memory just for the draw data structure
    struct SaciMemChunk* drawdata_chunk = saci_mem_alloc_chunk(SACI_MEM_CONTEXT_RENDERER, batch_capacity, sizeof(struct PSaciGFXDrawData));
    rendr->batch_array = (struct PSaciGFXDrawData*)saci_mem_chunk_get_ptr(drawdata_chunk, 0);

    struct SaciMemChunk* index_chunk = saci_mem_alloc_chunk(SACI_MEM_CONTEXT_RENDERER, batch_capacity, index_size);
    struct SaciMemChunk* vertex_chunk = saci_mem_alloc_chunk(SACI_MEM_CONTEXT_RENDERER, batch_capacity, vertex_size);
    struct SaciMemChunk* instance_chunk = saci_mem_alloc_chunk(SACI_MEM_CONTEXT_RENDERER, batch_capacity, instance_size);
    struct SaciMemChunk* uniform_chunk = saci_mem_alloc_chunk(SACI_MEM_CONTEXT_RENDERER, batch_capacity, uniform_size);

    for (SaciU8 i = 0; i < cfg.batch.capacity; ++i) {
        struct PSaciGFXDrawData* batch = &(rendr->batch_array[i]);
        batch->index_data.array =
            saci_darray_create_ctx_void(saci_mem_chunk_get_ptr(index_chunk, i),
                                        index_size,
                                        cfg.batch.index_cfg.capacity,
                                        cfg.index_data.element_size_internal);

        batch->vertex_data.array =
            saci_darray_create_ctx_void(saci_mem_chunk_get_ptr(vertex_chunk, i),
                                        vertex_size,
                                        cfg.batch.vertex_cfg.capacity,
                                        cfg.vertex_data.element_size_internal);

        batch->instance_data_array =
            saci_darray_create_ctx_void(saci_mem_chunk_get_ptr(instance_chunk, i),
                                        instance_size,
                                        cfg.batch.instance_cfg.capacity,
                                        sizeof(struct PSaciGFXInstanceData));

        batch->uniform_data_array =
            saci_darray_create_ctx_void(saci_mem_chunk_get_ptr(uniform_chunk, i),
                                        uniform_size,
                                        cfg.uniform_array_length,
                                        sizeof(struct PSaciGFXUniformData));

        SACI_LOG_ASSERT_M(batch->index_data.array && batch->vertex_data.array &&
                              batch->instance_data_array && batch->uniform_data_array,
                          SACI_LOG_CONTEXT_RENDERER_INSTANCE, "Could not create batch's arrays");

        batch->index_data.struct_size = cfg.index_data.element_size_internal;
        batch->vertex_data.struct_size = cfg.vertex_data.element_size_internal;

        for (SaciU8 j = 0; j < SACI_MAX_TEXTURES; ++j) {
            rendr->batch_array[i].texture_array[j].gl.texture = 0;
            rendr->batch_array[i].texture_array[j].gl.is_empty = SACI_TRUE;
        }
    }
}

SACI_INTERNAL void csaci__renderer_init_bound_extra(struct CSaciInstanceBoundExtra* bound_extra, const struct PSaciRendererConfig cfg, SaciMemPool* pool)
{
    SaciU64 size = cfg.bound.instance_cfg.capacity * sizeof(struct PSaciGFXInstanceData) + SACI_SIZE_OF_DARRAY;
    void* mem = saci_mem_pool_alloc(pool, size);
    bound_extra->bound_instance_data_array = saci_darray_create_ctx_void(mem, size, cfg.bound.instance_cfg.capacity, sizeof(struct PSaciGFXInstanceData));
}
