#include "saci-main/saci.h"
#include "saci-utils/su-math.h"

static su_vec3 rotation = {0.02, 0.02, 0.02};
static su_vec3 rotation1 = {0.03, 0.01, 0.06};
static su_vec3 rotation2 = {0.001, 0.02, 0.03};

static saci_cube cube_middle = {
    .dimentions = (su_vec3){1, 1, 1},
    .pos_center = (su_vec3){0, 0, 0},
    .rotation = (su_vec3){0, 0, 0},
    .color = (su_color){1, 0, 0, 1},
};

static saci_cube cube_small_left = {
    .dimentions = (su_vec3){0.75, 0.5, 0.5},
    .pos_center = (su_vec3){8, 3, 0},
    .rotation = (su_vec3){0, 0, 0},
    .color = (su_color){0, 1, 0, 1},
};

static saci_cube cube_big_orbiting = {
    .dimentions = (su_vec3){2, 2, 3},
    .pos_center = (su_vec3){5, 0, 0},
    .rotation = (su_vec3){0, 0, 0},
    .color = (su_color){0, 1, 0, 1},
};

void Draw_Fps(void) {
#if 0
    double fps = saci_Get_Fps();
    char formatted_str[50]; 

    snprintf(formatted_str, sizeof(formatted_str), "FPS: %.6f", fps);

    saci_Draw_Text(font_id, formatted_str);
#endif
}

void Loop_Func(double delta) {
    saci_Begin();

    cube_middle.rotation = su_Vec3_Add(cube_middle.rotation, rotation);
    cube_small_left.rotation = su_Vec3_Add(cube_middle.rotation, rotation1);
    cube_big_orbiting.rotation = su_Vec3_Add(cube_middle.rotation, rotation2);

    saci_Enable(saci_RENDERING_FLAG_ROTATION_RTS, su_FALSE); // default rotation
    saci_Draw_Cube(cube_middle);

    saci_Draw_Cube(cube_small_left);

    saci_Enable(saci_RENDERING_FLAG_ROTATION_RTS, su_TRUE);
    saci_Draw_Cube(cube_big_orbiting);

    saci_Present();
}

int main(void) {
    saci_Init();
    saci_Set_Background_Color(su_Color_From_U8(25, 70, 125, 255));

    saci_Set_Loop_Func(Loop_Func);

    saci_Loop();

    saci_Free();
}
