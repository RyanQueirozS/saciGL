#ifndef __SACI_CORE_SC_CAMERA_H__
#define __SACI_CORE_SC_CAMERA_H__

#include "saci-utils/su-types.h"

struct sc_camera_c {
    sa_vec3_t m_position;
    sa_vec3_t m_target;
    sa_vec3_t m_up;

    float m_fov;
    float m_aspect_ratio;
    float m_near;
    float m_far;
};

struct sc_camera_c sc_Camera_Get_Default(void);

#endif
