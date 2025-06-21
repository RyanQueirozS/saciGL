#include "saci-main/saci.h"

#include "saci-backend/sc-renderer.h"
#include "saci-backend/sc-windowing.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-debug.h"

#define SACI_RENDERER_AMOUNT 2

// Helper

SA_INTERNAL su_bool s_Has_Flag(su_u64 flag_var, su_u64 flag_to_check) {
    return ((flag_var & flag_to_check) == flag_to_check);
}

SA_INTERNAL su_mat4 s_Mat4_Create_Transform_From_Flag(su_vec3 pos,
                                                      su_vec3 rotation,
                                                      su_vec3 dimentions,
                                                      su_u64 flag_var) {
    su_mat4 transform = SU_IDENTITY_MAT4;
    if (s_Has_Flag(flag_var, saci_ENABLE_ROTATION_RTS)) { // Most likelly
        transform = su_Mat4_Model_Matrix_RTS(
            pos,
            rotation,
            dimentions);
    } else {
        transform = su_Mat4_Model_Matrix_TRS(
            pos,
            rotation,
            dimentions);
    }
    return transform;
}

SA_INTERNAL double s_Get_Delta(void) {
    return 0.0; // TODO
}

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

struct saci_contextBoundInfo {
    su_vec3 rotation;
    su_vec3 scale;
    su_vec3 pos;
};

SA_INTERNAL struct saci_context {
    su_dArray* renderer_array;

    su_dArray* shape_draw_call_array;

    struct saci_contextBoundInfo bound_info;

    struct saci_windowing {
        sc_window_t* window;
    } windowing;

    saci_loopFunc loop_func;

    su_u64 enable_flags;
} saci_context = {0};

SA_API void saci_Init(void) {
    saci_context.renderer_array = su_DArray_Create(
        SACI_RENDERER_AMOUNT,
        sizeof(struct sc_renderer*),
        su_TRUE);
    struct sc_renderer* static_rendr = sc_Renderer_New(sc_RENDERER_STATIC);
    struct sc_renderer* instance_rendr = sc_Renderer_New(sc_RENDERER_STATIC);
    su_DArray_Push(saci_context.renderer_array, static_rendr);
    su_DArray_Push(saci_context.renderer_array, instance_rendr);
}

SA_API void saci_Begin(void) {
    for (su_u64 i = 0; i < su_DArray_Length(saci_context.renderer_array); ++i) {
        struct sc_renderer* renderer = su_DArray_Get_Ptr(saci_context.renderer_array, i);
        sc_Renderer_Begin(renderer);
    }

    for (su_u64 i = 0; i < su_DArray_Length(saci_context.renderer_array); ++i) {
        struct sc_renderer* renderer = su_DArray_Get_Ptr(saci_context.renderer_array, i);
        sc_Renderer_Draw(renderer);
    }
}

SA_API void saci_Set_Loop_Func(saci_loopFunc loop_func) {
    su_Log_Assert_Message_m(loop_func, "Loop function is NULL");
    saci_context.loop_func = loop_func;
}

SA_API void saci_Loop(void) {
    while (!sc_Window_Should_Close(saci_context.windowing.window)) {
        saci_context.loop_func(s_Get_Delta());
    }
}

SA_API su_vec3 saci_Translation_Rotation_Get(void) {
    return saci_context.bound_info.rotation;
}

SA_API void saci_Translation_Rotate(const su_vec3 rotation) {
}

// Doesn't actually draw it but instead pushes to shape draw call array
SA_API void saci_Draw_Cube(const saci_cube cube) {
    struct saci_shapeDrawCall* call;
    call = su_DArray_Get_Ptr(saci_context.shape_draw_call_array, saci_SHAPE_CUBE);

    su_mat4 transform = s_Mat4_Create_Transform_From_Flag(
        cube.pos_center,
        cube.rotation,
        cube.dimentions,
        saci_context.enable_flags);
    su_DArray_Push(call->transforms, &transform);
    su_DArray_Set(saci_context.shape_draw_call_array, saci_SHAPE_CUBE, call);
}

SA_API void saci_Present(void) {
}

SA_API void saci_Free(void) {}

// Helper
