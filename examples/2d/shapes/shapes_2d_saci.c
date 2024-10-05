// Implementation of basic shape drawing using SCGL(saci graphics library) and SACI

#include <saci-lib.h>
#include <saci-core.h>

bool shouldFillShape = true;

void handle_viewing_mode() {
    if (shouldFillShape) {
        saciGL_RenderSetFillMode();
    } else {
        saciGL_RenderSetNoFillMode();
    }
}

void toggle_viewing_mode() {
    if (saci_IsKeyPressed(SACI_KEY_SPACE)) {
        shouldFillShape = false;
    } else {
        shouldFillShape = true;
    }
    handle_viewing_mode();
}

int main() {
    saci_InitWindow(700, 700, "SACI Examples - Shapes2D");
    saci_InitCompositor();

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

    saci_Color bgColor = {0, 0, 0, 1};
    saci_Color fgColor = {0, 1, 0, 1};

    saci_Color triangleColors[4] = {
        fgColor,
        fgColor,
        fgColor,
        fgColor,
    };

    saci_SetCanvasColor(bgColor);
    while (!saci_WindowShouldClose()) {
        saci_BeginComposition();

        saci_ComposeRect(rect, fgColor);
        saci_ComposeQuad(quad, fgColor);
        saci_ComposeTriangle(triangleFixColor, fgColor);
        toggle_viewing_mode();

        saci_EndComposition();
    }

    saci_Terminate();
}
