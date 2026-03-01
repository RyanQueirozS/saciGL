#include "saci_platform/config/config_renderer.h"
#include "saci_util/memory.h"
#include "saci_util/internal/log.h"
#include "saci_util/log.h"
#include <saci_util/types.h>

/* = Header Impl == */

/* == Structs == */

struct PSaciConfigRenderer {
    SaciMemPool* pool;
    const struct PSaciConfigPreallocEntity* prealloc_table;
    const struct PSaciConfigPopulateEntity* populate_table;

    const struct PSaciConfigRendererInterface* interface;

    void* config_data;
};

/* == Funcs == */

struct PSaciConfigRenderer* psaci_config_renderer_new(
    const struct PSaciConfigRendererInterface* config_renderer_interface,
    const struct PSaciConfigPreallocEntity* prealloc_table,
    const struct PSaciConfigPopulateEntity* populate_table,
    SaciMemPool* pool)
{
    SaciBool has_all_fields = config_renderer_interface && prealloc_table &&
                              populate_table && pool;
    if (!has_all_fields) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Renderer config could not be created, contains null"
                         " params");
    }

    struct PSaciConfigRenderer* renderer_config =
        saci_mem_pool_alloc(pool, sizeof(struct PSaciConfigRenderer));

    renderer_config->interface = config_renderer_interface;
    renderer_config->prealloc_table = prealloc_table;
    renderer_config->populate_table = populate_table;

    renderer_config->interface->preallocate(
        renderer_config, pool, prealloc_table, &renderer_config->config_data);
    if (!renderer_config->config_data) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Renderer config could not be properlly allocated, "
                         "config_data is null");
    }
    renderer_config->interface->populate(
        renderer_config, populate_table, &renderer_config->config_data);

    return renderer_config;
}

SaciBool psaci_config_renderer_reset(struct PSaciConfigRenderer* cfg_renderer)
{
    if (!cfg_renderer) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Null or not initialized renderer config trying to be "
                         "reset");
        return SACI_FALSE;
    }
    cfg_renderer->interface->reset(cfg_renderer, cfg_renderer->config_data);
    return SACI_TRUE;
}

SaciBool psaci_config_renderer_free(struct PSaciConfigRenderer* cfg_renderer)
{
    if (!cfg_renderer) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Null or not initialized renderer config trying to be"
                         " freed");
        return SACI_FALSE;
    }
    cfg_renderer->interface->free(cfg_renderer);
    return SACI_TRUE;
}

SaciBool psaci_config_renderer_fetch(struct PSaciConfigRenderer* cfg_renderer)
{
    if (!cfg_renderer) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Null or not initialized renderer config trying to "
                         "fetch new data");
        return SACI_FALSE;
    }

    cfg_renderer->interface->reset(cfg_renderer, cfg_renderer->config_data);
    cfg_renderer->interface->preallocate(cfg_renderer, cfg_renderer->pool,
                                         cfg_renderer->prealloc_table,
                                         &cfg_renderer->config_data);
    cfg_renderer->interface->populate(cfg_renderer,
                                      cfg_renderer->populate_table,
                                      &cfg_renderer->config_data);
    return SACI_TRUE;
}

const void* psaci_config_renderer_get_data(
    const struct PSaciConfigRenderer* cfg_renderer)
{
    if (!cfg_renderer) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Null or not initialized renderer config trying to get"
                         " data");
        return NULL;
    }

    return cfg_renderer->config_data;
}

SaciMemPool* psaci_config_renderer_get_pool(
    struct PSaciConfigRenderer* cfg_renderer)
{
    if (!cfg_renderer) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Null or not initialized renderer config trying to get"
                         " memory pool");
        return NULL;
    }

    return cfg_renderer->pool;
}
