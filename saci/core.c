#include "core.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "scgl.h"

//----------------------------------------------------------------------------//

saciWindow* saciCreateWindow(const saciWindowSpecs windowSpecs) {
    assert(scglGLFWInit() != -1);
    printf("INFO: Initialized GLFW\n");

    saciWindow* window = scglCreateWindow(windowSpecs.width, windowSpecs.height,
                                          windowSpecs.title,
                                          windowSpecs.monitor, windowSpecs.windowShare);
    assert(window != NULL);
    printf("INFO: Created window\n");
    scglMakeWindowContext(window);
    printf("INFO: Made window context\n");

    assert(scglGLEWInit() != -1);
    printf("INFO: Initialized GLEW\n");

    return window;
}

void saciTerminate(void) {
    // TODO remove use glfw
    glfwTerminate();
}

//----------------------------------------------------------------------------//
// Canvas
static struct saciComposition {
    color canvasColor;
} saciComposition;

void saciSetCanvasColor(const color color) {
    saciComposition.canvasColor = color;
}

void saciBeginComposition() {
    scglClearBackground(saciComposition.canvasColor);
}

void saciEndComposition() {
}
