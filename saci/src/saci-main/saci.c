#include "saci-main/saci.h"

#include "saci-backend/sc-renderer.h"
#include "saci-backend/sc-windowing.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-debug.h"

#include <string.h>

#define SACI_RENDERER_AMOUNT 3

// Helper

struct saci_transform {
    su_vec3 pos;
    su_vec3 rotation;
    su_vec3 dimentions;
};

SA_INTERNAL_INLINE su_bool s_Has_Flag(su_u64 flag_var, su_u64 flag_to_check);

SA_INTERNAL double s_Get_Delta(void);

SA_INTERNAL void s_Init_Windowing(
    sc_window_t** window_out,
    su_s32 x,
    su_s32 y,
    const char* name);

SA_INTERNAL void s_Init_Memory(void);

SA_INTERNAL void s_Reset_Memory(void);

SA_INTERNAL su_mat4 s_Mat4_Create_Transform_From_Flag(
    struct saci_transform transform,
    su_u64 flag_var);

SA_INTERNAL void s_Begin_Renderer(sc_renderer* rendr);

SA_INTERNAL void s_Handle_Events(void);

/* === Header impl === */

enum saci_contextRendererLocation {
    saci_RENDERER_LOCATION_INSTANCE = 2,
};

enum saci_shapeType {
    saci_SHAPE_CUBE = 1,
};

// Each index is a shape so there is no need to store the shape type directly
struct saci_shapeDrawCall {
    su_dArray* transforms;
};

SA_INTERNAL struct saci_context {
    sc_renderer** renderer_array;

    struct saci_shapeDrawCall* shape_draw_call_array;

    struct saci_windowing {
        su_color bg_color;
        sc_window_t* window;
    } windowing;

    saci_loopFunc loop_func;

    saci_event event;

    su_u64 enable_flags;
} saci_context = {0};

void saci_Init(void) {
    s_Init_Windowing(&saci_context.windowing.window, 1600, 900, "test");
    s_Init_Memory();
}

void saci_Enable(saci_enableFlags enable_flag) {
    saci_context.enable_flags |= enable_flag;
}

void saci_Begin(void) {
    s_Handle_Events();
    sc_Window_Clear_Color(saci_context.windowing.bg_color);
    s_Begin_Renderer(saci_context.renderer_array[sc_RENDERER_INSTANCE]);
}

void saci_Set_Background_Color(const su_color color) {
    saci_context.windowing.bg_color = color;
}

void saci_Set_Loop_Func(saci_loopFunc loop_func) {
    su_Log_Assert_Message_m(loop_func, "Loop function is NULL");
    saci_context.loop_func = loop_func;
}

const saci_event* saci_Get_Event(void) {
    return NULL;
}

void saci_Loop(void) {
    while (!sc_Window_Should_Close(saci_context.windowing.window)) {
        saci_context.loop_func(s_Get_Delta());
    }
}

// Doesn't actually draw it but instead pushes to shape draw call array
void saci_Draw_Cube(const saci_cube cube) {
    struct saci_shapeDrawCall call;
    call = saci_context.shape_draw_call_array[saci_SHAPE_CUBE];

    su_mat4 transform = s_Mat4_Create_Transform_From_Flag(
        (struct saci_transform){
            cube.pos_center,
            cube.rotation,
            cube.dimentions,
        },
        saci_context.enable_flags);
    su_DArray_Push(call.transforms, &transform);
    saci_context.shape_draw_call_array[saci_SHAPE_CUBE] = call;
}

void saci_Present(void) {
    sc_Window_Swap_Buffer(saci_context.windowing.window);
}

void saci_Free(void) {}

// Helper

SA_INTERNAL void s_Init_Windowing(
    sc_window_t** window_out,
    su_s32 x,
    su_s32 y,
    const char* name) {
    su_Log_Assert_Message_m(sc_GLFW_Init(), "Could not load GLFW");

    *window_out = sc_Window_Create(
        x,
        y,
        name,
        NULL,
        NULL);
    sc_Window_Make_Context(*window_out);

    su_Log_Assert_Message_m(sc_GLAD_Init(), "Could not load GLAD");
}

SA_INTERNAL void s_Init_Memory(void) {
    saci_context.renderer_array = su_Calloc_m(SACI_RENDERER_AMOUNT, sizeof(sc_renderer*));
    struct sc_renderer* instance_rendr = sc_Renderer_New(sc_RENDERER_STATIC);
    saci_context.renderer_array[sc_RENDERER_INSTANCE] = instance_rendr;

    const su_s32 saci_shape_amount = 10; /// TODO
    saci_context.shape_draw_call_array = su_Calloc_m(saci_shape_amount, sizeof(struct saci_shapeDrawCall*));
    for (su_s32 i = 0; i < saci_shape_amount; ++i) {
        saci_context.shape_draw_call_array[i] = (struct saci_shapeDrawCall){
            .transforms = su_DArray_Create(1024, sizeof(su_mat4), su_TRUE),
        };
    }
}

SA_INTERNAL void s_Reset_Memory(void) {
    const su_s32 saci_shape_amount = 10; /// TODO
    for (su_s32 i = 0; i < saci_shape_amount; ++i) {
        su_DArray_Clear(saci_context.shape_draw_call_array[i].transforms);
    }
}

SA_INTERNAL su_mat4 s_Mat4_Create_Transform_From_Flag(
    struct saci_transform transform,
    su_u64 flag_var) {
    su_mat4 transform_mat = SU_IDENTITY_MAT4;
    if (s_Has_Flag(flag_var, saci_ENABLE_ROTATION_RTS)) { // Most likelly
        transform_mat = su_Mat4_Model_Matrix_RTS(
            transform.pos,
            transform.rotation,
            transform.dimentions);
    } else {
        transform_mat = su_Mat4_Model_Matrix_TRS(
            transform.pos,
            transform.rotation,
            transform.dimentions);
    }
    return transform_mat;
}

SA_INTERNAL void s_Begin_Renderer(sc_renderer* rendr) {
    s_Reset_Memory();
    sc_Renderer_Begin(rendr);
}

SA_INTERNAL void s_Handle_Events(void) {
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

    sc_Event_Poll();
}

SA_INTERNAL_INLINE su_bool s_Has_Flag(su_u64 flag_var, su_u64 flag_to_check) {
    return ((flag_var & flag_to_check) == flag_to_check);
}

SA_INTERNAL double s_Get_Delta(void) {
    return 0.0; // TODO
}
