#include "saci_platform/config/config.h"

#include "saci_platform/config/internal/lua.h"
#include "saci_platform/config/internal/renderer_prealloc.h"
#include "saci_platform/config/internal/renderer_populate.h"

#include "saci_util/internal/general.h"
#include "saci_util/memory.h"
#include "saci_util/log.h"
#include "saci_util/internal/log.h"

#include <saci_util/defines.h>
#include <string.h>

/* === Internal === */

SACI_INTERNAL void psaci__cfg_renderer_print_shaders(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_uniforms(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_samplers(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_vertex_attrib(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_index_data(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_instance_data(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_batch(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_bound(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_draw_data(const struct PSaciConfigRenderer* cfg);
SACI_INTERNAL void psaci__cfg_renderer_print_pipeline(const struct PSaciConfigRenderer* cfg);

/* === config.h === */

void psaci_cfg_renderer_get(const char* name, struct PSaciConfigRenderer* cfg_out, const char* cfg_file_path)
{
    PSaciLuaState* lua = psaci_lua_load(cfg_file_path);

    SaciMemPool* pool = NULL;

    psaci_cfg_renderer_prealloc(lua, cfg_out, name, &pool);
    psaci_cfg_renderer_populate_fields(lua, cfg_out);

    psaci_lua_close(lua);
}

SaciU64 psaci_cfg_renderer_cfg_size(const struct PSaciConfigRenderer* cfg)
{
    SaciU64 total_size = 0;
    return total_size;
}

void psaci_cfg_renderer_cleanup(struct PSaciConfigRenderer* cfg)
{
}

SACI_API void psaci_cfg_renderer_print(const struct PSaciConfigRenderer* cfg, int print_opts)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "=== Renderer Config '%s' ===\n", cfg->name ? cfg->name : "(unnamed)");

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_SHADERS)) {
        psaci__cfg_renderer_print_shaders(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_UNIFORMS)) {
        psaci__cfg_renderer_print_uniforms(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_SAMPLERS)) {
        psaci__cfg_renderer_print_samplers(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_VERTEX_ATTRIB)) {
        psaci__cfg_renderer_print_vertex_attrib(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_INDEX_DATA)) {
        psaci__cfg_renderer_print_index_data(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_INSTANCE_DATA)) {
        psaci__cfg_renderer_print_instance_data(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_BATCH)) {
        psaci__cfg_renderer_print_batch(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_BOUND)) {
        psaci__cfg_renderer_print_bound(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_DRAW_DATA)) {
        psaci__cfg_renderer_print_draw_data(cfg);
    }

    if (SACI_HAS_FLAG_M(print_opts, PSACI_CONFIG_RENDERER_PRINT_OPT_PIPELINE)) {
        psaci__cfg_renderer_print_pipeline(cfg);
    }
}

/* === Internal Impl === */

SACI_INTERNAL void psaci__cfg_renderer_print_shaders(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Shaders: vertex='%s', fragment='%s', geometry='%s'\n",
                     cfg->shaders.vert ? cfg->shaders.vert : "(none)",
                     cfg->shaders.frag ? cfg->shaders.frag : "(none)",
                     cfg->shaders.geom ? cfg->shaders.geom : "(none)");
}

SACI_INTERNAL void psaci__cfg_renderer_print_uniforms(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Uniforms: " SACI_FMTU64 " entries\n", cfg->uniform_array_length);
    for (SaciU64 i = 0; i < cfg->uniform_array_length; i++) {
        const struct PSaciRendererCfgUniform* uniform = &cfg->uniform_array[i];
        SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                         "  [%" PRIu64 "] name='%s', type=%u\n", i,
                         uniform->name ? uniform->name : "(unnamed)", uniform->type);
    }
}

SACI_INTERNAL void psaci__cfg_renderer_print_samplers(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Samplers: " SACI_FMTU64 " entries\n", cfg->sampler_array_length);
    for (SaciU64 i = 0; i < cfg->sampler_array_length; i++) {
        const struct PSaciRendererCfgSampler* sampler = &cfg->sampler_array[i];
        SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                         "  [%" PRIu64 "] name='%s', type=%u, binding=%u, unit=" SACI_FMTU64 "\n",
                         i, sampler->name ? sampler->name : "(unnamed)", sampler->type,
                         sampler->binding, sampler->unit);
    }
}

SACI_INTERNAL void psaci__cfg_renderer_print_vertex_attrib(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Vertex Attributes: element_size=" SACI_FMTU64 ", " SACI_FMTU64 " elements\n",
                     cfg->vertex_attributes.element_size_internal,
                     cfg->vertex_attributes.element_array_length);
    for (SaciU64 i = 0; i < cfg->vertex_attributes.element_array_length; i++) {
        const struct PSaciRendererCfgVertexElement* elem = &cfg->vertex_attributes.element_array[i];
        SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                         "  [%" PRIu64 "] name='%s', type=%u, offset=" SACI_FMTU64 ", location=%u\n",
                         i, elem->name ? elem->name : "(unnamed)", elem->type, elem->offset, elem->location);
    }
}

SACI_INTERNAL void psaci__cfg_renderer_print_index_data(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Index Data: element_size=" SACI_FMTU64 "\n",
                     cfg->index_data.element_size_internal);
}

SACI_INTERNAL void psaci__cfg_renderer_print_instance_data(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Instance Data: " SACI_FMTU64 " buffers\n",
                     cfg->instance_data.buffer_array_length);
    for (SaciU64 i = 0; i < cfg->instance_data.buffer_array_length; i++) {
        const struct PSaciRendererCfgInstanceBuffer* buffer = &cfg->instance_data.buffer_array[i];
        SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                         "  Buffer[%" PRIu64 "] name='%s', size=" SACI_FMTU64 ", layouts=" SACI_FMTU64 "\n",
                         i, buffer->name ? buffer->name : "(unnamed)",
                         buffer->size_byte_internal, buffer->layout_array_length);

        for (SaciU64 j = 0; j < buffer->layout_array_length; j++) {
            const struct PSaciRendererCfgInstanceBufferLayout* layout = &buffer->layout_array[j];
            SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                             "    Layout[%" PRIu64 "] name='%s', type=%u, offset=" SACI_FMTU64 ", location=%u\n",
                             j, layout->name ? layout->name : "(unnamed)", layout->type,
                             layout->offset, layout->location);
        }
    }
}

SACI_INTERNAL void psaci__cfg_renderer_print_batch(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Batch: capacity=" SACI_FMTU64 ", fixed=%s\n",
                     cfg->batch.capacity, cfg->batch.fixed_capacity ? "true" : "false");
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "  Index: capacity=" SACI_FMTU64 "\n", cfg->batch.index_cfg.capacity);
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "  Vertex: capacity=" SACI_FMTU64 "\n", cfg->batch.vertex_cfg.capacity);
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "  Instance: capacity=" SACI_FMTU64 "\n", cfg->batch.instance_cfg.capacity);
}

SACI_INTERNAL void psaci__cfg_renderer_print_bound(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Bound:\n  Index: capacity=" SACI_FMTU64 "\n  Instance: capacity=" SACI_FMTU64 "\n",
                     cfg->bound.index_cfg.capacity, cfg->bound.instance_cfg.capacity);
}

SACI_INTERNAL void psaci__cfg_renderer_print_draw_data(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Draw: primitive=%u, cull_mode=%u, front_face=%u\n",
                     cfg->draw.primitive, cfg->draw.cull_mode, cfg->draw.front_face);
}

SACI_INTERNAL void psaci__cfg_renderer_print_pipeline(const struct PSaciConfigRenderer* cfg)
{
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "Pipeline: depth_test=%s\n", cfg->pipeline.depth_test ? "true" : "false");
    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_RENDERER,
                     "  Blend: enabled=%s, opts=" SACI_FMTU64 "\n",
                     cfg->pipeline.blend.enabled ? "true" : "false", cfg->pipeline.blend.opts);
}
