#include "saci_platform/dependencies/dependency.h"

#include <stdio.h>
#define DYLILO_IMPL
#include <dylilo/dylilo.h>

#include "saci_platform/dependencies/internal/dependency.h"

#include "saci_util/defines.h"
#include "saci_util/internal/general.h"
#include "saci_util/internal/log.h"
#include "saci_util/log.h"

/* === HELPER === */

struct PSaciDependencySymbolTable {
    const char* name;
    void** func_out;
};

SACI_INTERNAL const char* psaci__dependencies_get_renderer_api_path(void);
SACI_INTERNAL const char* psaci__dependencies_get_renderer_loader_path(void);
SACI_INTERNAL const char* psaci__dependencies_get_windowing_api_path(void);

SACI_INTERNAL void psaci__dependencies_load_handles(void);
SACI_INTERNAL void psaci__dependencies_load_symbols(void);
SACI_INTERNAL void psaci__dependecies_validate(void);
SACI_INTERNAL void psaci__load_symbols(DyliloHandle handle,
                                       const struct PSaciDependencySymbolTable* symbols,
                                       size_t count);

SACI_INTERNAL struct {
    struct PSaciWindowingApiFuncs windowing_funcs;
    struct PSaciRenderApiFuncs render_funcs;
    struct PSaciRenderApiLoaderFuncs render_loader_funcs;

    enum PSaciWindowApi windowing_api;
    enum PSaciRenderApi render_api;
    enum PSaciRenderApiLoader render_api_loader;
    const char* windowing_path;
    const char* render_path;
    const char* render_loader_path;

    DyliloHandle window_handle;
    DyliloHandle render_handle;
    DyliloHandle render_loader_handle;
} psaci_g_dependency_handler;

SACI_INTERNAL struct PSaciDependencySymbolTable psaci_g_gl_symbols[] = {
    {"glClearColor", (void**)&psaci_g_dependency_handler.render_funcs.gl.clear_color},
    {"glClear", (void**)&psaci_g_dependency_handler.render_funcs.gl.clear},
    {"glEnable", (void**)&psaci_g_dependency_handler.render_funcs.gl.enable},
    {"glDebugMessageCallback", (void**)&psaci_g_dependency_handler.render_funcs.gl.debug_message_callback},
    {"glGetString", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_string},

    {"glUniform1ui", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform1ui},
    {"glUniform1i", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform1i},
    {"glUniform2f", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform2f},
    {"glUniform3f", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform3f},
    {"glUniform4f", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform4f},
    {"glUniformMatrix2fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_2fv},
    {"glUniformMatrix3fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_3fv},
    {"glUniformMatrix4fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_4fv},
    {"glUniformMatrix2x3fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_2x3fv},
    {"glUniformMatrix2x4fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_2x4fv},
    {"glUniformMatrix3x2fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_3x2fv},
    {"glUniformMatrix3x4fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_3x4fv},
    {"glUniformMatrix4x2fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_4x2fv},
    {"glUniformMatrix4x3fv", (void**)&psaci_g_dependency_handler.render_funcs.gl.uniform_matrix_4x3fv},

    {"glBindVertexArray", (void**)&psaci_g_dependency_handler.render_funcs.gl.bind_vertex_array},
    {"glBindBuffer", (void**)&psaci_g_dependency_handler.render_funcs.gl.bind_buffer},
    {"glBufferData", (void**)&psaci_g_dependency_handler.render_funcs.gl.buffer_data},
    {"glBufferSubData", (void**)&psaci_g_dependency_handler.render_funcs.gl.buffer_subdata},
    {"glGenBuffers", (void**)&psaci_g_dependency_handler.render_funcs.gl.gen_buffers},
    {"glGenVertexArrays", (void**)&psaci_g_dependency_handler.render_funcs.gl.gen_vertex_arrays},
    {"glVertexAttribPointer", (void**)&psaci_g_dependency_handler.render_funcs.gl.vertex_attrib_pointer},
    {"glVertexAttribDivisor", (void**)&psaci_g_dependency_handler.render_funcs.gl.vertex_attrib_divisor},
    {"glEnableVertexAttribArray", (void**)&psaci_g_dependency_handler.render_funcs.gl.enable_vertex_attrib_array},

    {"glCreateShader", (void**)&psaci_g_dependency_handler.render_funcs.gl.create_shader},
    {"glShaderSource", (void**)&psaci_g_dependency_handler.render_funcs.gl.shader_source},
    {"glCompileShader", (void**)&psaci_g_dependency_handler.render_funcs.gl.compile_shader},
    {"glGetShaderiv", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_shaderiv},
    {"glGetShaderInfoLog", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_shader_info_log},
    {"glDeleteShader", (void**)&psaci_g_dependency_handler.render_funcs.gl.delete_shader},

    {"glGetIntegerv", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_integer_v},

    {"glGetBooleanv", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_boolean_v},
    {"glBlendFunc", (void**)&psaci_g_dependency_handler.render_funcs.gl.blend_func},
    {"glGetError", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_error},

    {"glCreateProgram", (void**)&psaci_g_dependency_handler.render_funcs.gl.create_program},
    {"glUseProgram", (void**)&psaci_g_dependency_handler.render_funcs.gl.use_program},
    {"glAttachShader", (void**)&psaci_g_dependency_handler.render_funcs.gl.attach_shader},
    {"glLinkProgram", (void**)&psaci_g_dependency_handler.render_funcs.gl.link_program},
    {"glGetProgramiv", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_program_iv},
    {"glGetProgramInfoLog", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_program_info_log},
    {"glDetachShader", (void**)&psaci_g_dependency_handler.render_funcs.gl.detach_shader},

    {"glDrawElementsInstanced", (void**)&psaci_g_dependency_handler.render_funcs.gl.draw_elements_instanced},
    {"glDrawElements", (void**)&psaci_g_dependency_handler.render_funcs.gl.draw_elements},

    {"glGetUniformLocation", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_uniform_location},

    {"glGenTextures", (void**)&psaci_g_dependency_handler.render_funcs.gl.gen_textures},
    {"glBindTexture", (void**)&psaci_g_dependency_handler.render_funcs.gl.bind_texture},
    {"glActiveTexture", (void**)&psaci_g_dependency_handler.render_funcs.gl.active_texture},
    {"glTexImage2D", (void**)&psaci_g_dependency_handler.render_funcs.gl.tex_image_2d},
    {"glGetTexLevelParameteriv", (void**)&psaci_g_dependency_handler.render_funcs.gl.get_texlevel_parameter_iv},
    {"glGenerateMipmap", (void**)&psaci_g_dependency_handler.render_funcs.gl.generate_mipmap},
    {"glDeleteTextures", (void**)&psaci_g_dependency_handler.render_funcs.gl.delete_textures},
};

SACI_INTERNAL struct PSaciDependencySymbolTable psaci_g_glad_symbols[] = {
    {"gladLoadGL", (void**)&psaci_g_dependency_handler.render_loader_funcs.glad.load_opengl},
};

SACI_INTERNAL struct PSaciDependencySymbolTable psaci_g_glfw_symbols[] = {
    {"glfwInit", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.init},
    {"glfwWindowHint", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.set_hint},
    {"glfwCreateWindow", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.create_window},
    {"glfwGetProcAddress", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.get_proc},
    {"glfwDestroyWindow", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.destroy_window},
    {"glfwMakeContextCurrent", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.make_context_current},
    {"glfwWindowShouldClose", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.should_close},
    {"glfwSetWindowPosCallback", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.set_pos_handler},
    {"glfwSetWindowSizeCallback", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.set_size_handler},
    {"glfwTerminate", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.terminate},
    {"glfwSwapBuffers", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.swap_buffers},
    {"glfwPollEvents", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.poll_events},
    {"glfwWaitEvents", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.wait_events},
    {"glfwWaitEventsTimeout", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.wait_events_timeout},
    {"glfwPostEmptyEvent", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.post_empty_event},
    {"glfwSetCursorPosCallback", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.set_mouse_pos_handler},
    {"glfwGetKey", (void**)&psaci_g_dependency_handler.windowing_funcs.glfw.is_key_pressed},
};

/* === HEADER IMPL ===  */

void psaci_dependencies_load(const struct PSaciDependencyLoaderContext dependency_context)
{
    psaci_g_dependency_handler.windowing_api = dependency_context.windowing_api_data.api;
    psaci_g_dependency_handler.render_api = dependency_context.render_api_data.api;
    psaci_g_dependency_handler.render_api_loader = dependency_context.render_api_loader_data.api_loader;
    psaci__dependencies_load_handles();
    // TODO dupe strings for paths
    psaci__dependecies_validate();
    psaci__dependencies_load_symbols();
}

enum PSaciRenderApiLoader psaci_dependencies_get_render_loader(void)
{
    return psaci_g_dependency_handler.render_api_loader;
}

enum PSaciRenderApi psaci_dependencies_get_render_api(void)
{
    return psaci_g_dependency_handler.render_api;
}

enum PSaciWindowApi psaci_dependencies_get_windowing_api(void)
{
    return psaci_g_dependency_handler.windowing_api;
}

// Internal
struct PSaciWindowingApiFuncs psaci_dependencies_get_windowing_api_funcs(void)
{
    return psaci_g_dependency_handler.windowing_funcs;
}

struct PSaciRenderApiFuncs psaci_dependencies_get_render_api_funcs(void)
{
    return psaci_g_dependency_handler.render_funcs;
}

struct PSaciRenderApiLoaderFuncs psaci_dependencies_get_render_loader_api_funcs(void)
{
    return psaci_g_dependency_handler.render_loader_funcs;
}

/* === HELPER IMPL === */

SACI_INTERNAL void psaci__dependencies_load_symbols(void)
{
    switch (psaci_dependencies_get_render_api()) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci__load_symbols(psaci_g_dependency_handler.render_handle,
                            psaci_g_gl_symbols,
                            SACI_ARRLEN_M(psaci_g_gl_symbols));
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // Loaded at compile time
        break;
    default:
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Symbol table is not prepared for this rendering api");
        exit(1);
    }

    switch (psaci_dependencies_get_render_loader()) {
    case PSACI_RENDERER_LOADER_GLAD:
        psaci__load_symbols(psaci_g_dependency_handler.render_loader_handle,
                            psaci_g_glad_symbols,
                            SACI_ARRLEN_M(psaci_g_glad_symbols));
        break;
    case PSACI_RENDERER_LOADER_EMSCRIPTEN:
        // Loaded at compile time
        break;
    default:
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Symbol table is not prepared for this rendering api loader");
        exit(1);
    }

    switch (psaci_dependencies_get_windowing_api()) {
    case PSACI_WINDOW_API_GLFW:
        psaci__load_symbols(psaci_g_dependency_handler.window_handle,
                            psaci_g_glfw_symbols,
                            SACI_ARRLEN_M(psaci_g_glfw_symbols));
        break;
    case PSACI_WINDOW_API_EMSCRIPTEN:
        // Loaded at compile time
        break;
    default:
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                         SACI_LOG_CONTEXT_CORE_CONFIG,
                         "Symbol table is not prepared for this windowing api");
        exit(1);
    }
}

SACI_INTERNAL const char* psaci__dependencies_get_renderer_api_path(void)
{
    return psaci_g_dependency_handler.render_path;
}

SACI_INTERNAL const char* psaci__dependencies_get_renderer_loader_path(void)
{
    return psaci_g_dependency_handler.render_loader_path;
}

SACI_INTERNAL const char* psaci__dependencies_get_windowing_api_path(void)
{
    return psaci_g_dependency_handler.windowing_path;
}

SACI_INTERNAL void psaci__dependencies_load_handles(void)
{
    /* Render API */
    switch (psaci_dependencies_get_render_api()) {
    case PSACI_RENDERER_API_OPENGL4:
        psaci_g_dependency_handler.render_handle = dylilo_load_lib(
            (char*)psaci__dependencies_get_renderer_api_path(),
            DYLILO_FLAGS_DEFAULT);
        SACI_LOG_INFO_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_DEPENDENCIES,
                        "Renderer API is OPENGL4, loading symbols");
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        SACI_LOG_INFO_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_DEPENDENCIES,
                        "Renderer API set through EMSCRIPTEN, no need to load at runtime");
        break;
    default:
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_CONFIG, "Invalid renderer api");
    }

    /* Render API Loader */
    switch (psaci_dependencies_get_render_loader()) {
    case PSACI_RENDERER_LOADER_GLAD:
        psaci_g_dependency_handler.render_loader_handle = dylilo_load_lib(
            (char*)psaci__dependencies_get_renderer_loader_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_CORE_CONFIG, "Invalid api loader");
    }

    /* Windowing API */
    switch (psaci_dependencies_get_windowing_api()) {
    case PSACI_WINDOW_API_GLFW:
        psaci_g_dependency_handler.window_handle = dylilo_load_lib(
            (char*)psaci__dependencies_get_windowing_api_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_CORE_CONFIG, "Invalid windowing api");
    }
}

SACI_INTERNAL void psaci__load_symbols(DyliloHandle handle,
                                       const struct PSaciDependencySymbolTable* symbols,
                                       size_t count)
{
    for (size_t i = 0; i < count; i++) {
        *symbols[i].func_out = dylilo_get_symbol(handle, symbols[i].name);
        SACI_LOG_ASSERTF_M(*symbols[i].func_out, SACI_LOG_CONTEXT_CORE_CONFIG,
                           "Failed to load symbol: %s", symbols[i].name);
    }
}

SACI_INTERNAL void psaci__dependecies_validate(void)
{
    switch (psaci_dependencies_get_render_api()) {
    case PSACI_RENDERER_API_OPENGL4:
        SACI_LOG_ASSERT_M(psaci_g_dependency_handler.render_funcs.gl.buffer_data,
                          SACI_LOG_CONTEXT_DEPENDENCIES, "Could not load OPENGL4 funcs");
        break;
    case PSACI_RENDERER_API_OPENGLES3:
        // OpenglES3 funcs are compiled and not loaded at runtime.
        break;
    case PSACI_RENDERER_API_VULKAN:
        break;
    }
    switch (psaci_dependencies_get_windowing_api()) {
    case PSACI_WINDOW_API_GLFW:
        SACI_LOG_ASSERT_M(psaci_g_dependency_handler.windowing_funcs.glfw.create_window,
                          SACI_LOG_CONTEXT_DEPENDENCIES, "Could not load GLFW funcs");
        break;
    case PSACI_WINDOW_API_EMSCRIPTEN:
        // Loaded at compile time, no need to check.
        break;
    }
    switch (psaci_dependencies_get_render_loader()) {
    case PSACI_RENDERER_LOADER_GLAD:
        SACI_LOG_ASSERT_M(psaci_g_dependency_handler.render_loader_funcs.glad.load_opengl,
                          SACI_LOG_CONTEXT_DEPENDENCIES, "Could not load GLAD funcs");
    case PSACI_RENDERER_LOADER_EMSCRIPTEN:
        // Loaded at compiletime.
        break;
    }

    SACI_LOG_ASSERT_M(psaci_g_dependency_handler.render_handle, SACI_LOG_CONTEXT_CORE_CONFIG,
                      "Could not load Render API handle");
    SACI_LOG_ASSERT_M(psaci_g_dependency_handler.render_loader_handle, SACI_LOG_CONTEXT_CORE_CONFIG,
                      "Could not load Render API Loader handle");
    SACI_LOG_ASSERT_M(psaci_g_dependency_handler.window_handle, SACI_LOG_CONTEXT_CORE_CONFIG,
                      "Could not load Windowing API handle");
    free(psaci_g_dependency_handler.render_loader_handle); // TODO update dylilo
    free(psaci_g_dependency_handler.render_handle);        // TODO update dylilo
    free(psaci_g_dependency_handler.window_handle);        // TODO update dylilo
}
