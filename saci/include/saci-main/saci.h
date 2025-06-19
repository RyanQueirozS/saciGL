#ifndef SACI_MAIN_SACI_H
#define SACI_MAIN_SACI_H

#include "saci-utils/su-general.h"

typedef struct saci_cube {
    sa_vec3 pos_center;
    sa_vec3 dimentions;
} saci_cube;

typedef void (*saci_loopFunc)(const double delta);

SA_API void saci_Init(void);

SA_API void saci_Begin(void);

SA_API void saci_Set_Loop_Func(saci_loopFunc loop_func);

SA_API void saci_Loop(void);

SA_API sa_vec3 saci_Translation_Rotation_Get(void);

SA_API void saci_Translation_Rotate(const sa_vec3 rotation);

SA_API void saci_Draw_Cube(const saci_cube cube);

SA_API void saci_Present(void);

SA_API void saci_Free(void);

#endif // SACI_MAIN_SACI_H
