#ifndef __SACI_UTILS_SU_MATH_H__
#define __SACI_UTILS_SU_MATH_H__

#include "saci-utils/su-types.h"

#define SACI_PI 3.141592653589793f

#define SACI_DEG2RAD_M (SACI_PI / 180.0f)
#define SACI_DEG2RAD(x) ((x) * SACI_DEG2RAD_M)

#define SACI_RAD2DEG_M (180.0f / SACI_PI)
#define SACI_RAD2DEG(x) ((x) * SACI_RAD2DEG_M)

//------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------

void saci_InitMath();

//------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------

saci_Vec3 saci_SubtractVec3(saci_Vec3 a, saci_Vec3 b);

saci_Vec3 saci_NormalizeVec3(saci_Vec3 v);

saci_Vec3 saci_CrossVec3(saci_Vec3 a, saci_Vec3 b);

float saci_DotVec3(saci_Vec3 a, saci_Vec3 b);

//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------

saci_Mat4 saci_MultiplyMat4(saci_Mat4 a, saci_Mat4 b);

saci_Mat4 saci_IdentityMat4();

saci_Mat4 saci_LookAtMat4(saci_Vec3 eye, saci_Vec3 center, saci_Vec3 up);

saci_Mat4 saci_PerspectiveMat4(float fov, float aspect, float near, float far);

#endif
