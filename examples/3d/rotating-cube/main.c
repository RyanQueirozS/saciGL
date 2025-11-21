#include "saci-main/saci.h"
#include "saci-utils/math/su-math-color.h"

#include "saci-utils/math/su-math-vec.h"

static su_Vec3 rotation = {0.02, 0.02, 0.02};
static su_Vec3 rotation1 = {0.03, 0.01, 0.06};
static su_Vec3 rotation2 = {0.001, 0.02, 0.03};

static saci_Cube cube_middle = {
    .dimentions = (su_Vec3){1, 1, 1},
    .pos_center = (su_Vec3){0, 0, 0},
    .rotation = (su_Vec3){0, 0, 0},
    .color = (su_Color){1, 0, 0, 1},
};

static saci_Cube cube_small_left = {
    .dimentions = (su_Vec3){0.75, 0.5, 0.5},
    .pos_center = (su_Vec3){8, 3, 0},
    .rotation = (su_Vec3){0, 0, 0},
    .color = (su_Color){0, 0, 1, 1},
};

static saci_Cube cube_big_orbiting = {
    .dimentions = (su_Vec3){2, 2, 3},
    .pos_center = (su_Vec3){5, 0, 0},
    .rotation = (su_Vec3){0, 0, 0},
    .color = (su_Color){0, 1, 0, 1},
};

void loop_func(double delta) {
    saci_begin();

    cube_middle.rotation = su_vec3_add(cube_middle.rotation, rotation);
    cube_small_left.rotation = su_vec3_add(cube_middle.rotation, rotation1);
    cube_big_orbiting.rotation = su_vec3_add(cube_middle.rotation, rotation2);

    saci_enable(saci_RENDERING_FLAG_ROTATION_RTS, su_FALSE); // default rotation
    saci_draw_cube(cube_middle);

    saci_draw_cube(cube_small_left);

    saci_enable(saci_RENDERING_FLAG_ROTATION_RTS, su_TRUE);
    saci_draw_cube(cube_big_orbiting);

    saci_present();
}

int main(void) {
    saci_init();
    saci_set_background_color(su_color_from_u8(25, 70, 125, 255));

    saci_set_loop_func(loop_func);

    saci_loop();

    saci_free();
}
