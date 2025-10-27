#include "saci-utils/config/su-config-manager.h"
#include "saci-utils/config/su-config.h"

#include <saci-utils/memory/su-memory.h>
#include <stdio.h>
#include <string.h>
#include "saci-utils/su-general.h"
#include "saci-utils/su-log.h"

// Platform-specific default library paths
#ifdef _WIN32 // Windows
#  define su_DEFAULT_OPENGL_PATH "C:\\Windows\\System32\\opengl32.dll"
#  define su_DEFAULT_GLFW_PATH "C:\\Program Files\\GLFW\\lib\\glfw3.dll"
#  define su_DEFAULT_GLAD_PATH "C:\\Program Files\\GLAD\\lib\\glad.dll"
#elif defined(__linux__) // Linux
#  define su_DEFAULT_OPENGL_PATH "/usr/lib/x86_64-linux-gnu/libGL.so"
#  define su_DEFAULT_GLFW_PATH "/usr/local/lib/libglfw.so"
#  define su_DEFAULT_GLAD_PATH "/usr/local/lib/libglad.so"
#elif defined(__APPLE__) // macOS
#  define su_DEFAULT_OPENGL_PATH "/System/Library/Frameworks/OpenGL.framework/OpenGL"
#  define su_DEFAULT_GLFW_PATH "/usr/local/lib/libglfw.dylib"
#  define su_DEFAULT_GLAD_PATH "/usr/local/lib/libglad.dylib"
#else
#  error "Unknown platform. Define paths for this platform."
#endif

/* === Internal === */

struct su_SymbolTable {
    const char* name;
    void** func_out;
};

SA_INTERNAL su_Bool su__cfg_manager_load_render_api(su_ConfigState* lua_state);

SA_INTERNAL su_Bool su__cfg_manager_load_render_loader(su_ConfigState* lua_state);

SA_INTERNAL su_Bool su__cfg_manager_load_window_api(su_ConfigState* lua_state);

SA_INTERNAL su_MemPool* su__cfg_manager_get_renderer_pool(su_ConfigState* cfg_state);

/* === Header impl === */

SA_INTERNAL struct su_ConfigManager su__cfg_manager = su_CFG_MANAGER_DEFAULT;

void su_cfg_manager_load_default(void) {
    su__cfg_manager = su_CFG_MANAGER_DEFAULT;
}

void su_cfg_manager_set(const struct su_ConfigManager cfg_manager) {
    su__cfg_manager = cfg_manager;
}

su_Bool su_cfg_manager_fetch(const char* path) {
    su_ConfigState* lua_state = su_config_load(path);
    if (!lua_state) {
        su_LOG_WARN_M(su_LOG_TYPE_USER, su_LOG_WARN_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Could not load config");
        return false;
    }
    su__cfg_manager.cfg_file_path = (char*)path;

    if (!su_config_push_global_table(lua_state, "Saci_base")) {
        su_config_close(lua_state);
        return false;
    }

    if (!su__cfg_manager_load_render_api(lua_state)) {
        su_config_pop(lua_state, 1);
        su_config_close(lua_state);
        return false;
    }

    if (!su__cfg_manager_load_render_loader(lua_state)) {
        su_config_pop(lua_state, 1);
        su_config_close(lua_state);
        return false;
    }

    if (!su__cfg_manager_load_window_api(lua_state)) {
        su_config_pop(lua_state, 1);
        su_config_close(lua_state);
        return false;
    }

    su_config_pop(lua_state, 1); // pop Saci_base table
    su_config_close(lua_state);

    return true;
}

SA_API enum su_RenderApi su_cfg_manager_get_renderer_api(void) {
    return su__cfg_manager.render_api_data.api;
}

const char* su_cfg_manager_get_renderer_api_path(void) {
    return su__cfg_manager.render_api_data.path_to_api;
}

SA_API enum su_RenderApiLoader su_cfg_manager_get_renderer_api_loader(void) {
    return su__cfg_manager.render_api_loader_data.api_loader;
}

const char* su_cfg_manager_get_renderer_api_loader_path(void) {
    return su__cfg_manager.render_api_loader_data.path_to_api;
}

SA_API enum su_WindowApi su_cfg_manager_get_window_api(void) {
    return su__cfg_manager.windowing_api_data.api;
}

const char* su_cfg_manager_get_window_api_path(void) {
    return su__cfg_manager.windowing_api_data.path_to_api;
}

void su_cfg_manager_get_renderer(const char* name, struct su_RendererConfig* cfg_out) {
    // cfg_out->name = name; // TODO
    su_ConfigState* lua_state = su_config_load(su__cfg_manager.cfg_file_path);
    if (!lua_state) {
        su_LOG_WARN_M(su_LOG_TYPE_USER, su_LOG_WARN_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Could not load config");
        return;
    }
    su_LOG_INFOF_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_CORE_CONFIG, "Loaded config at %s", name);
    if (!su_config_push_global_table(lua_state, "Saci_Backend")) {
        su_config_close(lua_state);
        return;
    }
    if (!su_config_push_field_table(lua_state, "renderers")) {
        su_config_close(lua_state);
        return;
    }
    if (!su_config_push_field_table(lua_state, name)) {
        su_config_close(lua_state);
        return;
    }

    su_MemPool* pool = su__cfg_manager_get_renderer_pool(lua_state);

    if (su_config_push_field_table(lua_state, "vertex")) {
        if (su_config_push_field_array(lua_state, "layout")) {
            su_U64 count = su_config_get_array_length(lua_state);
            cfg_out->vertex_data.layout_array_length = count;
            cfg_out->vertex_data.layout_array = su_mem_pool_alloc(pool, count * sizeof(struct su_RendererCfgVertexLayout));
            cfg_out->vertex_data.element_size_internal = 0;
            for (su_U64 i = 0; i < count; ++i) {
                if (su_config_push_array_entry(lua_state, i)) {
                    struct su_RendererCfgVertexLayout layout = {
                        .type = su_CAST_M(su_DataType)(su_config_get_enum(lua_state, "type")),
                        .offset = su_config_get_uint64(lua_state, "offset"),
                        .location = su_config_get_uint32(lua_state, "location"),
                    };

                    const char* name_str = su_config_get_str(lua_state, "name");
                    if (name_str) {
                        su_U64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
                        layout.name = su_mem_pool_alloc(pool, name_str_size);
                        su_mem_safe_copy(&layout.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
                    }
                    cfg_out->vertex_data.layout_array[i] = layout;
                    su_config_pop(lua_state, 1);
                }
            }
            su_config_pop(lua_state, 1);
        }
        su_config_pop(lua_state, 1);
    }
    if (su_config_push_field_table(lua_state, "index")) {
        cfg_out->index_data.element_size_internal = su_SIZE_OF_TYPE[su_config_get_enum(lua_state, "element_type")];
        su_config_pop(lua_state, 1);
    }
    if (su_config_push_field_table(lua_state, "shaders")) {
        const char* frag = su_config_get_str(lua_state, "frag");
        if (frag) {
            su_U64 frag_str_size = (strlen(frag) + 1) * sizeof(char);
            cfg_out->shaders.frag = su_mem_pool_alloc(pool, frag_str_size);
            su_mem_safe_copy(cfg_out->shaders.frag, frag_str_size, 0, frag, frag_str_size, 0, frag_str_size);
        }
        const char* vert = su_config_get_str(lua_state, "vert");
        if (vert) {
            su_U64 vert_str_size = (strlen(vert) + 1) * sizeof(char);
            cfg_out->shaders.vert = su_mem_pool_alloc(pool, vert_str_size);
            su_mem_safe_copy(cfg_out->shaders.vert, vert_str_size, 0, vert, vert_str_size, 0, vert_str_size);
        }
        const char* geom = su_config_get_str(lua_state, "geom");
        if (geom) {
            su_U64 geom_str_size = (strlen(geom) + 1) * sizeof(char);
            cfg_out->shaders.geom = su_mem_pool_alloc(pool, geom_str_size);
            su_mem_safe_copy(cfg_out->shaders.geom, geom_str_size, 0, geom, geom_str_size, 0, geom_str_size);
        }
        su_config_pop(lua_state, 1);
    }
    if (su_config_push_field_array(lua_state, "uniforms")) {
        su_U64 count = su_config_get_array_length(lua_state);
        cfg_out->uniform_array_length = count;
        cfg_out->uniform_array = su_mem_pool_alloc(pool, count * sizeof(struct su_RendererCfgUniform));

        for (su_U64 i = 0; i < count; i++) {
            if (su_config_push_array_entry(lua_state, i)) {
                struct su_RendererCfgUniform uniform = {
                    .type = su_CAST_M(su_DataType)(su_config_get_enum(lua_state, "type")),
                    .location = su_CAST_M(su_S32)(su_config_get_uint32(lua_state, "location")),
                };
                const char* name_str = su_config_get_str(lua_state, "name");
                if (name_str) {
                    su_U64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
                    uniform.name = su_mem_pool_alloc(pool, name_str_size);
                    su_mem_safe_copy(uniform.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
                }
                cfg_out->uniform_array[i] = uniform;
                su_config_pop(lua_state, 1);
            }
        }
        su_config_pop(lua_state, 1);
    }
    if (su_config_push_field_array(lua_state, "samplers")) {
        su_U64 count = su_config_get_array_length(lua_state);
        cfg_out->sampler_array_length = count;
        cfg_out->sampler_array = su_mem_pool_alloc(pool, count * sizeof(struct su_RendererCfgSampler));

        for (su_U64 i = 0; i < count; ++i) {
            if (su_config_push_array_entry(lua_state, i)) {
                struct su_RendererCfgSampler sampler = {
                    .type = su_CAST_M(su_DataType)(su_config_get_enum(lua_state, "type")),
                    .binding = su_CAST_M(su_S32)(su_config_get_uint32(lua_state, "binding")),
                };
                const char* name_str = su_config_get_str(lua_state, "name");
                if (name_str) {
                    su_U64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
                    sampler.name = su_mem_pool_alloc(pool, name_str_size);
                    su_mem_safe_copy(sampler.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
                }
                su_config_pop(lua_state, 1);
            }
        }
        su_config_pop(lua_state, 1);
    }
    if (su_config_push_field_table(lua_state, "batch")) {
        cfg_out->batch.capacity = su_config_get_uint64(lua_state, "capacity");
        if (su_config_push_field_table(lua_state, "index")) {
            cfg_out->batch.index_cfg.capacity = su_config_get_uint64(lua_state, "capacity");
            su_config_pop(lua_state, 1);
        }
        if (su_config_push_field_table(lua_state, "vertex")) {
            cfg_out->batch.vertex_cfg.capacity = su_config_get_uint64(lua_state, "capacity");
            su_config_pop(lua_state, 1);
        }
        if (su_config_push_field_table(lua_state, "instances")) {
            cfg_out->batch.instance_cfg.capacity = su_config_get_uint64(lua_state, "capacity");

            if (su_config_push_field_table(lua_state, "buffers")) {
                su_U64 buffer_count = su_config_get_array_length(lua_state);
                cfg_out->instance_data.buffer_array_length = buffer_count;
                cfg_out->instance_data.buffer_array = su_mem_pool_alloc(pool, sizeof(struct su_RendererCfgInstanceBuffer) * buffer_count);

                for (su_U64 b = 0; b < buffer_count; ++b) {
                    if (su_config_push_array_entry(lua_state, b)) {
                        struct su_RendererCfgInstanceBuffer buffer = {0};
                        const char* name_str = su_config_get_str(lua_state, "name");
                        if (name_str) {
                            su_U64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
                            buffer.name = su_mem_pool_alloc(pool, name_str_size);
                            su_mem_safe_copy(buffer.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
                        }

                        if (su_config_push_field_array(lua_state, "layout")) {
                            su_U64 layout_count = su_config_get_array_length(lua_state);
                            buffer.layout_array_length = layout_count;
                            buffer.layout_array = su_mem_pool_alloc(pool, layout_count * sizeof(struct su_RendererCfgInstanceBufferLayout));
                            buffer.size_byte_internal = 0;

                            for (su_U64 l = 0; l < layout_count; ++l) {
                                if (su_config_push_array_entry(lua_state, l)) {
                                    struct su_RendererCfgInstanceBufferLayout layout = {
                                        .type = su_CAST_M(su_DataType)(su_config_get_enum(lua_state, "type")),
                                        .offset = su_config_get_uint64(lua_state, "offset"),
                                        .location = su_config_get_uint32(lua_state, "location"),
                                    };
                                    const char* layout_name_str = su_config_get_str(lua_state, "name");
                                    if (layout_name_str) {
                                        su_U64 layout_name_str_size = (strlen(name_str) + 1) * sizeof(char);
                                        layout.name = su_mem_pool_alloc(pool, layout_name_str_size);
                                        su_mem_safe_copy(layout.name, layout_name_str_size, 0, name_str, layout_name_str_size, 0, layout_name_str_size);
                                    }
                                    buffer.size_byte_internal += su_SIZE_OF_TYPE[layout.type];
                                    buffer.layout_array[l] = layout;
                                    su_config_pop(lua_state, 1);
                                }
                            }

                            su_config_pop(lua_state, 1); // pop layout array
                        }

                        cfg_out->instance_data.buffer_array[b] = buffer;
                        su_config_pop(lua_state, 1); // pop buffer entry
                    }
                }

                su_config_pop(lua_state, 1); // pop buffers array
            }

            su_config_pop(lua_state, 1); // pop instances
        }
        if (su_config_push_field_table(lua_state, "bound")) {
            cfg_out->bound.index_cfg.capacity = su_config_get_uint64(lua_state, "index_capacity");
            cfg_out->bound.instance_cfg.capacity = su_config_get_uint64(lua_state, "instance_capacity");
            su_config_pop(lua_state, 1);
        }
        if (su_config_push_field_table(lua_state, "draw")) {
            cfg_out->draw = (struct su_RendererCfgDraw){
                .primitive = su_CAST_M(enum su_RendererPrimitives)(su_config_get_enum(lua_state, "primitives")),
                .cull_mode = su_CAST_M(enum su_RendererCullMode)(su_config_get_enum(lua_state, "cull_mode")),
                .front_face = su_CAST_M(enum su_RendererFrontFace)(su_config_get_enum(lua_state, "front_face")),
            };
            su_config_pop(lua_state, 1);
        }
        if (su_config_push_field_table(lua_state, "pipeline")) {
            cfg_out->pipeline.depth_test = su_config_get_bool(lua_state, "depth_test");
            if (su_config_push_field_table(lua_state, "blend")) {
                cfg_out->pipeline.blend.enabled = su_config_get_bool(lua_state, "enabled");
                su_config_pop(lua_state, 1);
            }
            su_config_pop(lua_state, 1);
        }
    }
}

SA_API su_U64 su_cfg_manager_render_cfg_size(const struct su_RendererConfig* cfg) {
    su_U64 total_size = 0;
    return total_size;
}

SA_API void su_cfg_manager_cleanup_renderer_cfg(struct su_RendererConfig* cfg) {
}

/* === Internal Impl === */

SA_INTERNAL enum su_RenderApi su__parse_render_api(const char* api_str) {
    if (!api_str) {
        exit(1);
    }
    if (strcmp(api_str, "OpenGL") == 0) {
        return su_RENDERER_API_OPENGL;
    }
    if (strcmp(api_str, "Vulkan") == 0) {
        return su_RENDERER_API_VULKAN;
    }
    exit(1);
}

SA_INTERNAL enum su_RenderApiLoader su__parse_render_loader(const char* loader_str) {
    if (!loader_str) {
        exit(1);
    }
    if (strcmp(loader_str, "GLAD") == 0)
        return su_RENDERER_LOADER_GLAD;
    exit(1);
}

SA_INTERNAL enum su_WindowApi su__parse_window_api(const char* api_str) {
    if (!api_str) {
        exit(1);
    }
    if (strcmp(api_str, "GLFW") == 0) {
        return su_WINDOW_API_GLFW;
    }
    exit(1);
}

SA_INTERNAL su_Bool su__cfg_manager_load_render_api(su_ConfigState* lua_state) {
    if (!su_config_push_field_table(lua_state, "render_api")) {
        return false;
    }

    const char* api_str = su_config_get_str(lua_state, "api");
    const char* path_str = su_config_get_str(lua_state, "path");

    su__cfg_manager.render_api_data.api = su__parse_render_api(api_str);

    su__cfg_manager.render_api_data.path_to_api = path_str ? strdup(path_str) : NULL;

    su_config_pop(lua_state, 1); // pop render_api table
    return true;
}

SA_INTERNAL su_Bool su__cfg_manager_load_render_loader(su_ConfigState* lua_state) {
    if (!su_config_push_field_table(lua_state, "render_loader")) {
        return false;
    }

    const char* api_str = su_config_get_str(lua_state, "api");
    const char* path_str = su_config_get_str(lua_state, "path");

    su__cfg_manager.render_api_loader_data.api_loader = su__parse_render_loader(api_str);

    free(su__cfg_manager.render_api_loader_data.path_to_api);
    su__cfg_manager.render_api_loader_data.path_to_api = path_str ? strdup(path_str) : NULL;

    su_config_pop(lua_state, 1); // pop render_loader table
    return true;
}

SA_INTERNAL su_Bool su__cfg_manager_load_window_api(su_ConfigState* lua_state) {
    if (!su_config_push_field_table(lua_state, "window_api")) {
        return false;
    }

    const char* api_str = su_config_get_str(lua_state, "api");
    const char* path_str = su_config_get_str(lua_state, "path");

    su__cfg_manager.windowing_api_data.api = su__parse_window_api(api_str);

    free(su__cfg_manager.windowing_api_data.path_to_api);
    su__cfg_manager.windowing_api_data.path_to_api = path_str ? strdup(path_str) : NULL;

    su_config_pop(lua_state, 1); // pop window_api table
    return true;
}

// Returns a memory pool with preallocated memory to use in the renderer cfg.
SA_INTERNAL su_MemPool* su__cfg_manager_get_renderer_pool(su_ConfigState* cfg_state) {
    su_U64 total_size = 0;

    if (su_config_push_field_table(cfg_state, "vertex")) {
        if (su_config_push_field_array(cfg_state, "layout")) {
            su_U64 count = su_config_get_array_length(cfg_state);
            total_size += count * sizeof(struct su_RendererCfgVertexLayout);

            for (su_U64 i = 0; i < count; ++i) {
                if (su_config_push_array_entry(cfg_state, i)) {
                    const char* name = su_config_get_str(cfg_state, "name");
                    if (name) {
                        total_size += (strlen(name) + 1) * sizeof(char);
                    }
                    su_config_pop(cfg_state, 1);
                }
            }
            su_config_pop(cfg_state, 1); // pop layout array
        }
        su_config_pop(cfg_state, 1); // pop vertex table
    }

    if (su_config_push_field_table(cfg_state, "shaders")) {
        const char* frag = su_config_get_str(cfg_state, "frag");
        const char* vert = su_config_get_str(cfg_state, "vert");
        const char* geom = su_config_get_str(cfg_state, "geom");

        if (frag)
            total_size += (strlen(frag) + 1) * sizeof(char);
        if (vert)
            total_size += (strlen(vert) + 1) * sizeof(char);
        if (geom)
            total_size += (strlen(geom) + 1) * sizeof(char);

        su_config_pop(cfg_state, 1); // pop shaders table
    }

    if (su_config_push_field_array(cfg_state, "uniforms")) {
        su_U64 count = su_config_get_array_length(cfg_state);
        total_size += count * sizeof(struct su_RendererCfgUniform);

        for (su_U64 i = 0; i < count; ++i) {
            if (su_config_push_array_entry(cfg_state, i)) {
                const char* name = su_config_get_str(cfg_state, "name");
                if (name) {
                    total_size += (strlen(name) + 1) * sizeof(char);
                }
                su_config_pop(cfg_state, 1);
            }
        }
        su_config_pop(cfg_state, 1); // pop uniforms array
    }

    if (su_config_push_field_array(cfg_state, "samplers")) {
        su_U64 count = su_config_get_array_length(cfg_state);
        total_size += count * sizeof(struct su_RendererCfgSampler);

        for (su_U64 i = 0; i < count; ++i) {
            if (su_config_push_array_entry(cfg_state, i)) {
                const char* name = su_config_get_str(cfg_state, "name");
                if (name) {
                    total_size += (strlen(name) + 1) * sizeof(char);
                }
                su_config_pop(cfg_state, 1);
            }
        }
        su_config_pop(cfg_state, 1); // pop samplers array
    }

    if (su_config_push_field_table(cfg_state, "batch")) {
        if (su_config_push_field_table(cfg_state, "instances")) {
            if (su_config_push_field_array(cfg_state, "buffers")) {
                su_U64 buffer_count = su_config_get_array_length(cfg_state);
                total_size += buffer_count * sizeof(struct su_RendererCfgInstanceBuffer);

                for (su_U64 b = 0; b < buffer_count; ++b) {
                    if (su_config_push_array_entry(cfg_state, b)) {
                        const char* buff_name = su_config_get_str(cfg_state, "name");
                        if (buff_name) {
                            total_size += (strlen(buff_name) + 1) * sizeof(char);
                        }

                        // Handle layout array inside each buffer
                        if (su_config_push_field_array(cfg_state, "layout")) {
                            su_U64 layout_count = su_config_get_array_length(cfg_state);
                            total_size += layout_count * sizeof(struct su_RendererCfgInstanceBufferLayout);

                            for (su_U64 l = 0; l < layout_count; ++l) {
                                if (su_config_push_array_entry(cfg_state, l)) {
                                    const char* layout_name = su_config_get_str(cfg_state, "name");
                                    if (layout_name) {
                                        total_size += (strlen(layout_name) + 1) * sizeof(char);
                                    }
                                    su_config_pop(cfg_state, 1);
                                }
                            }
                            su_config_pop(cfg_state, 1); // pop layout array
                        }

                        su_config_pop(cfg_state, 1); // pop buffer entry
                    }
                }

                su_config_pop(cfg_state, 1); // pop buffers array
            }
            su_config_pop(cfg_state, 1); // pop instances table
        }
        su_config_pop(cfg_state, 1); // pop batch table
    }

    return su_mem_create_pool(su_MEM_CONTEXT_CONFIG, total_size);
}
