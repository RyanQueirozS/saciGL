#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

sc_Camera sc_Camera_Get_Default() {
    sc_Camera camera;
    camera.position = (saci_Vec3){0.0f, 0.0f, 0.0f};
    camera.target = (saci_Vec3){0.0f, 0.0f, 0.0f};
    camera.up = (saci_Vec3){0.0f, 1.0f, 0.0f};

    camera.fov = 45.0f;
    camera.aspectRatio = 1600.0f / 900.0f;
    camera.near = 0.1f;
    camera.far = 100.0f;

    return camera;
}
