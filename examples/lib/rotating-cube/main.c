// TODO, this is a prototype on how saci-lib should work.
#include "saci.h"

int main(void) {
    saci_Init();

    saci_Cube cube_opts;
    while (!saci_Should_Close()) {
        saci_Begin();

        saci_Draw_Mode(SACI_DRAW_MODE_INSTANCED);
        saci_Uniform_Translation_Rotate(rotation1);
        saci_Draw_Cube(cube_opts);
        saci_Uniform_Translation_Rotate(rotation2);
        saci_Draw_Cube(cube_opts);
        saci_Uniform_Translation_Rotate(rotation3);
        saci_Draw_Cube(cube_opts);

        saci_Draw_Present();
    }
    saci_Free();
}
