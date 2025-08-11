#include <saci-backend/sb-config.h>
#include <stdio.h>
#include <string.h>
#define DYLILO_IMPL
#include "dylilo/dylilo.h"

#include "saci-backend/sb-config-manager.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

// Platform-specific default library paths
#ifdef _WIN32 // Windows
#  define sb_DEFAULT_OPENGL_PATH "C:\\Windows\\System32\\opengl32.dll"
#  define sb_DEFAULT_GLFW_PATH "C:\\Program Files\\GLFW\\lib\\glfw3.dll"
#  define sb_DEFAULT_GLAD_PATH "C:\\Program Files\\GLAD\\lib\\glad.dll"
#elif defined(__linux__) // Linux
#  define sb_DEFAULT_OPENGL_PATH "/usr/lib/x86_64-linux-gnu/libGL.so"
#  define sb_DEFAULT_GLFW_PATH "/usr/local/lib/libglfw.so"
#  define sb_DEFAULT_GLAD_PATH "/usr/local/lib/libglad.so"
#elif defined(__APPLE__) // macOS
#  define sb_DEFAULT_OPENGL_PATH "/System/Library/Frameworks/OpenGL.framework/OpenGL"
#  define sb_DEFAULT_GLFW_PATH "/usr/local/lib/libglfw.dylib"
#  define sb_DEFAULT_GLAD_PATH "/usr/local/lib/libglad.dylib"
#else
#  error "Unknown platform. Define paths for this platform."
#endif

/* === Internal === */

SA_INTERNAL void sb__cfg_manager_load_handles(void);
SA_INTERNAL void sb__cfg_manager_load_symbols(void);
SA_INTERNAL void sb__cfg_manager_validate(void);

struct sb_SymbolTable {
    const char* name;
    void** func_out;
};

SA_INTERNAL void sb__load_symbols(DyliloHandle handle,
                                  const struct sb_SymbolTable* symbols,
                                  size_t count);

SA_INTERNAL su_Bool sb__cfg_manager_load_render_api(sb_ConfigState* lua_state);

SA_INTERNAL su_Bool sb__cfg_manager_load_render_loader(sb_ConfigState* lua_state);

SA_INTERNAL su_Bool sb__cfg_manager_load_window_api(sb_ConfigState* lua_state);

/* === Header impl === */

SA_INTERNAL struct sb_ConfigManager sb_cfg_manager = sb_CFG_MANAGER_DEFAULT;

SA_INTERNAL struct sb_SymbolTable sb_gl_symbols[] = {
    {"glClearColor", (void**)&sb_cfg_manager.render_api_funcs.clear_color},
    {"glClear", (void**)&sb_cfg_manager.render_api_funcs.clear},
    {"glEnable", (void**)&sb_cfg_manager.render_api_funcs.enable},
    {"glDebugMessageCallback", (void**)&sb_cfg_manager.render_api_funcs.debug_message_callback},
    {"glGetString", (void**)&sb_cfg_manager.render_api_funcs.get_version_string},

    {"glUniform1ui", (void**)&sb_cfg_manager.render_api_funcs.uniform1ui},
    {"glUniform1i", (void**)&sb_cfg_manager.render_api_funcs.uniform1i},
    {"glUniform2f", (void**)&sb_cfg_manager.render_api_funcs.uniform2f},
    {"glUniform3f", (void**)&sb_cfg_manager.render_api_funcs.uniform3f},
    {"glUniform4f", (void**)&sb_cfg_manager.render_api_funcs.uniform4f},
    {"glUniformMatrix2fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_2fv},
    {"glUniformMatrix3fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_3fv},
    {"glUniformMatrix4fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_4fv},
    {"glUniformMatrix2x3fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_2x3fv},
    {"glUniformMatrix2x4fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_2x4fv},
    {"glUniformMatrix3x2fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_3x2fv},
    {"glUniformMatrix3x4fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_3x4fv},
    {"glUniformMatrix4x2fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_4x2fv},
    {"glUniformMatrix4x3fv", (void**)&sb_cfg_manager.render_api_funcs.uniform_matrix_4x3fv},

    {"glBindVertexArray", (void**)&sb_cfg_manager.render_api_funcs.bind_vertex_array},
    {"glBindBuffer", (void**)&sb_cfg_manager.render_api_funcs.bind_buffer},
    {"glBufferData", (void**)&sb_cfg_manager.render_api_funcs.buffer_data},
    {"glBufferSubData", (void**)&sb_cfg_manager.render_api_funcs.buffer_subdata},
    {"glGenBuffers", (void**)&sb_cfg_manager.render_api_funcs.gen_buffers},
    {"glGenVertexArrays", (void**)&sb_cfg_manager.render_api_funcs.gen_vertex_arrays},
    {"glVertexAttribPointer", (void**)&sb_cfg_manager.render_api_funcs.vertex_attrib_pointer},
    {"glVertexAttribDivisor", (void**)&sb_cfg_manager.render_api_funcs.vertex_attrib_divisor},
    {"glEnableVertexAttribArray", (void**)&sb_cfg_manager.render_api_funcs.enable_vertex_attrib_array},

    {"glCreateShader", (void**)&sb_cfg_manager.render_api_funcs.create_shader},
    {"glShaderSource", (void**)&sb_cfg_manager.render_api_funcs.shader_source},
    {"glCompileShader", (void**)&sb_cfg_manager.render_api_funcs.compile_shader},
    {"glGetShaderiv", (void**)&sb_cfg_manager.render_api_funcs.get_shaderiv},
    {"glGetShaderInfoLog", (void**)&sb_cfg_manager.render_api_funcs.get_shader_info_log},
    {"glDeleteShader", (void**)&sb_cfg_manager.render_api_funcs.delete_shader},

    {"glCreateProgram", (void**)&sb_cfg_manager.render_api_funcs.create_program},
    {"glUseProgram", (void**)&sb_cfg_manager.render_api_funcs.use_program},
    {"glAttachShader", (void**)&sb_cfg_manager.render_api_funcs.attach_shader},
    {"glLinkProgram", (void**)&sb_cfg_manager.render_api_funcs.link_program},
    {"glGetProgramiv", (void**)&sb_cfg_manager.render_api_funcs.get_programiv},
    {"glGetProgramInfoLog", (void**)&sb_cfg_manager.render_api_funcs.get_program_info_log},
    {"glDetachShader", (void**)&sb_cfg_manager.render_api_funcs.detach_shader},

    {"glDrawElementsInstanced", (void**)&sb_cfg_manager.render_api_funcs.draw_elements_instanced},
    {"glDrawElements", (void**)&sb_cfg_manager.render_api_funcs.draw_elements},

    {"glGetUniformLocation", (void**)&sb_cfg_manager.render_api_funcs.get_uniform_location},

    {"glGenTextures", (void**)&sb_cfg_manager.render_api_funcs.gen_textures},
    {"glBindTexture", (void**)&sb_cfg_manager.render_api_funcs.bind_texture},
    {"glActiveTexture", (void**)&sb_cfg_manager.render_api_funcs.active_texture},
    {"glTexImage2D", (void**)&sb_cfg_manager.render_api_funcs.tex_image_2d},
    {"glGetTexLevelParameteriv", (void**)&sb_cfg_manager.render_api_funcs.get_texlevel_parameter_iv},
    {"glGenerateMipmap", (void**)&sb_cfg_manager.render_api_funcs.generate_mipmap},
    {"glDeleteTextures", (void**)&sb_cfg_manager.render_api_funcs.delete_textures},
};

SA_INTERNAL struct sb_SymbolTable sb_glad_symbols[] = {
    {"gladLoadGL", (void**)&sb_cfg_manager.render_api_loader_funcs.load_opengl},
};

SA_INTERNAL struct sb_SymbolTable sb_glfw_symbols[] = {
    {"glfwInit", (void**)&sb_cfg_manager.window_funcs.init},
    {"glfwWindowHint", (void**)&sb_cfg_manager.window_funcs.set_hint},
    {"glfwCreateWindow", (void**)&sb_cfg_manager.window_funcs.create_window},
    {"glfwGetProcAddress", (void**)&sb_cfg_manager.window_funcs.get_proc},
    {"glfwDestroyWindow", (void**)&sb_cfg_manager.window_funcs.destroy_window},
    {"glfwMakeContextCurrent", (void**)&sb_cfg_manager.window_funcs.make_context_current},
    {"glfwWindowShouldClose", (void**)&sb_cfg_manager.window_funcs.should_close},
    {"glfwSetWindowPosCallback", (void**)&sb_cfg_manager.window_funcs.set_pos_handler},
    {"glfwSetWindowSizeCallback", (void**)&sb_cfg_manager.window_funcs.set_size_handler},
    {"glfwTerminate", (void**)&sb_cfg_manager.window_funcs.terminate},
    {"glfwSwapBuffers", (void**)&sb_cfg_manager.window_funcs.swap_buffers},
    {"glfwPollEvents", (void**)&sb_cfg_manager.window_funcs.poll_events},
    {"glfwWaitEvents", (void**)&sb_cfg_manager.window_funcs.wait_events},
    {"glfwWaitEventsTimeout", (void**)&sb_cfg_manager.window_funcs.wait_events_timeout},
    {"glfwPostEmptyEvent", (void**)&sb_cfg_manager.window_funcs.post_empty_event},
    {"glfwSetCursorPosCallback", (void**)&sb_cfg_manager.window_funcs.set_mouse_pos_handler},
    {"glfwGetKey", (void**)&sb_cfg_manager.window_funcs.is_key_pressed},
};

void sb_cfg_manager_load_default(void) {
    sb_cfg_manager = sb_CFG_MANAGER_DEFAULT;
}

void sb_cfg_manager_set(const struct sb_ConfigManager cfg_manager) {
    sb_cfg_manager = cfg_manager;
}

su_Bool sb_cfg_manager_fetch(const char* path) {
    sb_ConfigState* lua_state = sb_config_load(path);
    if (!lua_state) {
        su_LOG_WARN_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_CONFIG, "Could not load config");
        return false;
    }

    if (!sb_config_push_global_table(lua_state, "Saci_base")) {
        sb_config_close(lua_state);
        return false;
    }

    if (!sb__cfg_manager_load_render_api(lua_state)) {
        sb_config_pop(lua_state, 1);
        sb_config_close(lua_state);
        return false;
    }

    if (!sb__cfg_manager_load_render_loader(lua_state)) {
        sb_config_pop(lua_state, 1);
        sb_config_close(lua_state);
        return false;
    }

    if (!sb__cfg_manager_load_window_api(lua_state)) {
        sb_config_pop(lua_state, 1);
        sb_config_close(lua_state);
        return false;
    }

    sb_config_pop(lua_state, 1); // pop Saci_base table
    sb_config_close(lua_state);

    return true;
}

void sb_cfg_manager_load_dependencies(void) {
    sb__cfg_manager_load_handles();
    sb__cfg_manager_validate();
    sb__cfg_manager_load_symbols();
}

enum sb_RendererApi sb_cfg_manager_get_renderer_api(void) {
    return sb_cfg_manager.render_api_data.api;
}

struct sb_WindowingApiFuncs sb_cfg_manager_get_window_funcs(void) {
    return sb_cfg_manager.window_funcs;
}

SA_API struct sb_RenderApiLoaderFuncs sb_cfg_manager_get_loader_funcs(void) {
    return sb_cfg_manager.render_api_loader_funcs;
}

SA_API struct sb_RenderApiFuncs sb_cfg_manager_get_render_funcs(void) {
    return sb_cfg_manager.render_api_funcs;
}

/* === Internal Impl === */

SA_INTERNAL enum sb_RendererApi sb__parse_renderer_api(const char* api_str) {
    if (!api_str) {
        exit(1);
    }
    if (strcmp(api_str, "OpenGL") == 0) {
        return sb_RENDERER_API_OPENGL;
    }
    if (strcmp(api_str, "Vulkan") == 0) {
        return sb_RENDERER_API_VULKAN;
    }
    exit(1);
}

SA_INTERNAL enum sb_RenderApiLoader sb__parse_render_loader(const char* loader_str) {
    if (!loader_str) {
        exit(1);
    }
    if (strcmp(loader_str, "GLAD") == 0)
        return sb_RENDERER_LOADER_GLAD;
    exit(1);
}

SA_INTERNAL enum sb_WindowApi sb__parse_window_api(const char* api_str) {
    if (!api_str) {
        exit(1);
    }
    if (strcmp(api_str, "GLFW") == 0) {
        return sb_WINDOW_API_GLFW;
    }
    exit(1);
}

SA_INTERNAL su_Bool sb__cfg_manager_load_render_api(sb_ConfigState* lua_state) {
    if (!sb_config_push_field_table(lua_state, "render_api")) {
        return false;
    }

    const char* api_str = sb_config_get_str(lua_state, "api");
    const char* path_str = sb_config_get_str(lua_state, "path");

    sb_cfg_manager.render_api_data.api = sb__parse_renderer_api(api_str);

    sb_cfg_manager.render_api_data.path_to_api = path_str ? strdup(path_str) : NULL;

    sb_config_pop(lua_state, 1); // pop render_api table
    return true;
}

SA_INTERNAL su_Bool sb__cfg_manager_load_render_loader(sb_ConfigState* lua_state) {
    if (!sb_config_push_field_table(lua_state, "render_loader")) {
        return false;
    }

    const char* api_str = sb_config_get_str(lua_state, "api");
    const char* path_str = sb_config_get_str(lua_state, "path");

    sb_cfg_manager.render_api_loader_data.api_loader = sb__parse_render_loader(api_str);

    free(sb_cfg_manager.render_api_loader_data.path_to_api);
    sb_cfg_manager.render_api_loader_data.path_to_api = path_str ? strdup(path_str) : NULL;

    sb_config_pop(lua_state, 1); // pop render_loader table
    return true;
}

SA_INTERNAL su_Bool sb__cfg_manager_load_window_api(sb_ConfigState* lua_state) {
    if (!sb_config_push_field_table(lua_state, "window_api")) {
        return false;
    }

    const char* api_str = sb_config_get_str(lua_state, "api");
    const char* path_str = sb_config_get_str(lua_state, "path");

    sb_cfg_manager.windowing_api_data.api = sb__parse_window_api(api_str);

    free(sb_cfg_manager.windowing_api_data.path_to_api);
    sb_cfg_manager.windowing_api_data.path_to_api = path_str ? strdup(path_str) : NULL;

    sb_config_pop(lua_state, 1); // pop window_api table
    return true;
}

SA_INTERNAL void sb__cfg_manager_load_handles(void) {
    /* Render API */
    switch (sb_cfg_manager.render_api_data.api) {
    case sb_RENDERER_API_OPENGL:
        sb_cfg_manager.render_api_data.handle = dylilo_load_lib(
            sb_cfg_manager.render_api_data.path_to_api
                ? sb_cfg_manager.render_api_data.path_to_api
                : sb_DEFAULT_OPENGL_PATH,
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid renderer API");
        exit(1);
    }

    /* Render API Loader */
    switch (sb_cfg_manager.render_api_loader_data.api_loader) {
    case sb_RENDERER_LOADER_GLAD:
        sb_cfg_manager.render_api_loader_data.handle = dylilo_load_lib(
            sb_cfg_manager.render_api_loader_data.path_to_api
                ? sb_cfg_manager.render_api_loader_data.path_to_api
                : sb_DEFAULT_GLAD_PATH,
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid render API loader");
        exit(1);
    }

    /* Windowing API */
    switch (sb_cfg_manager.windowing_api_data.api) {
    case sb_WINDOW_API_GLFW:
        sb_cfg_manager.windowing_api_data.handle = dylilo_load_lib(
            sb_cfg_manager.windowing_api_data.path_to_api
                ? sb_cfg_manager.windowing_api_data.path_to_api
                : sb_DEFAULT_GLFW_PATH,
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid windowing API");
        exit(1);
    }
}

SA_INTERNAL void sb__cfg_manager_load_symbols(void) {
    switch (sb_cfg_manager.render_api_data.api) {
    case sb_RENDERER_API_OPENGL:
        sb__load_symbols(sb_cfg_manager.render_api_data.handle,
                         sb_gl_symbols,
                         su_ARRLEN(sb_gl_symbols));
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Unsupported Render API");
        exit(1);
    }

    switch (sb_cfg_manager.render_api_loader_data.api_loader) {
    case sb_RENDERER_LOADER_GLAD:
        sb__load_symbols(sb_cfg_manager.render_api_loader_data.handle,
                         sb_glad_symbols,
                         su_ARRLEN(sb_glad_symbols));
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Unsupported Render API");
        exit(1);
    }

    switch (sb_cfg_manager.windowing_api_data.api) {
    case sb_WINDOW_API_GLFW:
        sb__load_symbols(sb_cfg_manager.windowing_api_data.handle,
                         sb_glfw_symbols,
                         su_ARRLEN(sb_glfw_symbols));
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Unsupported Render API");
        exit(1);
    }
}

SA_INTERNAL void sb__cfg_manager_validate(void) {
    su_LOG_ASSERT_MESSAGE_M(sb_cfg_manager.render_api_data.handle,
                            "Could not load Render API handle");
    su_LOG_ASSERT_MESSAGE_M(sb_cfg_manager.render_api_loader_data.handle,
                            "Could not load Render API Loader handle");
    su_LOG_ASSERT_MESSAGE_M(sb_cfg_manager.windowing_api_data.handle,
                            "Could not load Windowing API handle");
}

SA_INTERNAL void sb__load_symbols(DyliloHandle handle,
                                  const struct sb_SymbolTable* symbols,
                                  size_t count) {
    for (size_t i = 0; i < count; i++) {
        *symbols[i].func_out = dylilo_get_symbol(handle, symbols[i].name);
        su_LOG_ASSERTF_MESSAGE_M(*symbols[i].func_out,
                                 "Failed to load symbol: %s", symbols[i].name);
    }
}
