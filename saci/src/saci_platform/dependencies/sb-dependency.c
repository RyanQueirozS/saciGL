#include <stdio.h>
#define DYLILO_IMPL
#include "dylilo/dylilo.h"

#include "saci_platform/dependencies/dependency.h"
#include "./sb-dependency-internal.h"

#include "saci_util/defines.h"
#include "saci_util/log.h"

/* === HELPER === */

struct PSaciDependencySymbolTable {
    const char* name;
    void** func_out;
};

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
    DyliloHandle window_handle;
    DyliloHandle render_handle;
    DyliloHandle render_loader_handle;
} psaci__dependency_handler;

SACI_INTERNAL struct PSaciDependencySymbolTable psaci__gl_symbols[] = {
    {"glClearColor", (void**)&psaci__dependency_handler.render_funcs.gl.clear_color},
    {"glClear", (void**)&psaci__dependency_handler.render_funcs.gl.clear},
    {"glEnable", (void**)&psaci__dependency_handler.render_funcs.gl.enable},
    {"glDebugMessageCallback", (void**)&psaci__dependency_handler.render_funcs.gl.debug_message_callback},
    {"glGetString", (void**)&psaci__dependency_handler.render_funcs.gl.get_string},

    {"glUniform1ui", (void**)&psaci__dependency_handler.render_funcs.gl.uniform1ui},
    {"glUniform1i", (void**)&psaci__dependency_handler.render_funcs.gl.uniform1i},
    {"glUniform2f", (void**)&psaci__dependency_handler.render_funcs.gl.uniform2f},
    {"glUniform3f", (void**)&psaci__dependency_handler.render_funcs.gl.uniform3f},
    {"glUniform4f", (void**)&psaci__dependency_handler.render_funcs.gl.uniform4f},
    {"glUniformMatrix2fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_2fv},
    {"glUniformMatrix3fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_3fv},
    {"glUniformMatrix4fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_4fv},
    {"glUniformMatrix2x3fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_2x3fv},
    {"glUniformMatrix2x4fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_2x4fv},
    {"glUniformMatrix3x2fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_3x2fv},
    {"glUniformMatrix3x4fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_3x4fv},
    {"glUniformMatrix4x2fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_4x2fv},
    {"glUniformMatrix4x3fv", (void**)&psaci__dependency_handler.render_funcs.gl.uniform_matrix_4x3fv},

    {"glBindVertexArray", (void**)&psaci__dependency_handler.render_funcs.gl.bind_vertex_array},
    {"glBindBuffer", (void**)&psaci__dependency_handler.render_funcs.gl.bind_buffer},
    {"glBufferData", (void**)&psaci__dependency_handler.render_funcs.gl.buffer_data},
    {"glBufferSubData", (void**)&psaci__dependency_handler.render_funcs.gl.buffer_subdata},
    {"glGenBuffers", (void**)&psaci__dependency_handler.render_funcs.gl.gen_buffers},
    {"glGenVertexArrays", (void**)&psaci__dependency_handler.render_funcs.gl.gen_vertex_arrays},
    {"glVertexAttribPointer", (void**)&psaci__dependency_handler.render_funcs.gl.vertex_attrib_pointer},
    {"glVertexAttribDivisor", (void**)&psaci__dependency_handler.render_funcs.gl.vertex_attrib_divisor},
    {"glEnableVertexAttribArray", (void**)&psaci__dependency_handler.render_funcs.gl.enable_vertex_attrib_array},

    {"glCreateShader", (void**)&psaci__dependency_handler.render_funcs.gl.create_shader},
    {"glShaderSource", (void**)&psaci__dependency_handler.render_funcs.gl.shader_source},
    {"glCompileShader", (void**)&psaci__dependency_handler.render_funcs.gl.compile_shader},
    {"glGetShaderiv", (void**)&psaci__dependency_handler.render_funcs.gl.get_shaderiv},
    {"glGetShaderInfoLog", (void**)&psaci__dependency_handler.render_funcs.gl.get_shader_info_log},
    {"glDeleteShader", (void**)&psaci__dependency_handler.render_funcs.gl.delete_shader},

    {"glGetIntegerv", (void**)&psaci__dependency_handler.render_funcs.gl.get_integer_v},

    {"glGetBooleanv", (void**)&psaci__dependency_handler.render_funcs.gl.get_boolean_v},
    {"glBlendFunc", (void**)&psaci__dependency_handler.render_funcs.gl.blend_func},
    {"glGetError", (void**)&psaci__dependency_handler.render_funcs.gl.get_error},

    {"glCreateProgram", (void**)&psaci__dependency_handler.render_funcs.gl.create_program},
    {"glUseProgram", (void**)&psaci__dependency_handler.render_funcs.gl.use_program},
    {"glAttachShader", (void**)&psaci__dependency_handler.render_funcs.gl.attach_shader},
    {"glLinkProgram", (void**)&psaci__dependency_handler.render_funcs.gl.link_program},
    {"glGetProgramiv", (void**)&psaci__dependency_handler.render_funcs.gl.get_program_iv},
    {"glGetProgramInfoLog", (void**)&psaci__dependency_handler.render_funcs.gl.get_program_info_log},
    {"glDetachShader", (void**)&psaci__dependency_handler.render_funcs.gl.detach_shader},

    {"glDrawElementsInstanced", (void**)&psaci__dependency_handler.render_funcs.gl.draw_elements_instanced},
    {"glDrawElements", (void**)&psaci__dependency_handler.render_funcs.gl.draw_elements},

    {"glGetUniformLocation", (void**)&psaci__dependency_handler.render_funcs.gl.get_uniform_location},

    {"glGenTextures", (void**)&psaci__dependency_handler.render_funcs.gl.gen_textures},
    {"glBindTexture", (void**)&psaci__dependency_handler.render_funcs.gl.bind_texture},
    {"glActiveTexture", (void**)&psaci__dependency_handler.render_funcs.gl.active_texture},
    {"glTexImage2D", (void**)&psaci__dependency_handler.render_funcs.gl.tex_image_2d},
    {"glGetTexLevelParameteriv", (void**)&psaci__dependency_handler.render_funcs.gl.get_texlevel_parameter_iv},
    {"glGenerateMipmap", (void**)&psaci__dependency_handler.render_funcs.gl.generate_mipmap},
    {"glDeleteTextures", (void**)&psaci__dependency_handler.render_funcs.gl.delete_textures},
};

SACI_INTERNAL struct PSaciDependencySymbolTable sb__glad_symbols[] = {
    {"gladLoadGL", (void**)&psaci__dependency_handler.render_loader_funcs.gl.load_opengl},
};

SACI_INTERNAL struct PSaciDependencySymbolTable sb__glfw_symbols[] = {
    {"glfwInit", (void**)&psaci__dependency_handler.windowing_funcs.glfw.init},
    {"glfwWindowHint", (void**)&psaci__dependency_handler.windowing_funcs.glfw.set_hint},
    {"glfwCreateWindow", (void**)&psaci__dependency_handler.windowing_funcs.glfw.create_window},
    {"glfwGetProcAddress", (void**)&psaci__dependency_handler.windowing_funcs.glfw.get_proc},
    {"glfwDestroyWindow", (void**)&psaci__dependency_handler.windowing_funcs.glfw.destroy_window},
    {"glfwMakeContextCurrent", (void**)&psaci__dependency_handler.windowing_funcs.glfw.make_context_current},
    {"glfwWindowShouldClose", (void**)&psaci__dependency_handler.windowing_funcs.glfw.should_close},
    {"glfwSetWindowPosCallback", (void**)&psaci__dependency_handler.windowing_funcs.glfw.set_pos_handler},
    {"glfwSetWindowSizeCallback", (void**)&psaci__dependency_handler.windowing_funcs.glfw.set_size_handler},
    {"glfwTerminate", (void**)&psaci__dependency_handler.windowing_funcs.glfw.terminate},
    {"glfwSwapBuffers", (void**)&psaci__dependency_handler.windowing_funcs.glfw.swap_buffers},
    {"glfwPollEvents", (void**)&psaci__dependency_handler.windowing_funcs.glfw.poll_events},
    {"glfwWaitEvents", (void**)&psaci__dependency_handler.windowing_funcs.glfw.wait_events},
    {"glfwWaitEventsTimeout", (void**)&psaci__dependency_handler.windowing_funcs.glfw.wait_events_timeout},
    {"glfwPostEmptyEvent", (void**)&psaci__dependency_handler.windowing_funcs.glfw.post_empty_event},
    {"glfwSetCursorPosCallback", (void**)&psaci__dependency_handler.windowing_funcs.glfw.set_mouse_pos_handler},
    {"glfwGetKey", (void**)&psaci__dependency_handler.windowing_funcs.glfw.is_key_pressed},
};

/* === HEADER IMPL ===  */

SACI_API void sb_dependecies_load(void)
{
    psaci__dependencies_load_handles();
    psaci__dependecies_validate();
    psaci__dependencies_load_symbols();
}

SACI_API struct PSaciWindowingApiFuncs psaci_dependencies_get_windowing_api_funcs(void)
{
    return psaci__dependency_handler.windowing_funcs;
}

SACI_API struct PSaciRenderApiFuncs psaci_dependencies_get_render_api_funcs(void)
{
    return psaci__dependency_handler.render_funcs;
}

SACI_API struct PSaciRenderApiLoaderFuncs sb_dependencies_get_render_loader_api_funcs(void)
{
    return psaci__dependency_handler.render_loader_funcs;
}

/* === HELPER IMPL === */

SACI_INTERNAL void sb__dependencies_load_symbols(void)
{
    switch (su_cfg_manager_get_renderer_api()) {
    case su_RENDERER_API_OPENGL:
        sb__load_symbols(psaci__dependency_handler.render_handle,
                         sb__gl_symbols,
                         su_ARRLEN_M(sb__gl_symbols));
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Symbol table is not prepared for this rendering api");
        exit(1);
    }

    switch (su_cfg_manager_get_renderer_api_loader()) {
    case su_RENDERER_LOADER_GLAD:
        sb__load_symbols(psaci__dependency_handler.render_loader_handle,
                         sb__glad_symbols,
                         su_ARRLEN_M(sb__glad_symbols));
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Symbol table is not prepared for this rendering api loader");
        exit(1);
    }

    switch (su_cfg_manager_get_window_api()) {
    case su_WINDOW_API_GLFW:
        sb__load_symbols(psaci__dependency_handler.window_handle,
                         sb__glfw_symbols,
                         su_ARRLEN_M(sb__glfw_symbols));
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Symbol table is not prepared for this windowing api");
        exit(1);
    }
}

SACI_INTERNAL void sb__load_symbols(DyliloHandle handle,
                                    const struct sb__DependencySymbolTable* symbols,
                                    size_t count)
{
    for (size_t i = 0; i < count; i++) {
        *symbols[i].func_out = dylilo_get_symbol(handle, symbols[i].name);
        su_LOG_ASSERTF_M(*symbols[i].func_out, su_LOG_CONTEXT_CORE_CONFIG,
                         "Failed to load symbol: %s", symbols[i].name);
    }
}

SACI_INTERNAL void sb__dependencies_load_handles(void)
{
    /* Render API */
    switch (su_cfg_manager_get_renderer_api()) {
    case su_RENDERER_API_OPENGL:
        psaci__dependency_handler.render_handle = dylilo_load_lib(
            (char*)su_cfg_manager_get_renderer_api_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_CORE_CONFIG, "Invalid renderer api");
    }

    /* Render API Loader */
    switch (su_cfg_manager_get_renderer_api_loader()) {
    case su_RENDERER_LOADER_GLAD:
        psaci__dependency_handler.render_loader_handle = dylilo_load_lib(
            (char*)su_cfg_manager_get_renderer_api_loader_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_CORE_CONFIG, "Invalid api loader");
    }

    /* Windowing API */
    switch (su_cfg_manager_get_window_api()) {
    case su_WINDOW_API_GLFW:
        psaci__dependency_handler.window_handle = dylilo_load_lib(
            (char*)su_cfg_manager_get_window_api_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_CORE_CONFIG, "Invalid windowing api");
    }
}

SACI_INTERNAL void sb__dependecies_validate(void)
{
    su_LOG_ASSERT_M(psaci__dependency_handler.render_handle, su_LOG_CONTEXT_CORE_CONFIG,
                    "Could not load Render API handle");
    su_LOG_ASSERT_M(psaci__dependency_handler.render_loader_handle, su_LOG_CONTEXT_CORE_CONFIG,
                    "Could not load Render API Loader handle");
    su_LOG_ASSERT_M(psaci__dependency_handler.window_handle, su_LOG_CONTEXT_CORE_CONFIG,
                    "Could not load Windowing API handle");
}
