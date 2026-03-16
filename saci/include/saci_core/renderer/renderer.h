#ifndef SACI_BACKEND_RENDERER_SB_RENDERER_H
#define SACI_BACKEND_RENDERER_SB_RENDERER_H

#include "saci_util/types.h"
#include "saci_util/defines.h"

#include "saci_platform/gfx/graphics.h"

typedef struct CSaciRenderer CSaciRenderer;
typedef void* CSaciIndex;
typedef void* CSaciVertex;
typedef void* CSaciUniformData;

// TODO
#ifdef CSACI_RENDERER_STRUCT_EXPOSE
// Should have the complete renderer structure
#endif // CSACI_RENDERER_STRUCT_EXPOSE

SACI_API CSaciRenderer* csaci_renderer_new(const char* name);

SACI_API void csaci_renderer_begin(struct CSaciRenderer* rendr);

SACI_API void csaci_renderer_bind_texture(struct CSaciRenderer* rendr,
                                          const union PSaciTexture tex_id);

SACI_API SaciS32 csaci_renderer_get_uniform_id(struct CSaciRenderer* rendr,
                                               const char* const uniform_name);

SACI_API void csaci_renderer_set_uniform(struct CSaciRenderer* rendr,
                                         const SaciS32 uniform_id,
                                         const void* const value,
                                         const SaciDataType type);

SACI_API void csaci_renderer_bind_index_buffer(struct CSaciRenderer* rendr,
                                               const CSaciIndex* data, const SaciU64 count);

SACI_API void csaci_renderer_push_mesh(struct CSaciRenderer* rendr,
                                       const CSaciVertex* data, const SaciU64 count);

SACI_API void csaci_renderer_draw(const struct CSaciRenderer* rendr);

SACI_API void csaci_renderer_free(struct CSaciRenderer* rendr);

/* --- Renderer specific --- */

// This needs to be used with a instanced renderer, else it should crash
SACI_API void csaci_renderer_push_instance_data(struct CSaciRenderer* rendr, const void* data, SaciU32 loc, SaciU32 data_size);

SACI_API void csaci_renderer_set_instance_data(struct CSaciRenderer* rendr, SaciU32* location_array, SaciU32 location_array_count, const void* data_array, SaciU64 data_size, SaciU32 data_count);

#endif // SACI_BACKEND_RENDERER_SB_RENDERER_H
