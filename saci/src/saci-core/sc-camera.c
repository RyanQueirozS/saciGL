#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

struct sc_Camera_c sc_Camera_Get_Default() {
    struct sc_Camera_c camera;
    camera.m_position = sa_SCAST_TO_m(sa_Vec3_t){0.0f, 0.0f, 0.0f};
    camera.m_target = sa_SCAST_TO_m(sa_Vec3_t){0.0f, 0.0f, 0.0f};
    camera.m_up = sa_SCAST_TO_m(sa_Vec3_t){0.0f, 1.0f, 0.0f};

    camera.m_fov = 45.0f;
    camera.m_aspect_ratio = 1600.0f / 900.0f;
    camera.m_near = 0.1f;
    camera.m_far = 100.0f;

    return camera;
}
