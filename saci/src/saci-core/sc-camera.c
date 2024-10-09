#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

sc_Camera sc_GenerateDefaultCamera3D() {
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

sc_Camera sc_GenerateDefaultCamera2D() {
    sc_Camera camera;
    camera.position = (saci_Vec3){0.0f, 0.0f, 1.0f}; // This Z=1.0f is to position
                                                     // the camera "behind" the 2D
                                                     // rendering layer(aka 0),
                                                     // perhaps this will change
                                                     // later, but I'm not so
                                                     // sure how to improve it now
    camera.target = (saci_Vec3){0.0f, 0.0f, 0.0f};
    camera.up = (saci_Vec3){0.0f, 1.0f, 0.0f};

    camera.fov = 45.0f;
    camera.aspectRatio = 1600.0f / 900.0f;
    camera.near = 0.1f;
    camera.far = 100.0f;

    return camera;
}
