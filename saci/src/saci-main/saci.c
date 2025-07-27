#include "saci-main/saci.h"

#include "saci-backend/sb-renderer.h"
#include "saci-backend/sb-windowing.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SACI_RENDERER_AMOUNT 3

// Helper

struct saci_Transform {
    su_vec3 pos;
    su_vec3 rotation;
    su_vec3 dimentions;
};

su_dArray* saci_cube_pos;

su_dArray* saci_cube_index;

SA_INTERNAL_INLINE su_bool saci__has_flag(su_u64 flag_var, su_u64 flag_to_check);

SA_INTERNAL double saci__get_delta(void);

SA_INTERNAL void saci__init_windowing(
    sb_Window** window_out,
    su_s32 x,
    su_s32 y,
    const char* name);

SA_INTERNAL void saci__init_memory(void);

SA_INTERNAL void saci__reset_memory(void);

SA_INTERNAL su_mat4 saci__mat4_create_transform_from_flag(
    struct saci_Transform transform,
    su_u64 flag_var);

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
    su_dArray* transforms;
    su_dArray* colors;
};

SA_INTERNAL struct saci_Context {

    struct saci_ShapeDrawCall* shape_draw_call_array;

    struct saci_Windowing {
        su_color bg_color;
        sb_Window* window;
    } windowing;

    struct saci_RendererInfo {
        su_s32* uniform_location_array;
        sb_Renderer* renderer;
    }* renderer_info_array;

    saci_LoopFunc loop_func;

    saci_Event event;

    su_u64 enable_flags;
} saci_context = {0};

void saci_Init(void) {
    su_vec3 cube_vertices[] = {
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

    saci_cube_pos = su_DArray_Create(8, sizeof(su_vec3), su_TRUE);
    saci_cube_index = su_DArray_Create(36, sizeof(su_u32), su_TRUE);
    for (int i = 0; i < 8; ++i) {
        su_Log_Assert_Message_m(su_DArray_Push(saci_cube_pos, &cube_vertices[i]), "Could not push to cube_pos DArray");
    }
    for (int i = 0; i < 36; ++i) {
        su_Log_Assert_Message_m(su_DArray_Push(saci_cube_index, &indices[i]), "Could not push to cube_index DArray");
    }

    saci__init_windowing(&saci_context.windowing.window, 1600, 900, "test");
    saci__init_memory();
}

void saci_Enable(saci_RenderingFlags enable_flag, su_bool enable) {
    if (enable) {
        saci_context.enable_flags |= enable_flag;
    } else {
        saci_context.enable_flags &= ~enable_flag;
    }
}

void saci_Begin(void) {
    saci__handle_events();
    sb_window_clear_color(saci_context.windowing.bg_color);
    saci__begin_renderer(saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer);
}

void saci_Set_Background_Color(const su_color color) {
    saci_context.windowing.bg_color = color;
}

void saci_Set_Loop_Func(saci_LoopFunc loop_func) {
    su_Log_Assert_Message_m(loop_func, "Loop function is NULL");
    saci_context.loop_func = loop_func;
}

const saci_Event* saci_Get_Event(void) {
    return NULL;
}

void saci_Loop(void) {
    while (!sb_window_should_close(saci_context.windowing.window)) {
        saci_context.loop_func(saci__get_delta());
    }
}

// Doesn't actually draw it but instead pushes to shape draw call array
void saci_Draw_Cube(const saci_Cube cube) {
    su_mat4 transform = saci__mat4_create_transform_from_flag(
        (struct saci_Transform){
            cube.pos_center,
            cube.rotation,
            cube.dimentions,
        },
        saci_context.enable_flags);
    if (!su_DArray_Push(saci_context.shape_draw_call_array[saci_SHAPE_CUBE].transforms,
                        &transform)) {
        su_Log_Error_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_SACI_MAIN_SHAPES, "Could not push cube transform");
    }
    if (!su_DArray_Push(saci_context.shape_draw_call_array[saci_SHAPE_CUBE].colors,
                        &cube.color)) {
        su_Log_Error_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_SACI_MAIN_SHAPES, "Could not push cube color");
    }
}

void saci_Present(void) {
    su_mat4 proj = su_Mat4_Perspective(90, 16.0f / 9.0f, 1, 100);
    su_mat4 view = su_Mat4_Look_At((su_vec3){0.0f, 2.0f, -20.0f},
                                   (su_vec3){0.0f, 0.0f, 0.0f},
                                   (su_vec3){0.0f, 1.0f, 0.0f});
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[0],
        &SU_IDENTITY_MAT4,
        SU_TYPE_MAT4);
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[1],
        &view,
        SU_TYPE_MAT4);
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[2],
        &proj,
        SU_TYPE_MAT4);
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

void saci_Free(void) {}

// Helper

SA_INTERNAL void saci__init_windowing(
    sb_Window** window_out,
    su_s32 x,
    su_s32 y,
    const char* name) {
    su_Log_Assert_Message_m(sb_glfw_init(), "Could not load GLFW");

    *window_out = sb_window_create(
        x,
        y,
        name,
        NULL,
        NULL);
    sb_window_make_context(*window_out);

    su_Log_Assert_Message_m(sb_glad_init(), "Could not load GLAD");
}

SA_INTERNAL void saci__init_memory(void) {
    saci_context.renderer_info_array = su_Calloc_m(
        SACI_RENDERER_AMOUNT,
        sizeof(struct saci_RendererInfo*));
    struct sb_Renderer* instance_rendr = sb_renderer_new(sb_RENDERER_STATIC);
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer = instance_rendr;
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array = su_Malloc_m(sizeof(su_s32) * 5);
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[0] =
        sb_renderer_get_uniform_id(instance_rendr, "u_model_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[1] =
        sb_renderer_get_uniform_id(instance_rendr, "u_view_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[2] =
        sb_renderer_get_uniform_id(instance_rendr, "u_projection_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[3] =
        sb_renderer_get_uniform_id(instance_rendr, "u_flags");

    const su_s32 saci_shape_amount = 10; /// TODO
    saci_context.shape_draw_call_array = su_Calloc_m(
        saci_shape_amount,
        sizeof(struct saci_ShapeDrawCall*));
    for (su_s32 i = 0; i < saci_shape_amount; ++i) {
        saci_context.shape_draw_call_array[i] = (struct saci_ShapeDrawCall){
            .transforms = su_DArray_Create(1024, sizeof(su_mat4), su_TRUE),
            .colors = su_DArray_Create(1024, sizeof(su_color), su_TRUE),
        };
    }
}

SA_INTERNAL void saci__reset_memory(void) {
    const su_s32 saci_shape_amount = 10; /// TODO
    for (su_s32 i = 0; i < saci_shape_amount; ++i) {
        su_DArray_Clear(saci_context.shape_draw_call_array[i].transforms);
        su_DArray_Clear(saci_context.shape_draw_call_array[i].colors);
    }
}

SA_INTERNAL su_mat4 saci__mat4_create_transform_from_flag(
    struct saci_Transform transform,
    su_u64 flag_var) {
    su_mat4 transform_mat = SU_IDENTITY_MAT4;
    if (!saci__has_flag(flag_var, saci_RENDERING_FLAG_ROTATION_RTS)) { // Most likelly
        transform_mat = su_Mat4_Model_Matrix_TRS(
            transform.pos,
            transform.rotation,
            transform.dimentions);
    } else {
        transform_mat = su_Mat4_Model_Matrix_RTS(
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

SA_INTERNAL_INLINE su_bool saci__has_flag(su_u64 flag_var, su_u64 flag_to_check) {
    return ((flag_var & flag_to_check) == flag_to_check);
}

SA_INTERNAL double saci__get_delta(void) {
    return 0.0; // TODO
}
