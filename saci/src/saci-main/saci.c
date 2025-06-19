#include "saci-main/saci.h"

#include "saci-backend/sc-renderer.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#define SACI_RENDERER_AMOUNT 2

// Helper

/* === Header impl === */

enum saci_contextRendererLocation {
    saci_RENDERER_LOCATION_INSTANCE = 2,
};

enum saci_shapeType {
    saci_SHAPE_CUBE = 1,
};

struct saci_shapeDrawCall {
    sa_dArray* transforms;
};

struct saci_contextShapeInfo {
    sa_vec3 rotation;
};

struct saci_contextBoundInfo {
    struct saci_contextShapeInfo shapes;
};

SA_INTERNAL struct saci_context {
    sa_dArray* renderer_array;

    sa_dArray* shape_draw_call_array;

    struct saci_contextBoundInfo bound_info;

} saci_context = {0};

SA_API void saci_Init(void) {
    saci_context.renderer_array = sa_DArray_Create(
        SACI_RENDERER_AMOUNT,
        sizeof(struct sc_renderer*),
        sa_TRUE);
    struct sc_renderer* static_rendr = sc_Renderer_New(sc_RENDERER_STATIC);
    struct sc_renderer* instance_rendr = sc_Renderer_New(sc_RENDERER_STATIC);
    sa_DArray_Push(saci_context.renderer_array, static_rendr);
    sa_DArray_Push(saci_context.renderer_array, instance_rendr);
}

SA_API void saci_Begin(void) {
    for (sa_u64 i = 0; i < sa_DArray_Length(saci_context.renderer_array); ++i) {
        struct sc_renderer* renderer = sa_DArray_Get_Ptr(saci_context.renderer_array, i);
        sc_Renderer_Begin(renderer);
    }

    for (sa_u64 i = 0; i < sa_DArray_Length(saci_context.renderer_array); ++i) {
        struct sc_renderer* renderer = sa_DArray_Get_Ptr(saci_context.renderer_array, i);
        sc_Renderer_Draw(renderer);
    }
}

SA_API void saci_Uniform_Translation_Rotate(const sa_vec3 rotation) {
}

// Doesn't actually draw it but instead pushes to shape draw call array
SA_API void saci_Draw_Cube(const saci_cube cube) {
    struct saci_shapeDrawCall* call;
    call = sa_DArray_Get_Ptr(saci_context.shape_draw_call_array, saci_SHAPE_CUBE);

    sa_mat4 transform = sa_Mat4_Model_Matrix_RTS(
        cube.pos_center,
        saci_context.bound_info.shapes.rotation,
        cube.dimentions);
    sa_DArray_Push(call->transforms, &transform);
}

SA_API void saci_Present(void) {
}

SA_API void saci_Free(void) {}

// Helper
