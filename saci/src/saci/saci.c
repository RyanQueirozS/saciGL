#include "saci/saci.h"

#include "saci_util/math.h"
#include "saci_util/darray.h"
#include "saci_util/memory.h"
#include "saci_util/internal/general.h"
#include "saci_util/log.h"
#include "saci_util/internal/log.h"
#include "saci_util/types.h"

#include "saci_core/renderer/renderer.h"
#include "saci_core/runtime/looper.h"
#include "saci_core/runtime/windowing.h"
#include "saci_core/startup/startup.h"
#include "saci_core/runtime/event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SACI_RENDERER_AMOUNT 3

SACI_INTERNAL void saci__init_memory(void);

SACI_INTERNAL void saci__reset_memory(void);

SACI_INTERNAL void saci__begin_renderer(CSaciRenderer* rendr);

SACI_INTERNAL void saci__handle_events(void);

/* === Header impl === */

enum SaciContextRendererLocation {
    SACI_RENDERER_LOCATION_INSTANCE = 2,
};

struct SaciShapeDrawCall {
    SaciDArray* instance_data_array; // SaciInstanceData
};

enum {
    SACI_UNIFORM_DEFAULT_MODEL_MATRIX = 0,
    SACI_UNIFORM_DEFAULT_VIEW_MATRIX = 1,
    SACI_UNIFORM_DEFAULT_PROJ_MATRIX = 2,
    SACI_UNIFORM_DEFAULT_FLAGS = 3,
};

SACI_INTERNAL struct {
    // Each index is a shape
    struct SaciShapeDrawCall* shape_instance_data_array;

    struct CSaciWindowingCtx* windowing_ctx_ptr_array[CSACI_WINDOWING_MAX_WINDOW_CONTEXTS];

    struct SaciRendererInfo {
        SaciS32* uniform_location_array;
        CSaciRenderer* renderer;
    }* renderer_info_array;

    SaciEvent event;

    SaciU64 enable_flags;
} saci_g_context = {0};

void saci_init(void)
{
    csaci_startup_dependencies_load();
    csaci_startup_gfx_load();
    {
        struct CSaciWindowProperties props = {
            .height = 900,
            .width = 1600,
            .title = "Test",
            .x = 0,
            .y = 0,
            .clear_color = {0},
        };
        csaci_windowing_init(props,
                             saci_g_context.windowing_ctx_ptr_array
                                 [csaci_windowing_get_ctx_count()]);
    }
    saci__init_memory();
}

void saci_enable(SaciFlags flag, SaciBool enable)
{
    if (enable) {
        saci_g_context.enable_flags |= flag;
    } else {
        saci_g_context.enable_flags &= ~flag;
    }
}

void saci_begin(void)
{
    saci__handle_events();
    csaci_windowing_clear();
    // saci__begin_renderer(saci_g_context.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer);
}

void saci_set_background_color(const SaciColor color)
{
    csaci_windowing_set_clear_color(color);
}

void saci_set_loop_func(SaciLoopFunc loop_func)
{
    SACI_LOG_ASSERT_M(loop_func, SACI_LOG_CONTEXT_CORE_MAINLOOP, "Loop function is NULL");
    csaci_looper_set_main_loop(loop_func, (CSaciLoopOpts){.desired_fps = 60.0f});
}

const SaciEvent* saci_get_event(void)
{
    return NULL;
}

void saci_loop(void)
{
    csaci_looper_run();
}

// Doesn't actually draw it but instead pushes to shape draw call array
void saci_present(void)
{
    // SaciMat4 proj = saci_mat4_perspective(90, saci_g_contextn.windowing.width / saci_g_contextn.windowing.height, 1, 100);
    SaciMat4 view = saci_mat4_look_at((SaciVec3){0.0f, 2.0f, -20.0f},
                                      (SaciVec3){0.0f, 0.0f, 0.0f},
                                      (SaciVec3){0.0f, 1.0f, 0.0f});
    (void)view;
    // csaci_renderer_set_uniform(
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer,
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_MODEL_MATRIX],
    //     &su_IDENTITY_MAT4,
    //     su_TYPE_MAT4);
    // csaci_renderer_set_uniform(
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer,
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_VIEW_MATRIX],
    //     &view,
    //     su_TYPE_MAT4);
    // csaci_renderer_set_uniform(
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer,
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array[saci__UNIFORM_DEFAULT_PROJ_MATRIX],
    //     &proj,
    //     su_TYPE_MAT4);
    // csaci_renderer_bind_index_buffer(
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer,
    //     saci_g_cube_index);
    // csaci_renderer_set_instance_data_array(
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer,
    //     saci_g_contextn.shape_instance_data_array[SACI_SHAPE_CUBE].instance_data_array);
    // csaci_renderer_push_mesh(
    //     saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer,
    //     saci_g_cube_pos,
    //     NULL,
    //     NULL);
    // csaci_renderer_draw(saci_g_contextn.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer);
    csaci_windowing_present();
}

void saci_end(void)
{
    saci_mem_print_info();
}

// Helper

// SACI_INTERNAL void saci__init_windowing(
//     union sb_Window* window_out,
//     SaciS32 x,
//     SaciS32 y,
//     const char* name)
// {
// SACI_LOG_ASSERT_M(sb_window_load(), SACI_LOG_CONTEXT_CORE_INIT, "Could not load window");
//
// *window_out = sb_window_create(
//     x,
//     y,
//     name,
//     (union sb_WindowOpts){0});
// sb_window_make_context(*window_out);
// }

SACI_INTERNAL void saci__init_memory(void)
{
    saci_g_context.renderer_info_array = calloc(
        SACI_RENDERER_AMOUNT,
        sizeof(struct SaciRendererInfo*));
    // struct CSaciRenderer* instance_rendr = csaci_renderer_new(CSACI_RENDERER_INSTANCE, "instance");
    // saci_g_context.renderer_info_array[CSACI_RENDERER_INSTANCE].renderer = instance_rendr;
    // saci_g_context.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array = malloc(sizeof(SaciS32) * 5);
    // saci_g_context.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array[SACI_UNIFORM_DEFAULT_MODEL_MATRIX] =
    //     csaci_renderer_get_uniform_id(instance_rendr, "u_model_matrix");
    // saci_g_context.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array[SACI_UNIFORM_DEFAULT_VIEW_MATRIX] =
    //     csaci_renderer_get_uniform_id(instance_rendr, "u_view_matrix");
    // saci_g_context.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array[SACI_UNIFORM_DEFAULT_PROJ_MATRIX] =
    //     csaci_renderer_get_uniform_id(instance_rendr, "u_projection_matrix");
    // saci_g_context.renderer_info_array[CSACI_RENDERER_INSTANCE].uniform_location_array[SACI_UNIFORM_DEFAULT_FLAGS] =
    //     csaci_renderer_get_uniform_id(instance_rendr, "u_flags");

    const SaciS32 saci_shape_amount = 10; /// TODO
    saci_g_context.shape_instance_data_array = calloc(
        SACI_CAST_M(SaciU64)(saci_shape_amount),
        sizeof(struct SaciShapeDrawCall));
    for (SaciS32 i = 0; i < saci_shape_amount; ++i) {
        // saci_g_contextn.shape_instance_data_array[i].instance_data_array = saci_darray_create(1024, sizeof(SaciInstanceData), SACI_TRUE);
    }
}

SACI_INTERNAL void saci__reset_memory(void)
{
    const SaciS32 saci_shape_amount = 10; /// TODO
    for (SaciS32 i = 0; i < saci_shape_amount; ++i) {
        if (!saci_darray_clear(saci_g_context.shape_instance_data_array[i].instance_data_array)) {
            SACI_LOG_ERROR_M(
                SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                SACI_LOG_CONTEXT_MAIN_SHAPES_DRAW, "Could not reset shape instance");
        }
    }
}

SACI_INTERNAL void saci__begin_renderer(CSaciRenderer* rendr)
{
    saci__reset_memory();
    // csaci_renderer_begin(rendr);
}

SACI_INTERNAL void saci__handle_events(void)
{
    csaci_event_poll(&saci_g_context.event);
}
