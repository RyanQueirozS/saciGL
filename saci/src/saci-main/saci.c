#include "saci-main/saci.h"

#include "saci-utils/config/su-config-manager.h"
#include "saci-utils/math/su-math-mat.h"
#include "saci-utils/memory/su-memory.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-log.h"
#include "saci-utils/su-types-common.h"

#include "saci-backend/graphics/sb-gfx.h"
#include "saci-backend/graphics/sb-graphics.h"
#include "saci-backend/renderer/sb-renderer.h"
#include "saci-backend/resources/sb-dependency.h"

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

typedef struct sb_GFXInstanceData saci_InstanceData;
typedef struct {
    su_Mat4 transform;
    su_Color color;
} saci_ShapeInstance;

struct saci_ShapeDrawCall {
    su_DArray* instance_data_array; // saci_InstanceData
};

enum saci__UniformDefaults {
    saci__UNIFORM_DEFAULT_MODEL_MATRIX = 0,
    saci__UNIFORM_DEFAULT_VIEW_MATRIX = 1,
    saci__UNIFORM_DEFAULT_PROJ_MATRIX = 2,
    saci__UNIFORM_DEFAULT_FLAGS = 3,
};

SA_INTERNAL struct saci_Context {

    // Each index is a shape
    struct saci_ShapeDrawCall* shape_instance_data_array;

    struct saci_Windowing {
        su_Color bg_color;
        su_Window window;
        int x, y;
        float width, height;
    } windowing;

    struct saci_RendererInfo {
        su_S32* uniform_location_array;
        sb_Renderer* renderer;
    }* renderer_info_array;

    saci_LoopFunc loop_func;

    saci_Event event;

    su_U64 enable_flags;
} saci_context = {0};

void saci_init(void)
{
    if (!su_cfg_manager_fetch(SACI_DEFAULT_CONFIG_PATH)) {
        su_cfg_manager_load_default();
    }
    sb_dependecies_load();
    sb_gfx_load();
    sb_gfx_initialize_renderer_debugger();
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
        su_LOG_ASSERT_M(su_darray_push(saci_cube_pos, &cube_vertices[i]), su_LOG_CONTEXT_MAIN_SHAPES_INIT, "Could not initialize cube model mesh vertices");
    }
    for (int i = 0; i < 36; ++i) {
        su_LOG_ASSERT_M(su_darray_push(saci_cube_index, &indices[i]), su_LOG_CONTEXT_CORE_INIT, "Could not initialize cube model mesh indices");
    }

    saci_context.windowing.width = 800,
    saci_context.windowing.height = 600;
    saci__init_windowing(&saci_context.windowing.window,
                         (int)saci_context.windowing.width,
                         (int)saci_context.windowing.height, "test");
    saci__init_memory();
}

void saci_enable(saci_RenderingFlags render_flag, su_Bool enable)
{
    if (enable) {
        saci_context.enable_flags |= render_flag;
    } else {
        saci_context.enable_flags &= ~render_flag;
    }
}

void saci_begin(void)
{
    saci__handle_events();
    sb_graphics_clear_color(saci_context.windowing.bg_color);
    sb_graphics_clear_depth_buffer();
    saci__begin_renderer(saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer);
}

void saci_set_background_color(const su_Color color)
{
    saci_context.windowing.bg_color = color;
}

void saci_set_loop_func(saci_LoopFunc loop_func)
{
    su_LOG_ASSERT_M(loop_func, su_LOG_CONTEXT_CORE_MAINLOOP, "Loop function is NULL");
    saci_context.loop_func = loop_func;
}

const saci_Event* saci_get_event(void)
{
    return NULL;
}

void saci_loop(void)
{
    while (!sb_window_should_close(saci_context.windowing.window)) {
        saci_context.loop_func(saci__get_delta());
    }
}

// Doesn't actually draw it but instead pushes to shape draw call array
void saci_draw_cube(const saci_Cube cube)
{
    su_Mat4 transform = saci__mat4_create_transform_from_flag(
        (struct saci_Transform){
            cube.pos_center,
            cube.rotation,
            cube.dimentions,
        },
        cube.flags);
    saci_ShapeInstance instance = {
        .color = cube.fill_color,
        .transform = transform,
    };
    saci_InstanceData instance_data = {
        .data_size = sizeof(saci_ShapeInstance),
        .location = 3,
        .instance_data_structure = malloc(sizeof(saci_ShapeInstance)), // TODO, this needs to be prefilled, perhaps be stored as a static var
    };
    memcpy(instance_data.instance_data_structure, &instance, sizeof(saci_ShapeInstance));
    if (!su_darray_push(saci_context.shape_instance_data_array[saci_SHAPE_CUBE].instance_data_array,
                        &instance_data)) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_MAIN_SHAPES_DRAW, "Could not push cube transform");
    }
}

void saci_present(void)
{
    su_Mat4 proj = su_mat4_perspective(90, saci_context.windowing.width / saci_context.windowing.height, 1, 100);
    su_Mat4 view = su_mat4_look_at((su_Vec3){0.0f, 2.0f, -20.0f},
                                   (su_Vec3){0.0f, 0.0f, 0.0f},
                                   (su_Vec3){0.0f, 1.0f, 0.0f});
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_MODEL_MATRIX],
        &su_IDENTITY_MAT4,
        su_TYPE_MAT4);
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_VIEW_MATRIX],
        &view,
        su_TYPE_MAT4);
    sb_renderer_set_uniform(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_PROJ_MATRIX],
        &proj,
        su_TYPE_MAT4);
    sb_renderer_bind_index_buffer(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_cube_index);
    sb_renderer_set_instance_data_array(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_context.shape_instance_data_array[saci_SHAPE_CUBE].instance_data_array);
    sb_renderer_push_mesh(
        saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer,
        saci_cube_pos,
        NULL,
        NULL);
    sb_renderer_draw(saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer);
    sb_window_swap_buffer(saci_context.windowing.window);
}

void saci_free(void)
{
    su_mem_print_info();
}

// Helper

SA_INTERNAL void saci__init_windowing(
    su_Window* window_out,
    su_S32 x,
    su_S32 y,
    const char* name)
{
    su_LOG_ASSERT_M(sb_load_windowing(), su_LOG_CONTEXT_CORE_INIT, "Could not load window");

    *window_out = sb_window_create(
        x,
        y,
        name,
        NULL,
        NULL);
    sb_window_make_context(*window_out);

    su_LOG_ASSERT_M(sb_proc_load(), su_LOG_CONTEXT_CORE_INIT, "Could not load proc");
}

SA_INTERNAL void saci__init_memory(void)
{
    saci_context.renderer_info_array = calloc(
        SACI_RENDERER_AMOUNT,
        sizeof(struct saci_RendererInfo*));
    struct sb_Renderer* instance_rendr = sb_renderer_new(sb_RENDERER_INSTANCE, "instance");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].renderer = instance_rendr;
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array = malloc(sizeof(su_S32) * 5);
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_MODEL_MATRIX] =
        sb_renderer_get_uniform_id(instance_rendr, "u_model_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_VIEW_MATRIX] =
        sb_renderer_get_uniform_id(instance_rendr, "u_view_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_PROJ_MATRIX] =
        sb_renderer_get_uniform_id(instance_rendr, "u_projection_matrix");
    saci_context.renderer_info_array[sb_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_FLAGS] =
        sb_renderer_get_uniform_id(instance_rendr, "u_flags");

    const su_S32 saci_shape_amount = 10; /// TODO
    saci_context.shape_instance_data_array = calloc(
        su_CAST_M(su_U64)(saci_shape_amount),
        sizeof(struct saci_ShapeDrawCall));
    for (su_S32 i = 0; i < saci_shape_amount; ++i) {
        saci_context.shape_instance_data_array[i].instance_data_array = su_darray_create(1024, sizeof(saci_InstanceData), su_TRUE);
    }
}

SA_INTERNAL void saci__reset_memory(void)
{
    const su_S32 saci_shape_amount = 10; /// TODO
    for (su_S32 i = 0; i < saci_shape_amount; ++i) {
        if (!su_darray_clear(saci_context.shape_instance_data_array[i].instance_data_array)) {
            su_LOG_ERROR_M(
                su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                su_LOG_CONTEXT_MAIN_SHAPES_DRAW, "Could not reset shape instance");
        }
    }
}

SA_INTERNAL su_Mat4 saci__mat4_create_transform_from_flag(
    struct saci_Transform transform,
    su_U64 flag_var)
{
    su_Mat4 transform_mat = su_IDENTITY_MAT4;
    if (!su_HAS_FLAG(flag_var, saci_MODEL_FLAG_ROTATION_RTS)) { // Most likelly
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

SA_INTERNAL void saci__begin_renderer(sb_Renderer* rendr)
{
    saci__reset_memory();
    sb_renderer_begin(rendr);
}

SA_INTERNAL void saci__handle_events(void)
{
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

SA_INTERNAL double saci__get_delta(void)
{
    return 0.0; // TODO
}
