#include "saci.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "sacigl-windowing.h"
#include "sacigl.h"

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __saci_defaultedMousePosHandler(saci_Window* window, double posx, double posy);
void __saci_defaultedWindowPosHandler(saci_Window* window, int posx, int posy);
void __saci_defaultedWindowSizeHandler(saci_Window* window, int width, int height);

//----------------------------------------------------------------------------//

static struct saciCompositor {
    struct {
        saciGL_Renderer* renderer;
    } Render;
    struct {
        saci_Color canvasColor;
        struct {
            saciGL_Window* window;
            int width;
            int height;
            int x;
            int y;
        } Window;
    } Canvas;
    struct {
        saci_Vec3 pos;
        saci_Vec3 angleRad;
        saci_Vec3 target;
        saci_CameraMovementMode cameraMode;
    } Camera;
    struct {
        struct {
            double x;
            double y;
        } Mouse;
        struct {
        } Keyboard;
        struct {
        } Controller;
    } Input;
} saci_compositor;

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

void saci_InitWindow(int width, int height, const char* title) {
    assert(saciGL_GLFWInit());
    printf("INFO: Initialized GLFW\n");

    saci_compositor.Canvas.Window.window = saciGL_CreateWindow(width, height, title,
                                                               NULL, NULL);
    saci_compositor.Canvas.Window.width = width;
    saci_compositor.Canvas.Window.height = height;
    assert(saci_compositor.Canvas.Window.window != NULL);
    printf("INFO: Created window\n");
    saciGL_MakeWindowContext(saci_compositor.Canvas.Window.window);
    printf("INFO: Made window context\n");

    assert(saciGL_GLEWInit());
    printf("INFO: Initialized GLEW\n");
}

void saci_Terminate(void) {
    saciGL_Terminate();
    saciGL_DeleteRenderer(saci_compositor.Render.renderer);
}

bool saci_WindowShouldClose() {
    return glfwWindowShouldClose(saci_compositor.Canvas.Window.window);
}

//----------------------------------------------------------------------------//
// Composition
//----------------------------------------------------------------------------//

void saci_InitCompositor() {
    saci_compositor.Render.renderer = saciGL_CreateRenderer();
    {
        saciGL_SetMousePosHandler(saci_compositor.Canvas.Window.window, __saci_defaultedMousePosHandler);
        saciGL_SetWindowPosHandler(saci_compositor.Canvas.Window.window, __saci_defaultedWindowPosHandler);
        saciGL_SetWindowSizeHandler(saci_compositor.Canvas.Window.window, __saci_defaultedWindowSizeHandler);
    }
}

void saci_SetCanvasColor(const saci_Color color) {
    saci_compositor.Canvas.canvasColor = color;
}

void saci_BeginComposition() {
    saciGL_ClearWindow(saci_compositor.Canvas.canvasColor);
    saciGL_RenderBegin(saci_compositor.Render.renderer);
}

void saci_EndComposition() {
    saciGL_RenderEnd(saci_compositor.Render.renderer);
    saciGL_SwapWindowBuffer(saci_compositor.Canvas.Window.window);
    saciGL__PollEvents();
}

void saci_ComposeTriangle(const saciTri triangle, const saci_Color fillColor) {
    saciGL_RenderPushTriangle(saci_compositor.Render.renderer, triangle.a, triangle.b, triangle.c, fillColor, fillColor, fillColor);
}

void saci_ComposeRect(const saciRect rect, const saci_Color fillColor) {
    saci_Vec2 a = {rect.pos.x, rect.pos.y};
    saci_Vec2 b = {rect.pos.x + rect.width, rect.pos.y};
    saci_Vec2 c = {rect.pos.x + rect.width, rect.pos.y + rect.height};
    saci_Vec2 d = {rect.pos.x, rect.pos.y + rect.height};
    saciGL_RenderPushTriangle(saci_compositor.Render.renderer,
                              a, b, c,
                              fillColor, fillColor, fillColor);
    saciGL_RenderPushTriangle(saci_compositor.Render.renderer,
                              a, c, d,
                              fillColor, fillColor, fillColor);
}

void saci_ComposeQuad(const saciQuad quad, const saci_Color fillColor) {
    saciGL_RenderPushTriangle(saci_compositor.Render.renderer,
                              quad.a, quad.b, quad.d,
                              fillColor, fillColor, fillColor);
    saciGL_RenderPushTriangle(saci_compositor.Render.renderer,
                              quad.b, quad.d, quad.c,
                              fillColor, fillColor, fillColor);
}

void saci_ComposeCuboid(const saciCuboid cuboid, const saci_Color fillColor) {
    printf("TODO\n");
}

//----------------------------------------------------------------------------//
// Camera
//----------------------------------------------------------------------------//

void saci_SetCameraMode(saci_CameraMovementMode cameraMode) {
    saci_compositor.Camera.cameraMode = cameraMode;
}

void saci_SetCameraPos(saci_Vec3 newPos) {
    saci_compositor.Camera.pos = newPos;
}

void saci_SetCameraAngle(saci_Vec3 angleRad) {
    saci_compositor.Camera.angleRad = angleRad;
}

void saci_SetCameraTarget(saci_Vec3 lookingAtPos) {
    saci_compositor.Camera.target = lookingAtPos;
}

saci_Vec3 saci_GetCameraPos() {
    return saci_compositor.Camera.pos;
}

saci_Vec3 saci_GetCameraAngleRad() {
    return saci_compositor.Camera.angleRad;
}

//----------------------------------------------------------------------------//
// Event
//----------------------------------------------------------------------------//

saci_Vec2 saci_GetMousePos() {
    return (saci_Vec2){.x = saci_compositor.Input.Mouse.x,
                       .y = saci_compositor.Input.Mouse.y};
}

bool saci_IsKeyPressed(saci_Keycode keycode) {
    return saciGL_IsKeyPressed(saci_compositor.Canvas.Window.window, keycode);
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __saci_defaultedMousePosHandler(saci_Window* window, double posx, double posy) {
    SACI_SCAST_TO(void)
    (window);
    saci_compositor.Input.Mouse.x = posx;
    saci_compositor.Input.Mouse.y = posy;
}

void __saci_defaultedWindowPosHandler(saci_Window* window, int posx, int posy) {
    SACI_SCAST_TO(void)
    (window);
    saci_compositor.Canvas.Window.x = posx;
    saci_compositor.Canvas.Window.y = posy;
}

void __saci_defaultedWindowSizeHandler(saci_Window* window, int width, int height) {
    SACI_SCAST_TO(void)
    (window);
    saci_compositor.Canvas.Window.width = width;
    saci_compositor.Canvas.Window.height = height;
}
