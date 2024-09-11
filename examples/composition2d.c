#include "../saci/core.h"
#include "../saci/shapes.h"

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_VERTICES_RECTANGLE 12
#define MAX_INDICES_RECTANGLE 6

int main(void) {
    saciWindowSpecs windowSpecs = {700, 700, "title", NULL, NULL};

    saciWindow* window = saciCreateWindow(windowSpecs);
    assert(window != NULL);

    scglRenderer* renderer = scglCreateRenderer();

    saci_Rect rect = {50, 50, 20, 30};

    color bgColor = Color(0, 0, 0, 1);
    saciSetCanvasColor(bgColor);
    while (!glfwWindowShouldClose(window)) {
        saciBeginComposition();

        saciEndComposition();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    saciTerminate();

    return 0;
}
