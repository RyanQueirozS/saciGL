#include "../saci/scgl.h"

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_VERTICES_RECTANGLE 12
#define MAX_INDICES_RECTANGLE 6

int main(void) {
    scglGLFWInit();

    scglWindow* window = scglCreateWindow(700, 700, "title", NULL, NULL);
    scglMakeWindowContext(window);
    assert(window != NULL);

    scglGLEWInit();

    scglRenderer* renderer = scglCreateRenderer();

    saciRect rect = {-0, -0.8, 0.3, 0.3};

    color bgColor = Color(0, 0, 0, 1);
    color fgColor = Color(1, 0, 0, 1);

    while (!glfwWindowShouldClose(window)) {
        scglClearBackground(bgColor);
        scglBeginComposition(renderer);

        scglRenderDrawRect(renderer, rect, fgColor);

        scglRenderComposition(renderer);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scglDeleteRenderer(renderer);

    return 0;
}
