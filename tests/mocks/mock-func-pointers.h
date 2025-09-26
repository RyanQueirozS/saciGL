#ifndef TEST_MOCKS_MOCK_FUNC_POINTERS_H
#define TEST_MOCKS_MOCK_FUNC_POINTERS_H

#include "gl-mock.h"
#include "glad-mock.h"
#include "glfw-mock.h"
#include "saci-backend/sb-config-manager.h"

SA_STATIC struct sb_RenderApiFuncs func_pointers = {
    .gl = {
        .clear_color = glClearColor,
        .clear = glClear,
        .enable = glEnable,
        .debug_message_callback = glDebugMessageCallback,
        .get_string = glGetString,

        .uniform1ui = glUniform1ui,
        .uniform1i = glUniform1i,
        .uniform2f = glUniform2f,
        .uniform3f = glUniform3f,
        .uniform4f = glUniform4f,
        .uniform_matrix_2fv = glUniformMatrix2fv,
        .uniform_matrix_3fv = glUniformMatrix3fv,
        .uniform_matrix_4fv = glUniformMatrix4fv,
        .uniform_matrix_2x3fv = glUniformMatrix2x3fv,
        .uniform_matrix_2x4fv = glUniformMatrix2x4fv,
        .uniform_matrix_3x2fv = glUniformMatrix3x2fv,
        .uniform_matrix_3x4fv = glUniformMatrix3x4fv,
        .uniform_matrix_4x2fv = glUniformMatrix4x2fv,
        .uniform_matrix_4x3fv = glUniformMatrix4x3fv,

        .bind_vertex_array = glBindVertexArray,
        .bind_buffer = glBindBuffer,
        .buffer_data = glBufferData,
        .buffer_subdata = glBufferSubData,
        .gen_buffers = glGenBuffers,
        .gen_vertex_arrays = glGenVertexArrays,
        .vertex_attrib_pointer = glVertexAttribPointer,
        .vertex_attrib_divisor = glVertexAttribDivisor,
        .enable_vertex_attrib_array = glEnableVertexAttribArray,

        .create_shader = glCreateShader,
        .shader_source = glShaderSource,
        .compile_shader = glCompileShader,
        .get_shaderiv = glGetShaderiv,
        .get_shader_info_log = glGetShaderInfoLog,
        .delete_shader = glDeleteShader,

        .get_integer_v = glGetIntegerv,
        .get_boolean_v = glGetBooleanv,
        .blend_func = glBlendFunc,
        .get_error = glGetError,

        .create_program = glCreateProgram,
        .use_program = glUseProgram,
        .attach_shader = glAttachShader,
        .link_program = glLinkProgram,
        .get_program_iv = glGetProgramiv,
        .get_program_info_log = glGetProgramInfoLog,
        .detach_shader = glDetachShader,

        .draw_elements_instanced = glDrawElementsInstanced,
        .draw_elements = glDrawElements,

        .get_uniform_location = glGetUniformLocation,

        .gen_textures = glGenTextures,
        .bind_texture = glBindTexture,
        .active_texture = glActiveTexture,
        .tex_image_2d = glTexImage2D,
        .get_texlevel_parameter_iv = glGetTexLevelParameteriv,
        .generate_mipmap = glGenerateMipmap,
        .delete_textures = glDeleteTextures,
    },
};

SA_STATIC struct sb_RenderApiLoaderFuncs mock_loader_funcs = {
    .gl = {
        .load_opengl = gladLoadGL,
    },
};

SA_STATIC struct sb_WindowingApiFuncs mock_window_api_funcs = {
    .glfw = {
        .init = glfwInit,
        .set_hint = glfwWindowHint,
        .create_window = glfwCreateWindow,
        .get_proc = glfwGetProcAddress,
        .destroy_window = glfwDestroyWindow,
        .make_context_current = glfwMakeContextCurrent,
        .should_close = glfwWindowShouldClose,
        .set_pos_handler = glfwSetWindowPosCallback,
        .set_size_handler = glfwSetWindowSizeCallback,
        .terminate = glfwTerminate,
        .swap_buffers = glfwSwapBuffers,
        .poll_events = glfwPollEvents,
        .wait_events = glfwWaitEvents,
        .wait_events_timeout = glfwWaitEventsTimeout,
        .post_empty_event = glfwPostEmptyEvent,
        .set_mouse_pos_handler = glfwSetCursorPosCallback,
        .is_key_pressed = glfwGetKey,
    },
};

#endif // TEST_MOCKS_GL_MOCK_FUNC_POINTERS_H
