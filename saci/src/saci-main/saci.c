#include "saci-main/saci.h"

#include "saci-backend/sb-renderer.h"
#include "saci-backend/sb-windowing.h"
#include "saci-backend/sb-config-manager.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-debug.h"

#include <saci-backend/sb-gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SACI_RENDERER_AMOUNT 3

#ifndef SACI_DEFAULT_CONFIG_PATH
#  define SACI_DEFAULT_CONFIG_PATH "saci-config.lua"
#endif // SACI_DEFAULT_CONFIG_PATH

// Helper

struct saci_Transform {
    su_Vec3 pos;
    su_Vec3 rotation;
    su_Vec3 dimentions;
};

su_DArray* saci_cube_pos;

su_DArray* saci_cube_index;

SA_INTERNAL_INLINE su_Bool saci__has_flag(su_U64 flag_var, su_U64 flag_to_check);

SA_INTERNAL double saci__get_delta(void);

SA_INTERNAL void saci__init_windowing(
    su_Window* window_out,
    su_S32 x,
    su_S32 y,
    const char* name);

SA_INTERNAL void saci__init_memory(void);

SA_INTERNAL void saci__reset_memory(void);

SA_INTERNAL su_Mat4 saci__mat4_create_transform_from_flag(
    struct saci_Transform transform,
    su_U64 flag_var);

SA_INTERNAL void saci__begin_renderer(sb_Renderer* rendr);

SA_INTERNAL void saci__handle_events(void);

/* === Header impl === */

enum saci_ContextRendererLocation {
    saci_RENDERER_LOCATION_INSTANCE = 2,
};

enum saci_ShapeType {
    saci_SHAPE_CUBE = 1,
};

// Each index is a shape so there is no need to store the shape type directly
struct saci_ShapeDrawCall {
    su_DArray* transforms;
    su_DArray* colors;
};

SA_INTERNAL struct saci_Context {

    struct saci_ShapeDrawCall* shape_draw_call_array;

    struct saci_Windowing {
        su_Color bg_color;
        su_Window window;
    } windowing;

    struct saci_RendererInfo {
        su_S32* uniform_location_array;
        sb_Renderer* renderer;
    }* renderer_info_array;

    saci_LoopFunc loop_func;

    saci_Event event;

    su_U64 enable_flags;
} saci_context = {0};

void saci_init(void) {
    if (!sb_cfg_manager_fetch(SACI_DEFAULT_CONFIG_PATH)) {
        sb_cfg_manager_load_default();
    }
    sb_cfg_manager_load_dependencies();
    su_Vec3 cube_vertices[] = {
        {-1, -1, -1},
        {-1, -1, 1},
        {-1, 1, -1},
        {-1, 1, 1},
        {1, -1, -1},
        {1, -1, 1},
        {1, 1, -1},
        {1, 1, 1}};
    int indices[36] = {
        1, 5, 7, 1, 7, 3,
        0, 2, 6, 0, 6, 4,
        0, 1, 3, 0, 3, 2,
        4, 6, 7, 4, 7, 5,
        2, 3, 7, 2, 7, 6,
        0, 4, 5, 0, 5, 1};

    saci_cube_pos = su_darray_create(8, sizeof(su_Vec3), su_TRUE);
    saci_cube_index = su_darray_create(36, sizeof(su_U32), su_TRUE);
    for (int i = 0; i < 8; ++i) {
        su_LOG_ASSERT_MESSAGE_M(su_darray_push(saci_cube_pos, &cube_vertices[i]), "Could not push to cube_pos DArray");
    }
    for (int i = 0; i < 36; ++i) {
        su_LOG_ASSERT_MESSAGE_M(su_darray_push(saci_cube_index, &indices[i]), "Could not push to cube_index DArray");
    }

    saci__init_windowing(&saci_context.windowing.window, 1600, 900, "test");
    saci__init_memory();
}

void saci_enable(saci_RenderingFlags enable_flag, su_Bool enable) {
    if (enable) {
        saci_context.enable_flags |= enable_flag;
    } else {
        saci_context.enable_flags &= ~enable_flag;
    }
}

void saci_begin(void) {
    saci__handle_events();
    sb_window_clear_color(saci_context.windowing.bg_color);
    saci__begin_renderer(saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer);
}

void saci_set_background_color(const su_Color color) {
    saci_context.windowing.bg_color = color;
}

void saci_set_loop_func(saci_LoopFunc loop_func) {
    su_LOG_ASSERT_MESSAGE_M(loop_func, "Loop function is NULL");
    saci_context.loop_func = loop_func;
}

const saci_Event* saci_get_event(void) {
    return NULL;
}

void saci_loop(void) {
    while (!sb_window_should_close(saci_context.windowing.window)) {
        saci_context.loop_func(saci__get_delta());
    }
}

// Doesn't actually draw it but instead pushes to shape draw call array
void saci_draw_cube(const saci_Cube cube) {
    su_Mat4 transform = saci__mat4_create_transform_from_flag(
        (struct saci_Transform){
            cube.pos_center,
            cube.rotation,
            cube.dimentions,
        },
        saci_context.enable_flags);
    if (!su_darray_push(saci_context.shape_draw_call_array[saci_SHAPE_CUBE].transforms,
                        &transform)) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_SACI_MAIN_SHAPES, "Could not push cube transform");
    }
    if (!su_darray_push(saci_context.shape_draw_call_array[saci_SHAPE_CUBE].colors,
                        &cube.color)) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_SACI_MAIN_SHAPES, "Could not push cube color");
    }
}

void saci_present(void) {
    su_Mat4 proj = su_mat4_perspective(90, 16.0f / 9.0f, 1, 100);
    su_Mat4 view = su_mat4_look_at((su_Vec3){0.0f, 2.0f, -20.0f},
                                   (su_Vec3){0.0f, 0.0f, 0.0f},
                                   (su_Vec3){0.0f, 1.0f, 0.0f});
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[0],
        &su_IDENTITY_MAT4,
        su_TYPE_MAT4);
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[1],
        &view,
        su_TYPE_MAT4);
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[2],
        &proj,
        su_TYPE_MAT4);
    sb_renderer_bind_index_buffer(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_cube_index);
    sb_renderer_set_instance_colors(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.shape_draw_call_array[saci_SHAPE_CUBE].colors);
    sb_renderer_set_instance_transforms(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.shape_draw_call_array[saci_SHAPE_CUBE].transforms);
    sb_renderer_push_mesh(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_cube_pos,
        NULL,
        NULL);
    sb_renderer_draw(saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer);
    sb_window_swap_buffer(saci_context.windowing.window);
}

void saci_free(void) {}

// Helper

SA_INTERNAL void saci__init_windowing(
    su_Window* window_out,
    su_S32 x,
    su_S32 y,
    const char* name) {
    su_LOG_ASSERT_MESSAGE_M(sb_load_windowing(), "Could not load window");

    *window_out = sb_window_create(
        x,
        y,
        name,
        NULL,
        NULL);
    sb_window_make_context(*window_out);

    su_LOG_ASSERT_MESSAGE_M(sb_proc_load(), "Could not load proc");
    sb_gl_load();
}

SA_INTERNAL void saci__init_memory(void) {
    saci_context.renderer_info_array = su_CALLOC_M(
        SACI_RENDERER_AMOUNT,
        sizeof(struct saci_RendererInfo*));
    struct sb_Renderer* instance_rendr = sb_renderer_new(sb_RENDERER_STATIC);
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer = instance_rendr;
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array = su_MALLOC_M(sizeof(su_S32) * 5);
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[0] =
        sb_renderer_get_uniform_id(instance_rendr, "u_model_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[1] =
        sb_renderer_get_uniform_id(instance_rendr, "u_view_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[2] =
        sb_renderer_get_uniform_id(instance_rendr, "u_projection_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[3] =
        sb_renderer_get_uniform_id(instance_rendr, "u_flags");

    const su_S32 saci_shape_amount = 10; /// TODO
    saci_context.shape_draw_call_array = su_CALLOC_M(
        saci_shape_amount,
        sizeof(struct saci_ShapeDrawCall*));
    for (su_S32 i = 0; i < saci_shape_amount; ++i) {
        saci_context.shape_draw_call_array[i] = (struct saci_ShapeDrawCall){
            .transforms = su_darray_create(1024, sizeof(su_Mat4), su_TRUE),
            .colors = su_darray_create(1024, sizeof(su_Color), su_TRUE),
        };
    }
}

SA_INTERNAL void saci__reset_memory(void) {
    const su_S32 saci_shape_amount = 10; /// TODO
    for (su_S32 i = 0; i < saci_shape_amount; ++i) {
        su_darray_clear(saci_context.shape_draw_call_array[i].transforms);
        su_darray_clear(saci_context.shape_draw_call_array[i].colors);
    }
}

SA_INTERNAL su_Mat4 saci__mat4_create_transform_from_flag(
    struct saci_Transform transform,
    su_U64 flag_var) {
    su_Mat4 transform_mat = su_IDENTITY_MAT4;
    if (!saci__has_flag(flag_var, saci_RENDERING_FLAG_ROTATION_RTS)) { // Most likelly
        transform_mat = su_mat4_model_matrix_trs(
            transform.pos,
            transform.rotation,
            transform.dimentions);
    } else {
        transform_mat = su_mat4_model_matrix_rts(
            transform.pos,
            transform.rotation,
            transform.dimentions);
    }
    return transform_mat;
}

SA_INTERNAL void saci__begin_renderer(sb_Renderer* rendr) {
    saci__reset_memory();
    sb_renderer_begin(rendr);
}

SA_INTERNAL void saci__handle_events(void) {
    memset(saci_context.event.keyboard.key_was_pressed, 0,
           sizeof(saci_context.event.keyboard.key_was_pressed));

    memset(saci_context.event.keyboard.key_was_released, 0,
           sizeof(saci_context.event.keyboard.key_was_released));

    memset(saci_context.event.mouse.button_was_pressed, 0,
           sizeof(saci_context.event.mouse.button_was_pressed));

    memset(saci_context.event.mouse.button_was_released, 0,
           sizeof(saci_context.event.mouse.button_was_released));

    memset(saci_context.event.controller.button_was_pressed, 0,
           sizeof(saci_context.event.controller.button_was_pressed));

    memset(saci_context.event.controller.button_was_released, 0,
           sizeof(saci_context.event.controller.button_was_released));

    sb_event_poll();
}

SA_INTERNAL_INLINE su_Bool saci__has_flag(su_U64 flag_var, su_U64 flag_to_check) {
    return ((flag_var & flag_to_check) == flag_to_check);
}

SA_INTERNAL double saci__get_delta(void) {
    return 0.0; // TODO
}
