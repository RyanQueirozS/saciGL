#include "saci/saci.h"

#include "saci_util/math.h"
#include "saci_util/types.h"

#include <stdio.h>

static SaciVec3 rotation = {0.02, 0.02, 0.02};
static SaciVec3 rotation1 = {0.03, 0.01, 0.06};
static SaciVec3 rotation2 = {0.001, 0.02, 0.03};

static SaciCube cube_middle = {
    .dimentions = (SaciVec3){1, 1, 1},
    .pos_center = (SaciVec3){1, 0, 0},
    .rotation = (SaciVec3){0, 0, 0},
    .fill_color = (SaciColor){1, 0, 0, 1},
    .flags = 0,
};

static SaciCube cube_small_left = {
    .dimentions = (SaciVec3){0.5, 0.5, 0.5},
    .pos_center = (SaciVec3){8, 3, 0},
    .rotation = (SaciVec3){0, 0, 0},
    .fill_color = (SaciColor){0, 0, 1, 1},
    .flags = SACI_MODEL_FLAG_ROTATION_RTS,
};

static SaciCube cube_big_orbiting = {
    .dimentions = (SaciVec3){2, 2, 2},
    .pos_center = (SaciVec3){5, 0, 0},
    .rotation = (SaciVec3){0, 0, 0},
    .fill_color = (SaciColor){0, 1, 0, 1},
    .flags = 0,
};

void loop_func(SaciLoopFrameData frame_data) {
    saci_begin();

    cube_middle.rotation = saci_vec3_add(cube_middle.rotation, rotation);
    cube_small_left.rotation = saci_vec3_add(cube_small_left.rotation, rotation1);
    cube_big_orbiting.rotation = saci_vec3_add(cube_big_orbiting.rotation, rotation2);

    saci_draw_cube(cube_middle);

    saci_draw_cube(cube_small_left);

    saci_draw_cube(cube_big_orbiting);

    saci_present();
}

int main(void) {
    saci_init();
    saci_set_background_color(saci_color_from_u8(25, 70, 125, 255));

    saci_set_loop_func(loop_func);

    saci_loop();

    saci_free();
}
