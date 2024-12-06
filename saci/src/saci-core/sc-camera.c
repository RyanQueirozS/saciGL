#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

struct sc_Camera_c sc_Camera_Get_Default() {
    struct sc_Camera_c camera;
    camera.position = (sa_Vec3_t){0.0f, 0.0f, 0.0f};
    camera.target = (sa_Vec3_t){0.0f, 0.0f, 0.0f};
    camera.up = (sa_Vec3_t){0.0f, 1.0f, 0.0f};

    camera.fov = 45.0f;
    camera.aspectRatio = 1600.0f / 900.0f;
    camera.near = 0.1f;
    camera.far = 100.0f;

    return camera;
}
