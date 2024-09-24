// Implementation of basic shape drawing using SCGL(saci graphics library)

#include <saci.h>
#include <gl.h>

bool shouldFillShape = true;

void handle_viewing_mode() {
    if (shouldFillShape) {
        scglRenderSetFillMode();
    } else {
        scglRenderSetNoFillMode();
    }
}

void toggle_viewing_mode() {
    if (saciIsKeyPressed(SACI_KEY_SPACE)) {
        shouldFillShape = false;
    } else {
        shouldFillShape = true;
    }
    handle_viewing_mode();
}

int main() {
    saciWindowProperties properties = {700, 700, "SACI Examples - Shapes2D"};

    saciInitWindow(properties);
    saciInitCompositor();

    saciRect rect = {
        -0,
        -0.8,
        0.3,
        0.3,
    };

    saciTri triangleFixColor = {
        {1, 0.5},
        {0.9, 0.9},
        {0.1, 0.2},
    };

    saciQuad quad = {
        {0.0, 1.0},  // Top
        {0.5, 0.0},  // Right
        {0.0, -1.0}, // Bottom
        {-0.5, 0.0}, // Left
    };

    Color bgColor = {0, 0, 0, 1};
    Color fgColor = {0, 1, 0, 1};

    Color triangleColors[4] = {
        fgColor,
        fgColor,
        fgColor,
        fgColor,
    };

    saciSetCanvasColor(bgColor);
    while (!saciWindowShouldClose()) {
        saciBeginComposition();

        saciComposeRect(rect, fgColor);
        saciComposeQuad(quad, fgColor);
        saciComposeTriangle(triangleFixColor, fgColor);
        toggle_viewing_mode();

        saciEndComposition();
    }

    saciTerminate();
}
