#define DYLILO_IMPL
#include "dylilo/dylilo.h"

#include "./sb-dependency.h"

#include "saci-utils/su-general.h"
#include "saci-utils/config/su-config-manager.h"
#include "saci-utils/su-log.h"

/* === HELPER === */

struct sb__DependencySymbolTable {
    const char* name;
    void** func_out;
};

SA_INTERNAL void sb__dependencies_load_handles(void);
SA_INTERNAL void sb__cfg_manager_load_symbols(void);
SA_INTERNAL void sb__dependecies_validate(void);
SA_INTERNAL void sb__load_symbols(DyliloHandle handle,
                                  const struct sb__DependencySymbolTable* symbols,
                                  size_t count);

SA_INTERNAL struct {
    struct sb_WindowingApiFuncs windowing_funcs;
    struct sb_RenderApiFuncs render_funcs;
    struct sb_RenderApiLoaderFuncs render_loader_funcs;
    DyliloHandle window_handle;
    DyliloHandle render_handle;
    DyliloHandle render_loader_handle;
} sb__dependency_handler;

SA_INTERNAL struct sb__DependencySymbolTable sb__gl_symbols[] = {
    {"glClearColor", (void**)&sb__dependency_handler.render_funcs.gl.clear_color},
    {"glClear", (void**)&sb__dependency_handler.render_funcs.gl.clear},
    {"glEnable", (void**)&sb__dependency_handler.render_funcs.gl.enable},
    {"glDebugMessageCallback", (void**)&sb__dependency_handler.render_funcs.gl.debug_message_callback},
    {"glGetString", (void**)&sb__dependency_handler.render_funcs.gl.get_string},

    {"glUniform1ui", (void**)&sb__dependency_handler.render_funcs.gl.uniform1ui},
    {"glUniform1i", (void**)&sb__dependency_handler.render_funcs.gl.uniform1i},
    {"glUniform2f", (void**)&sb__dependency_handler.render_funcs.gl.uniform2f},
    {"glUniform3f", (void**)&sb__dependency_handler.render_funcs.gl.uniform3f},
    {"glUniform4f", (void**)&sb__dependency_handler.render_funcs.gl.uniform4f},
    {"glUniformMatrix2fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_2fv},
    {"glUniformMatrix3fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_3fv},
    {"glUniformMatrix4fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_4fv},
    {"glUniformMatrix2x3fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_2x3fv},
    {"glUniformMatrix2x4fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_2x4fv},
    {"glUniformMatrix3x2fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_3x2fv},
    {"glUniformMatrix3x4fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_3x4fv},
    {"glUniformMatrix4x2fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_4x2fv},
    {"glUniformMatrix4x3fv", (void**)&sb__dependency_handler.render_funcs.gl.uniform_matrix_4x3fv},

    {"glBindVertexArray", (void**)&sb__dependency_handler.render_funcs.gl.bind_vertex_array},
    {"glBindBuffer", (void**)&sb__dependency_handler.render_funcs.gl.bind_buffer},
    {"glBufferData", (void**)&sb__dependency_handler.render_funcs.gl.buffer_data},
    {"glBufferSubData", (void**)&sb__dependency_handler.render_funcs.gl.buffer_subdata},
    {"glGenBuffers", (void**)&sb__dependency_handler.render_funcs.gl.gen_buffers},
    {"glGenVertexArrays", (void**)&sb__dependency_handler.render_funcs.gl.gen_vertex_arrays},
    {"glVertexAttribPointer", (void**)&sb__dependency_handler.render_funcs.gl.vertex_attrib_pointer},
    {"glVertexAttribDivisor", (void**)&sb__dependency_handler.render_funcs.gl.vertex_attrib_divisor},
    {"glEnableVertexAttribArray", (void**)&sb__dependency_handler.render_funcs.gl.enable_vertex_attrib_array},

    {"glCreateShader", (void**)&sb__dependency_handler.render_funcs.gl.create_shader},
    {"glShaderSource", (void**)&sb__dependency_handler.render_funcs.gl.shader_source},
    {"glCompileShader", (void**)&sb__dependency_handler.render_funcs.gl.compile_shader},
    {"glGetShaderiv", (void**)&sb__dependency_handler.render_funcs.gl.get_shaderiv},
    {"glGetShaderInfoLog", (void**)&sb__dependency_handler.render_funcs.gl.get_shader_info_log},
    {"glDeleteShader", (void**)&sb__dependency_handler.render_funcs.gl.delete_shader},

    {"glGetIntegerv", (void**)&sb__dependency_handler.render_funcs.gl.get_integer_v},

    {"glGetBooleanv", (void**)&sb__dependency_handler.render_funcs.gl.get_boolean_v},
    {"glBlendFunc", (void**)&sb__dependency_handler.render_funcs.gl.blend_func},
    {"glGetError", (void**)&sb__dependency_handler.render_funcs.gl.get_error},

    {"glCreateProgram", (void**)&sb__dependency_handler.render_funcs.gl.create_program},
    {"glUseProgram", (void**)&sb__dependency_handler.render_funcs.gl.use_program},
    {"glAttachShader", (void**)&sb__dependency_handler.render_funcs.gl.attach_shader},
    {"glLinkProgram", (void**)&sb__dependency_handler.render_funcs.gl.link_program},
    {"glGetProgramiv", (void**)&sb__dependency_handler.render_funcs.gl.get_program_iv},
    {"glGetProgramInfoLog", (void**)&sb__dependency_handler.render_funcs.gl.get_program_info_log},
    {"glDetachShader", (void**)&sb__dependency_handler.render_funcs.gl.detach_shader},

    {"glDrawElementsInstanced", (void**)&sb__dependency_handler.render_funcs.gl.draw_elements_instanced},
    {"glDrawElements", (void**)&sb__dependency_handler.render_funcs.gl.draw_elements},

    {"glGetUniformLocation", (void**)&sb__dependency_handler.render_funcs.gl.get_uniform_location},

    {"glGenTextures", (void**)&sb__dependency_handler.render_funcs.gl.gen_textures},
    {"glBindTexture", (void**)&sb__dependency_handler.render_funcs.gl.bind_texture},
    {"glActiveTexture", (void**)&sb__dependency_handler.render_funcs.gl.active_texture},
    {"glTexImage2D", (void**)&sb__dependency_handler.render_funcs.gl.tex_image_2d},
    {"glGetTexLevelParameteriv", (void**)&sb__dependency_handler.render_funcs.gl.get_texlevel_parameter_iv},
    {"glGenerateMipmap", (void**)&sb__dependency_handler.render_funcs.gl.generate_mipmap},
    {"glDeleteTextures", (void**)&sb__dependency_handler.render_funcs.gl.delete_textures},
};

SA_INTERNAL struct sb__DependencySymbolTable sb__glad_symbols[] = {
    {"gladLoadGL", (void**)&sb__dependency_handler.render_loader_funcs.gl.load_opengl},
};

SA_INTERNAL struct sb__DependencySymbolTable sb__glfw_symbols[] = {
    {"glfwInit", (void**)&sb__dependency_handler.windowing_funcs.glfw.init},
    {"glfwWindowHint", (void**)&sb__dependency_handler.windowing_funcs.glfw.set_hint},
    {"glfwCreateWindow", (void**)&sb__dependency_handler.windowing_funcs.glfw.create_window},
    {"glfwGetProcAddress", (void**)&sb__dependency_handler.windowing_funcs.glfw.get_proc},
    {"glfwDestroyWindow", (void**)&sb__dependency_handler.windowing_funcs.glfw.destroy_window},
    {"glfwMakeContextCurrent", (void**)&sb__dependency_handler.windowing_funcs.glfw.make_context_current},
    {"glfwWindowShouldClose", (void**)&sb__dependency_handler.windowing_funcs.glfw.should_close},
    {"glfwSetWindowPosCallback", (void**)&sb__dependency_handler.windowing_funcs.glfw.set_pos_handler},
    {"glfwSetWindowSizeCallback", (void**)&sb__dependency_handler.windowing_funcs.glfw.set_size_handler},
    {"glfwTerminate", (void**)&sb__dependency_handler.windowing_funcs.glfw.terminate},
    {"glfwSwapBuffers", (void**)&sb__dependency_handler.windowing_funcs.glfw.swap_buffers},
    {"glfwPollEvents", (void**)&sb__dependency_handler.windowing_funcs.glfw.poll_events},
    {"glfwWaitEvents", (void**)&sb__dependency_handler.windowing_funcs.glfw.wait_events},
    {"glfwWaitEventsTimeout", (void**)&sb__dependency_handler.windowing_funcs.glfw.wait_events_timeout},
    {"glfwPostEmptyEvent", (void**)&sb__dependency_handler.windowing_funcs.glfw.post_empty_event},
    {"glfwSetCursorPosCallback", (void**)&sb__dependency_handler.windowing_funcs.glfw.set_mouse_pos_handler},
    {"glfwGetKey", (void**)&sb__dependency_handler.windowing_funcs.glfw.is_key_pressed},
};

/* === HEADER IMPL ===  */

SA_API void sb_dependecies_load(void) {
    sb__dependencies_load_handles();
    sb__cfg_manager_load_symbols();
    sb__dependecies_validate();
}

SA_API struct sb_WindowingApiFuncs sb_dependencies_get_windowing_api_funcs(void) {
    return sb__dependency_handler.windowing_funcs;
}

SA_API struct sb_RenderApiFuncs sb_dependencies_get_render_api_funcs(void) {
    return sb__dependency_handler.render_funcs;
}

SA_API struct sb_RenderApiLoaderFuncs sb_dependencies_get_render_loader_api_funcs(void) {
    return sb__dependency_handler.render_loader_funcs;
}

/* === HELPER IMPL === */

SA_INTERNAL void sb__cfg_manager_load_symbols(void) {
    switch (su_cfg_manager_get_renderer_api()) {
    case su_RENDERER_API_OPENGL:
        sb__load_symbols(sb__dependency_handler.render_handle,
                         sb__gl_symbols,
                         su_ARRLEN_M(sb__gl_symbols));
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Symbol table is not prepared for this rendering api");
        exit(1);
    }

    switch (su_cfg_manager_get_renderer_api_loader()) {
    case su_RENDERER_LOADER_GLAD:
        sb__load_symbols(sb__dependency_handler.render_loader_handle,
                         sb__glad_symbols,
                         su_ARRLEN_M(sb__glad_symbols));
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Symbol table is not prepared for this rendering api loader");
        exit(1);
    }

    switch (su_cfg_manager_get_window_api()) {
    case su_WINDOW_API_GLFW:
        sb__load_symbols(sb__dependency_handler.window_handle,
                         sb__glfw_symbols,
                         su_ARRLEN_M(sb__glfw_symbols));
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Symbol table is not prepared for this windowing api");
        exit(1);
    }
}

SA_INTERNAL void sb__load_symbols(DyliloHandle handle,
                                  const struct sb__DependencySymbolTable* symbols,
                                  size_t count) {
    for (size_t i = 0; i < count; i++) {
        *symbols[i].func_out = dylilo_get_symbol(handle, symbols[i].name);
        su_LOG_ASSERTF_M(*symbols[i].func_out, su_LOG_CONTEXT_CORE_CONFIG,
                         "Failed to load symbol: %s", symbols[i].name);
    }
}

SA_INTERNAL void sb__dependencies_load_handles(void) {
    /* Render API */
    switch (su_cfg_manager_get_renderer_api()) {
    case su_RENDERER_API_OPENGL:
        sb__dependency_handler.render_loader_handle = dylilo_load_lib(
            (char*)su_cfg_manager_get_renderer_api_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Invalid renderer api");
        exit(1);
    }

    /* Render API Loader */
    switch (su_cfg_manager_get_renderer_api_loader()) {
    case su_RENDERER_LOADER_GLAD:
        sb__dependency_handler.render_loader_handle = dylilo_load_lib(
            (char*)su_cfg_manager_get_renderer_api_loader_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Invalid api loader");
        exit(1);
    }

    /* Windowing API */
    switch (su_cfg_manager_get_window_api()) {
    case su_WINDOW_API_GLFW:
        sb__dependency_handler.window_handle = dylilo_load_lib(
            (char*)su_cfg_manager_get_window_api_path(),
            DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_CONFIG, "Invalid windowing api");
        exit(1);
    }
}

SA_INTERNAL void sb__dependecies_validate(void) {
    su_LOG_ASSERT_M(sb__dependency_handler.render_handle, su_LOG_CONTEXT_CORE_CONFIG,
                    "Could not load Render API handle");
    su_LOG_ASSERT_M(sb__dependency_handler.render_loader_handle, su_LOG_CONTEXT_CORE_CONFIG,
                    "Could not load Render API Loader handle");
    su_LOG_ASSERT_M(sb__dependency_handler.window_handle, su_LOG_CONTEXT_CORE_CONFIG,
                    "Could not load Windowing API handle");
}
