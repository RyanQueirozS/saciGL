#ifndef SACI_UTILS_MATH_SU_MATH_MAT4_h
#define SACI_UTILS_MATH_SU_MATH_MAT4_h

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} su_Vec3;

typedef struct {
    float data[4][4]; /**< 4x4 matrix of floats */
} su_Mat4;

typedef struct {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} su_Color;

#endif // SACI_UTILS_MATH_SU_MATH_MAT4_h
