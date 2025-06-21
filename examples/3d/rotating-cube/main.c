#include "saci-main/saci.h"
#include "saci-utils/su-math.h"

#include <float.h>
#include <stddef.h>
#include <assert.h>

static su_vec3 rotation = {0};

static saci_cube cube = {
    .dimentions = (su_vec3){1, 1, 1},
    .pos_center = (su_vec3){0, 0, 0},
    .rotation = (su_vec3){0, 0, 0},
};

void Draw_Fps(void) {
#if 0
    double fps = saci_Get_Fps();
    char formatted_str[50];  // Buffer to store the formatted string

    // Safely format the FPS value into the string
    snprintf(formatted_str, sizeof(formatted_str), "FPS: %.6f", fps);

    saci_Draw_Text(font_id, formatted_str);
#endif
}

void Loop_Func(double delta) {
    saci_Begin();

    cube.rotation = su_Vec3_Add(cube.rotation, rotation);

    saci_Draw_Cube(cube);

    saci_Present();
}

int main(void) {
    saci_Init();

    saci_Set_Loop_Func(Loop_Func);

    saci_Loop();

    saci_Free();
}
