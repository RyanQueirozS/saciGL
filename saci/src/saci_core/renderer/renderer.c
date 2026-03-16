#include "saci_core/renderer/renderer.h"

#include "saci_platform/gfx/internal/gfx.h"

#include "saci_util/internal/log.h"
#include "saci_util/log.h"

/* === Renderer Struct === */

struct CSaciRendererInterface {
    void (*new)(struct CSaciRenderer* self, SaciMemPool* mem, struct PSaciConfigRenderer* cfg, union PSaciGFXInfo* info);
    void (*begin)(const struct CSaciRenderer* self);
    void (*bind_texture)(struct CSaciRenderer* self, union PSaciTexture);
    void (*set_uniform)(struct CSaciRenderer* self, SaciS32, const void* const, SaciDataType);
    void (*bind_index_buffer)(struct CSaciRenderer* self, const CSaciIndex* data, const SaciU64 count);
    void (*push_mesh)(struct CSaciRenderer* self, const CSaciVertex* data, const SaciU64 count);
    void (*draw)(const struct CSaciRenderer* self);
    void (*free)(struct CSaciRenderer* self);
    void (*free_opts)(struct CSaciRenderer* self, int);
    SaciS32 (*get_uniform_id)(struct CSaciRenderer* self, const char* const);
};

struct CSaciRenderer {
    const char* name;
    const struct CSaciRendererInterface* interface;

    void* rendr;
};

/* === Header impl === */

CSaciRenderer* csaci_renderer_new(const char* name)
{
    struct PSaciConfigRenderer* cfg = NULL;
    union PSaciGFXInfo info = {0};
    const char* name_ptr = name;

    if (!name) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER,
                         SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_RENDERER,
                         "NULL renderer name");
        return NULL;
    }

#if 0
    psaci_cfg_renderer_get(name_ptr, &cfg, );
    csaci_init_shaders(&cfg, &info);
    csaci__renderer_instance_fill_default(&cfg, &info);
    SaciMemPool* pool = csaci_renderer_get_pool_from_cfg(&cfg, type);
    CSaciRenderer* rendr = saci_mem_pool_alloc(pool, sizeof(struct CSaciRenderer));
    rendr->type = type;

    switch (type) {
    case CSACI_RENDERER_STATIC:
        SACI_TODO_M;
        break;
    case CSACI_RENDERER_DYNAMIC:
        SACI_TODO_M;
        break;
    case CSACI_RENDERER_INSTANCE:
        rendr->interface = &CSACI_G_RENDERER_INSTANCE_INTERFACE_DEFAULT_INITIALIZER;
        rendr->interface->new (rendr, pool, &cfg, &info);
        break;
    }
#endif
    return NULL;
}
