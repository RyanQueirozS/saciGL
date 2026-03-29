#include "saci/shapes.h"
#include "saci/internal/shapes.h"
#include "saci_util/internal/general.h"
#include "saci_util/math.h"

/* === Internal === */

struct SaciTransform {
    SaciVec3 pos;
    SaciVec3 rotation;
    SaciVec3 dimentions;
};

enum SaciShapeType {
    SACI_SHAPE_CUBE = 1,
};

typedef struct {
    SaciMat4 transform;
    SaciColor color;
} SaciShapeInstance;

SACI_INTERNAL SaciMat4 saci__mat4_create_transform_from_flag(
    struct SaciTransform transform,
    SaciU64 flag_var);

/* === Header declaration === */

void saci_draw_cube(const SaciCube cube)
{
    SaciMat4 transform = saci__mat4_create_transform_from_flag(
        (struct SaciTransform){
            cube.pos_center,
            cube.rotation,
            cube.dimentions,
        },
        cube.flags);
    SaciShapeInstance instance = {
        .color = cube.fill_color,
        .transform = transform,
    };
    (void)instance;
    // SaciInstanceData instance_data = {
    //     .data_size = sizeof(SaciShapeInstance),
    //     .location = 3,
    //     .instance_data_structure = malloc(sizeof(SaciShapeInstance)), // TODO, this needs to be prefilled, perhaps be stored as a static var
    // };
    // memcpy(instance_data.instance_data_structure, &instance, sizeof(SaciShapeInstance));
    // if (!saci_darray_push(saci_g_contextn.shape_instance_data_array[SACI_SHAPE_CUBE].instance_data_array,
    //                       &instance_data)) {
    //     SACI_LOG_ERROR_M(
    //         SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
    //         SACI_LOG_CONTEXT_MAIN_SHAPES_DRAW, "Could not push cube transform");
    // }
}

/* === Internal Implementation === */

SACI_INTERNAL SaciMat4 saci__mat4_create_transform_from_flag(
    struct SaciTransform transform,
    SaciU64 flag_var)
{
    SaciMat4 transform_mat = SACI_G_IDENTITY_MAT4;
    if (!SACI_HAS_FLAG_M(flag_var, SACI_MODEL_FLAG_ROTATION_RTS)) { // Most likelly
        transform_mat = saci_mat4_model_matrix_trs(
            transform.pos,
            transform.rotation,
            transform.dimentions);
    } else {
        transform_mat = saci_mat4_model_matrix_rts(
            transform.pos,
            transform.rotation,
            transform.dimentions);
    }
    return transform_mat;
}
