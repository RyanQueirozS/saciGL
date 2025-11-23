#include "saci-utils/config/su-config-manager.h"
#include "saci-utils/config/su-lua.h"

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

SA_INTERNAL su_Bool su__cfg_manager_load_render_api(su_LuaState* lua_state);

SA_INTERNAL su_Bool su__cfg_manager_load_render_loader(su_LuaState* lua_state);

SA_INTERNAL su_Bool su__cfg_manager_load_window_api(su_LuaState* lua_state);

SA_INTERNAL su_MemPool* su__cfg_manager_get_renderer_pool(su_LuaState* cfg_state);

SA_INTERNAL void su__load_renderer_vertex_data(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool);
SA_INTERNAL void su__load_renderer_index_data(su_LuaState* lua, struct su_RendererConfig* cfg_out);
SA_INTERNAL void su__load_renderer_shaders(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool);
SA_INTERNAL void su__load_renderer_uniforms(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool);
SA_INTERNAL void su__load_renderer_samplers(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool);
SA_INTERNAL void su__load_renderer_batch(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool);
SA_INTERNAL void su__load_renderer_bound(su_LuaState* lua, struct su_RendererConfig* cfg_out);
SA_INTERNAL void su__load_renderer_draw(su_LuaState* lua, struct su_RendererConfig* cfg_out);
SA_INTERNAL void su__load_renderer_pipeline(su_LuaState* lua, struct su_RendererConfig* cfg_out);

/* === Header impl === */

SA_INTERNAL struct su_ConfigManager su__cfg_manager = su_CFG_MANAGER_DEFAULT;

void su_cfg_manager_load_default(void)
{
    su__cfg_manager = su_CFG_MANAGER_DEFAULT;
}

void su_cfg_manager_set(const struct su_ConfigManager cfg_manager)
{
    su__cfg_manager = cfg_manager;
}

su_Bool su_cfg_manager_fetch(const char* path)
{
    su_LuaState* lua_state = su_lua_load(path);
    if (!lua_state) {
        su_LOG_WARN_M(su_LOG_TYPE_USER, su_LOG_WARN_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_CONFIG, "Could not load config");
        return false;
    }
    su__cfg_manager.cfg_file_path = (char*)path;

    if (!su_lua_push_global_table(lua_state, "Saci_base")) {
        su_lua_close(lua_state);
        return false;
    }

    if (!su__cfg_manager_load_render_api(lua_state)) {
        su_lua_pop(lua_state, 1);
        su_lua_close(lua_state);
        return false;
    }

    if (!su__cfg_manager_load_render_loader(lua_state)) {
        su_lua_pop(lua_state, 1);
        su_lua_close(lua_state);
        return false;
    }

    if (!su__cfg_manager_load_window_api(lua_state)) {
        su_lua_pop(lua_state, 1);
        su_lua_close(lua_state);
        return false;
    }

    su_lua_pop(lua_state, 1); // pop Saci_base table
    su_lua_close(lua_state);

    return true;
}

SA_API enum su_RenderApi su_cfg_manager_get_renderer_api(void)
{
    return su__cfg_manager.render_api_data.api;
}

const char* su_cfg_manager_get_renderer_api_path(void)
{
    return su__cfg_manager.render_api_data.path_to_api;
}

SA_API enum su_RenderApiLoader su_cfg_manager_get_renderer_api_loader(void)
{
    return su__cfg_manager.render_api_loader_data.api_loader;
}

const char* su_cfg_manager_get_renderer_api_loader_path(void)
{
    return su__cfg_manager.render_api_loader_data.path_to_api;
}

SA_API enum su_WindowApi su_cfg_manager_get_window_api(void)
{
    return su__cfg_manager.windowing_api_data.api;
}

const char* su_cfg_manager_get_window_api_path(void)
{
    return su__cfg_manager.windowing_api_data.path_to_api;
}

void su_cfg_manager_get_renderer(const char* name, struct su_RendererConfig* cfg_out)
{
    su_LuaState* lua = su_lua_load(su__cfg_manager.cfg_file_path);
    if (!lua) {
        su_LOG_WARN_M(su_LOG_TYPE_USER, su_LOG_WARN_SEVERITY_MEDIUM,
                      su_LOG_CONTEXT_CORE_CONFIG, "Could not load config");
        return;
    }

    su_LOG_INFOF_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_CORE_CONFIG, "Loaded config at %s", name);

    if (!su_lua_push_global_table(lua, "Saci_Backend") ||
        !su_lua_push_field_table(lua, "renderers") ||
        !su_lua_push_field_table(lua, name)) {
        su_lua_close(lua);
        return;
    }

    su_MemPool* pool = su__cfg_manager_get_renderer_pool(lua);

    su__load_renderer_vertex_data(lua, cfg_out, pool);
    su__load_renderer_index_data(lua, cfg_out);
    su__load_renderer_shaders(lua, cfg_out, pool);
    su__load_renderer_uniforms(lua, cfg_out, pool);
    su__load_renderer_samplers(lua, cfg_out, pool);
    su__load_renderer_batch(lua, cfg_out, pool);
    su__load_renderer_bound(lua, cfg_out);
    su__load_renderer_draw(lua, cfg_out);
    su__load_renderer_pipeline(lua, cfg_out);

    su_lua_close(lua);
}

SA_API su_U64 su_cfg_manager_render_cfg_size(const struct su_RendererConfig* cfg)
{
    su_U64 total_size = 0;
    return total_size;
}

SA_API void su_cfg_manager_cleanup_renderer_cfg(struct su_RendererConfig* cfg)
{
}

/* === Internal Impl === */

SA_INTERNAL enum su_RenderApi su__parse_render_api(const char* api_str)
{
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

SA_INTERNAL enum su_RenderApiLoader su__parse_render_loader(const char* loader_str)
{
    if (!loader_str) {
        exit(1);
    }
    if (strcmp(loader_str, "GLAD") == 0)
        return su_RENDERER_LOADER_GLAD;
    exit(1);
}

SA_INTERNAL enum su_WindowApi su__parse_window_api(const char* api_str)
{
    if (!api_str) {
        exit(1);
    }
    if (strcmp(api_str, "GLFW") == 0) {
        return su_WINDOW_API_GLFW;
    }
    exit(1);
}

SA_INTERNAL su_Bool su__cfg_manager_load_render_api(su_LuaState* lua_state)
{
    if (!su_lua_push_field_table(lua_state, "render_api")) {
        return false;
    }

    const char* api_str = su_lua_get_str(lua_state, "api");
    const char* path_str = su_lua_get_str(lua_state, "path");

    su__cfg_manager.render_api_data.api = su__parse_render_api(api_str);

    su__cfg_manager.render_api_data.path_to_api = path_str ? strdup(path_str) : NULL;

    su_lua_pop(lua_state, 1); // pop render_api table
    return true;
}

SA_INTERNAL su_Bool su__cfg_manager_load_render_loader(su_LuaState* lua_state)
{
    if (!su_lua_push_field_table(lua_state, "render_loader")) {
        return false;
    }

    const char* api_str = su_lua_get_str(lua_state, "api");
    const char* path_str = su_lua_get_str(lua_state, "path");

    su__cfg_manager.render_api_loader_data.api_loader = su__parse_render_loader(api_str);

    free(su__cfg_manager.render_api_loader_data.path_to_api);
    su__cfg_manager.render_api_loader_data.path_to_api = path_str ? strdup(path_str) : NULL;

    su_lua_pop(lua_state, 1); // pop render_loader table
    return true;
}

SA_INTERNAL su_Bool su__cfg_manager_load_window_api(su_LuaState* lua_state)
{
    if (!su_lua_push_field_table(lua_state, "window_api")) {
        return false;
    }

    const char* api_str = su_lua_get_str(lua_state, "api");
    const char* path_str = su_lua_get_str(lua_state, "path");

    su__cfg_manager.windowing_api_data.api = su__parse_window_api(api_str);

    free(su__cfg_manager.windowing_api_data.path_to_api);
    su__cfg_manager.windowing_api_data.path_to_api = path_str ? strdup(path_str) : NULL;

    su_lua_pop(lua_state, 1); // pop window_api table
    return true;
}

// Returns a memory pool with preallocated memory to use in the renderer cfg.
SA_INTERNAL su_MemPool* su__cfg_manager_get_renderer_pool(su_LuaState* cfg_state)
{
    su_U64 total_size = 0;

    if (su_lua_push_field_table(cfg_state, "vertex")) {
        if (su_lua_push_field_array(cfg_state, "layout")) {
            su_U64 count = su_lua_get_array_length(cfg_state);
            total_size += count * sizeof(struct su_RendererCfgVertexLayout);

            for (su_U64 i = 0; i < count; ++i) {
                if (su_lua_push_array_entry(cfg_state, i)) {
                    const char* name = su_lua_get_str(cfg_state, "name");
                    if (name) {
                        total_size += (strlen(name) + 1) * sizeof(char);
                    }
                    su_lua_pop(cfg_state, 1);
                }
            }
            su_lua_pop(cfg_state, 1); // pop layout array
        }
        su_lua_pop(cfg_state, 1); // pop vertex table
    }

    if (su_lua_push_field_table(cfg_state, "shaders")) {
        const char* frag = su_lua_get_str(cfg_state, "frag");
        const char* vert = su_lua_get_str(cfg_state, "vert");
        const char* geom = su_lua_get_str(cfg_state, "geom");

        if (frag)
            total_size += (strlen(frag) + 1) * sizeof(char);
        if (vert)
            total_size += (strlen(vert) + 1) * sizeof(char);
        if (geom)
            total_size += (strlen(geom) + 1) * sizeof(char);

        su_lua_pop(cfg_state, 1); // pop shaders table
    }

    if (su_lua_push_field_array(cfg_state, "uniforms")) {
        su_U64 count = su_lua_get_array_length(cfg_state);
        total_size += count * sizeof(struct su_RendererCfgUniform);

        for (su_U64 i = 0; i < count; ++i) {
            if (su_lua_push_array_entry(cfg_state, i)) {
                const char* name = su_lua_get_str(cfg_state, "name");
                if (name) {
                    total_size += (strlen(name) + 1) * sizeof(char);
                }
                su_lua_pop(cfg_state, 1);
            }
        }
        su_lua_pop(cfg_state, 1); // pop uniforms array
    }

    if (su_lua_push_field_array(cfg_state, "samplers")) {
        su_U64 count = su_lua_get_array_length(cfg_state);
        total_size += count * sizeof(struct su_RendererCfgSampler);

        for (su_U64 i = 0; i < count; ++i) {
            if (su_lua_push_array_entry(cfg_state, i)) {
                const char* name = su_lua_get_str(cfg_state, "name");
                if (name) {
                    total_size += (strlen(name) + 1) * sizeof(char);
                }
                su_lua_pop(cfg_state, 1);
            }
        }
        su_lua_pop(cfg_state, 1); // pop samplers array
    }

    if (su_lua_push_field_table(cfg_state, "batch")) {
        if (su_lua_push_field_table(cfg_state, "instances")) {
            if (su_lua_push_field_array(cfg_state, "buffers")) {
                su_U64 buffer_count = su_lua_get_array_length(cfg_state);
                total_size += buffer_count * sizeof(struct su_RendererCfgInstanceBuffer);

                for (su_U64 b = 0; b < buffer_count; ++b) {
                    if (su_lua_push_array_entry(cfg_state, b)) {
                        const char* buff_name = su_lua_get_str(cfg_state, "name");
                        if (buff_name) {
                            total_size += (strlen(buff_name) + 1) * sizeof(char);
                        }

                        // Handle layout array inside each buffer
                        if (su_lua_push_field_array(cfg_state, "layout")) {
                            su_U64 layout_count = su_lua_get_array_length(cfg_state);
                            total_size += layout_count * sizeof(struct su_RendererCfgInstanceBufferLayout);

                            for (su_U64 l = 0; l < layout_count; ++l) {
                                if (su_lua_push_array_entry(cfg_state, l)) {
                                    const char* layout_name = su_lua_get_str(cfg_state, "name");
                                    if (layout_name) {
                                        total_size += (strlen(layout_name) + 1) * sizeof(char);
                                    }
                                    su_lua_pop(cfg_state, 1);
                                }
                            }
                            su_lua_pop(cfg_state, 1); // pop layout array
                        }

                        su_lua_pop(cfg_state, 1); // pop buffer entry
                    }
                }

                su_lua_pop(cfg_state, 1); // pop buffers array
            }
            su_lua_pop(cfg_state, 1); // pop instances table
        }
        su_lua_pop(cfg_state, 1); // pop batch table
    }

    return su_mem_create_pool(su_MEM_CONTEXT_CONFIG, total_size);
}

void su__load_renderer_vertex_data(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool)
{
    if (!su_lua_push_field_table(lua, "vertex"))
        return;

    if (su_lua_push_field_array(lua, "layout")) {
        su_U64 count = su_lua_get_array_length(lua);
        cfg_out->vertex_data.layout_array_length = count;
        cfg_out->vertex_data.layout_array = su_mem_pool_alloc(pool, count * sizeof(struct su_RendererCfgVertexLayout));
        cfg_out->vertex_data.element_size_internal = 0;

        for (su_U64 i = 0; i < count; ++i) {
            if (su_lua_push_array_entry(lua, i)) {
                struct su_RendererCfgVertexLayout layout = {
                    .type = su_CAST_M(su_DataType)(su_lua_get_enum(lua, "type")),
                    .offset = su_lua_get_uint64(lua, "offset"),
                    .location = su_lua_get_uint32(lua, "location"),
                };

                const char* name_str = su_lua_get_str(lua, "name");
                if (name_str) {
                    su_U64 size = (strlen(name_str) + 1) * sizeof(char);
                    layout.name = su_mem_pool_alloc(pool, size);
                    su_mem_safe_copy(layout.name, size, 0, name_str, size, 0, size);
                }

                cfg_out->vertex_data.layout_array[i] = layout;
                su_lua_pop(lua, 1);
            }
        }

        su_lua_pop(lua, 1);
    }

    su_lua_pop(lua, 1);
}

void su__load_renderer_index_data(su_LuaState* lua, struct su_RendererConfig* cfg_out)
{
    if (su_lua_push_field_table(lua, "index")) {
        cfg_out->index_data.element_size_internal = su_SIZE_OF_TYPE[su_lua_get_enum(lua, "element_type")];
        su_lua_pop(lua, 1);
    }
}

void su__load_renderer_shaders(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool)
{
    if (!su_lua_push_field_table(lua, "shaders"))
        return;

    const char* frag = su_lua_get_str(lua, "frag");
    if (frag) {
        su_U64 size = (strlen(frag) + 1) * sizeof(char);
        cfg_out->shaders.frag = su_mem_pool_alloc(pool, size);
        su_mem_safe_copy(cfg_out->shaders.frag, size, 0, frag, size, 0, size);
    }

    const char* vert = su_lua_get_str(lua, "vert");
    if (vert) {
        su_U64 size = (strlen(vert) + 1) * sizeof(char);
        cfg_out->shaders.vert = su_mem_pool_alloc(pool, size);
        su_mem_safe_copy(cfg_out->shaders.vert, size, 0, vert, size, 0, size);
    }

    const char* geom = su_lua_get_str(lua, "geom");
    if (geom) {
        su_U64 size = (strlen(geom) + 1) * sizeof(char);
        cfg_out->shaders.geom = su_mem_pool_alloc(pool, size);
        su_mem_safe_copy(cfg_out->shaders.geom, size, 0, geom, size, 0, size);
    }

    su_lua_pop(lua, 1);
}

void su__load_renderer_uniforms(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool)
{
    if (su_lua_push_field_array(lua, "uniforms")) {
        su_U64 count = su_lua_get_array_length(lua);
        cfg_out->uniform_array_length = count;
        cfg_out->uniform_array = su_mem_pool_alloc(pool, count * sizeof(struct su_RendererCfgUniform));

        for (su_U64 i = 0; i < count; i++) {
            if (su_lua_push_array_entry(lua, i)) {
                struct su_RendererCfgUniform uniform = {
                    .type = su_CAST_M(su_DataType)(su_lua_get_enum(lua, "type")),
                    .location = su_CAST_M(su_S32)(su_lua_get_uint32(lua, "location")),
                };
                const char* name_str = su_lua_get_str(lua, "name");
                if (name_str) {
                    su_U64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
                    uniform.name = su_mem_pool_alloc(pool, name_str_size);
                    su_mem_safe_copy(uniform.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
                }
                cfg_out->uniform_array[i] = uniform;
                su_lua_pop(lua, 1);
            }
        }
        su_lua_pop(lua, 1);
    }
}

void su__load_renderer_samplers(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool)
{
    if (su_lua_push_field_array(lua, "samplers")) {
        su_U64 count = su_lua_get_array_length(lua);
        cfg_out->sampler_array_length = count;
        cfg_out->sampler_array = su_mem_pool_alloc(pool, count * sizeof(struct su_RendererCfgSampler));

        for (su_U64 i = 0; i < count; ++i) {
            if (su_lua_push_array_entry(lua, i)) {
                struct su_RendererCfgSampler sampler = {
                    .type = su_CAST_M(su_DataType)(su_lua_get_enum(lua, "type")),
                    .binding = su_CAST_M(su_S32)(su_lua_get_uint32(lua, "binding")),
                };
                const char* name_str = su_lua_get_str(lua, "name");
                if (name_str) {
                    su_U64 name_str_size = (strlen(name_str) + 1) * sizeof(char);
                    sampler.name = su_mem_pool_alloc(pool, name_str_size);
                    su_mem_safe_copy(sampler.name, name_str_size, 0, name_str, name_str_size, 0, name_str_size);
                }
                su_lua_pop(lua, 1);
            }
        }
        su_lua_pop(lua, 1);
    }
}

void su__load_renderer_batch(su_LuaState* lua, struct su_RendererConfig* cfg_out, su_MemPool* pool)
{
    if (!su_lua_push_field_table(lua, "batch"))
        return;

    cfg_out->batch.capacity = su_lua_get_uint64(lua, "capacity");

    if (su_lua_push_field_table(lua, "index")) {
        cfg_out->batch.index_cfg.capacity = su_lua_get_uint64(lua, "capacity");
        su_lua_pop(lua, 1); // pop index
    }

    if (su_lua_push_field_table(lua, "vertex")) {
        cfg_out->batch.vertex_cfg.capacity = su_lua_get_uint64(lua, "capacity");
        su_lua_pop(lua, 1); // pop vertex
    }

    if (su_lua_push_field_table(lua, "instances")) {
        cfg_out->batch.instance_cfg.capacity = su_lua_get_uint64(lua, "capacity");

        if (su_lua_push_field_array(lua, "buffers")) { // FIXED: should be push_field_array
            su_U64 buffer_count = su_lua_get_array_length(lua);
            cfg_out->instance_data.buffer_array_length = buffer_count;
            cfg_out->instance_data.buffer_array = su_mem_pool_alloc(pool, sizeof(struct su_RendererCfgInstanceBuffer) * buffer_count);

            for (su_U64 b = 0; b < buffer_count; ++b) {
                if (su_lua_push_array_entry(lua, b)) {
                    struct su_RendererCfgInstanceBuffer buffer = {0};

                    const char* name_str = su_lua_get_str(lua, "name");
                    if (name_str) {
                        su_U64 name_size = (strlen(name_str) + 1);
                        buffer.name = su_mem_pool_alloc(pool, name_size);
                        su_mem_safe_copy(buffer.name, name_size, 0, name_str, name_size, 0, name_size);
                    }

                    if (su_lua_push_field_array(lua, "layout")) {
                        su_U64 layout_count = su_lua_get_array_length(lua);
                        buffer.layout_array_length = layout_count;
                        buffer.layout_array = su_mem_pool_alloc(pool, layout_count * sizeof(struct su_RendererCfgInstanceBufferLayout));
                        buffer.size_byte_internal = 0;

                        for (su_U64 l = 0; l < layout_count; ++l) {
                            if (su_lua_push_array_entry(lua, l)) {
                                struct su_RendererCfgInstanceBufferLayout layout = {
                                    .type = su_CAST_M(su_DataType)(su_lua_get_enum(lua, "type")),
                                    .offset = su_lua_get_uint64(lua, "offset"),
                                    .location = su_lua_get_uint32(lua, "location"),
                                };

                                const char* layout_name_str = su_lua_get_str(lua, "name");
                                if (layout_name_str) {
                                    su_U64 layout_name_size = (strlen(layout_name_str) + 1);
                                    layout.name = su_mem_pool_alloc(pool, layout_name_size);
                                    su_mem_safe_copy(layout.name, layout_name_size, 0, layout_name_str, layout_name_size, 0, layout_name_size);
                                }

                                buffer.size_byte_internal += su_SIZE_OF_TYPE[layout.type];
                                buffer.layout_array[l] = layout;

                                su_lua_pop(lua, 1); // pop array entry
                            }
                        }

                        su_lua_pop(lua, 1); // pop layout array
                    }

                    cfg_out->instance_data.buffer_array[b] = buffer;

                    su_lua_pop(lua, 1); // pop buffer entry
                }
            }

            su_lua_pop(lua, 1); // pop buffers array
        }

        su_lua_pop(lua, 1); // pop instances
    }

    su_lua_pop(lua, 1); // pop batch
}

void su__load_renderer_bound(su_LuaState* lua, struct su_RendererConfig* cfg_out)
{
    if (su_lua_push_field_table(lua, "bound")) {
        cfg_out->bound.index_cfg.capacity = su_lua_get_uint64(lua, "index_capacity");
        cfg_out->bound.instance_cfg.capacity = su_lua_get_uint64(lua, "instance_capacity");
        su_lua_pop(lua, 1);
    }
}

void su__load_renderer_draw(su_LuaState* lua, struct su_RendererConfig* cfg_out)
{
    if (su_lua_push_field_table(lua, "draw")) {
        cfg_out->draw = (struct su_RendererCfgDraw){
            .primitive = su_CAST_M(enum su_RendererPrimitives)(su_lua_get_enum(lua, "primitives")),
            .cull_mode = su_CAST_M(enum su_RendererCullMode)(su_lua_get_enum(lua, "cull_mode")),
            .front_face = su_CAST_M(enum su_RendererFrontFace)(su_lua_get_enum(lua, "front_face")),
        };
        su_lua_pop(lua, 1);
    }
}

void su__load_renderer_pipeline(su_LuaState* lua, struct su_RendererConfig* cfg_out)
{
    if (su_lua_push_field_table(lua, "pipeline")) {
        cfg_out->pipeline.depth_test = su_lua_get_bool(lua, "depth_test");
        if (su_lua_push_field_table(lua, "blend")) {
            cfg_out->pipeline.blend.enabled = su_lua_get_bool(lua, "enabled");
            su_lua_pop(lua, 1);
        }
        su_lua_pop(lua, 1);
    }
}
