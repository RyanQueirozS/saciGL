#include "../saci/scgl.h"

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(void) {
    srand(time(0));
    scglGLFWInit();

    scglWindow* window = scglCreateWindow(700, 700, "title", NULL, NULL);
    scglMakeWindowContext(window);
    assert(window != NULL);

    scglGLEWInit();

    scglRenderer* renderer = scglCreateRenderer();

    saciRect rect = {-0, -0.8, 0.3, 0.3};

    // debug
    scglRenderSetFillMode();
    // scglRenderSetNoFillMode();

    vec2 triangleFixColor[3] = {
        Vec2(1, 0.5),
        Vec2(0.9, 0.9),
        Vec2(0.1, 0.2),
    };

    color bgColor = Color(0, 0, 0, 1);
    color fgColor = Color(0, 1, 0, 1);

    while (!glfwWindowShouldClose(window)) {
        scglClearBackground(bgColor);
        scglBeginComposition(renderer);

        scglRenderDrawRect(renderer, rect, fgColor);
        scglRenderDrawTriangleOneColor(renderer, triangleFixColor[0], triangleFixColor[1], triangleFixColor[2], fgColor);

        scglRenderComposition(renderer);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scglDeleteRenderer(renderer);

    return 0;
}
