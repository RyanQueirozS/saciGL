#if 0
#  include "saci-backend/sb-renderer.h"

#  include "saci-utils/su-debug.h"
#  include "saci-utils/su-general.h"
#  include "saci-utils/su-types.h"

#  include "saci-backend/sb-config-manager.h"
#  include "saci-backend/sb-gfx.h"
#  include "saci-backend/sb-texture.h"

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>

/* === Renderer === */

// Used in config functions, checks if value is -1 if is uses default
#  define SET_FIXED_SIZE(var, default_value, getter) \
      var = (getter == -1) ? default_value : getter;

// Used in config functions, checks if value is 0 if is uses default
#  define SET_CAPACITY(var, default_value, getter) \
      var = (getter == 0) ? default_value : getter;

// TODO to be used
#  ifndef SC_TEXTURE_ARRAY_SIZE
#    ifdef _WIN32
// Windows
#      define SC_TEXTURE_ARRAY_SIZE 16
#    elif defined(__APPLE__) && defined(__MACH__)
// macOS
#      define SC_TEXTURE_ARRAY_SIZE 32
#    elif defined(__linux__)
// Linux
#      define SC_TEXTURE_ARRAY_SIZE 16
#    elif defined(__ANDROID__)
// Android
#      define SC_TEXTURE_ARRAY_SIZE 8
#    else
// Default case
#      define SC_TEXTURE_ARRAY_SIZE 8
#    endif // _WIN32
#  endif   // SC_TEXTURE_ARRAY_SIZE

#  ifndef SC_RENDERER_UNIFORMS
#    define SC_RENDERER_UNIFORMS

// TODO test these
// These might change and might not be used later...
#    define SC_U_MODEL_MATRIX_LOC 0
#    define SC_U_VIEW_MATRIX_LOC 1
#    define SC_U_PROJECTION_MATRIX_LOC 2
#    define SC_U_FLAGS 3
#    define SC_U_LIGHTING 4
#    define SC_U_TEXTURE_LOC 5
#    define SC_U_USE_TEXTURE_LOC 6

#  endif // SC_RENDERER_UNIFORMS

SA_INTERNAL const char* const sb_INSTANCE_VERT_SHADER =
    // TODO Evaluate if there should be multiple shaders, one for 2d, 3d
    // instancing, 3d dynamic mesh etc. Saci-Core needs to be
    // performatic but also "just work", so the user shouldn't need to
    // change it unless they want that '1%' boost
    "#version 330 core\n"

    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec4 a_color;\n"
    "layout (location = 2) in vec2 a_texcoord;\n"
    "layout (location = 3) in mat4 i_model_matrix;\n"
    "layout (location = 7) in vec4 i_color;\n"

    "uniform mat4 u_model_matrix;\n"
    "uniform mat4 u_view_matrix;\n"
    "uniform mat4 u_projection_matrix;\n"
    "uniform int u_flags;\n"
    "uniform vec4 u_lighting;\n"

    "out vec4 v_color;\n"
    "out vec2 v_texcoord;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * i_model_matrix * vec4(a_pos, 1.0);\n"
    "   v_color = a_color + i_color;\n" // TODO later there might be some flag or way to not only add but multiply or divide or whatever
    "   v_texcoord = a_texcoord;\n"
    "}\n\0";

SA_INTERNAL const char* const sb_FRAG_SHADER =
    "#version 330 core\n"

    "in vec4 v_color;\n"
    "in vec2 v_texcoord;\n"

    "uniform sampler2D u_texture;\n"
    "uniform bool u_use_texture;\n"

    "out vec4 frag_color;\n"

    "void main()\n"
    "{\n"
    "   if (u_use_texture) {\n"
    "       vec4 texcolor = texture(u_texture, v_texcoord);\n"
    "       frag_color = texcolor * v_color;\n"
    "   } else {\n"
    "       frag_color = v_color;\n"
    "   }\n"
    "}\n\0";
/* --- Renderer Helper --- */

SA_INTERNAL struct sb_StaticRenderer* sb__renderer_static_new(void);

SA_INTERNAL void sb__renderer_static_fill_default(struct sb_StaticRenderer* rendr);

SA_INTERNAL su_S32 sb__renderer_get_uniform_id(struct sb_Renderer* self,
                                               const char* const uniform_name);

SA_INTERNAL void sb__renderer_bind_index_buffer(struct sb_Renderer* self, const su_DArray* new_indices);

SA_INTERNAL void sb__renderer_instance_fill_default(struct sb_RendererConfig* cfg_out, const union sb_GFXInfo* gfx_info);

SA_INTERNAL void sb__renderer_init_bound(struct sb_RendererBound* bound_out, struct sb_RendererBoundInfo* bound_cfg_out, const struct sb_RendererConfig cfg);

SA_INTERNAL struct sb_RendererConfig sb__renderer_get_static_config(const char* const path);

// Initializes with default opengl data
SA_INTERNAL void sb__renderer_init_gl(union sb_GFXInfo* info_out, const struct sb_RendererConfig rendr_cfg);

SA_INTERNAL void sb__renderer_init_static_batch(struct sb_StaticRenderer* rendr, struct sb_RendererConfig cfg);

SA_INTERNAL void sb__renderer_init_instance_batch(struct sb_InstanceRenderer* rendr);

SA_INTERNAL union sb_GFXUniformValue sb__renderer_uniform_value_from_type(su_DataType type, const void* value);

SA_INTERNAL void sb__renderer_set_uniform_from_uniform_data(const struct sb_GFXUniformData uniform_data);

SA_INTERNAL void sb__renderer_draw_instance_batch(const struct sb_Renderer* rendr);

SA_INTERNAL void sb__renderer_bind_texture(struct sb_Renderer* self, su_TextureId tex_id);

SA_INTERNAL void sb__renderer_push_mesh_instanced(struct sb_Renderer* self,
                                                  const su_DArray* pos_array,
                                                  const su_DArray* uv_array,
                                                  const su_DArray* color_array);

/* --- Renderer Header Impl --- */

/* --- Renderer Helper impl --- */
#endif
