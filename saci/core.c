#include "saci.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "gl.h"

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//
void __saci_defaultedMousePosHandler(saciWindow* window, double posx, double posy);

//----------------------------------------------------------------------------//

static struct saciCompositor {
    struct {
        scglRenderer* renderer;
    } Render;
    struct {
        Color canvasColor;
        scglWindow* window;
    } Canvas;
    struct {
        Vec3 pos;
        Vec3 angleRad;
        Vec3 target;
        saciCameraMovementMode cameraMode;
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
} saciCompositor;

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

void saciInitWindow(const saciWindowProperties windowProperties) {
    assert(scglGLFWInit());
    printf("INFO: Initialized GLFW\n");

    saciCompositor.Canvas.window = scglCreateWindow(windowProperties.width, windowProperties.height, windowProperties.title,
                                                    NULL, NULL);
    assert(saciCompositor.Canvas.window != NULL);
    printf("INFO: Created window\n");
    scglMakeWindowContext(saciCompositor.Canvas.window);
    printf("INFO: Made window context\n");

    assert(scglGLEWInit());
    printf("INFO: Initialized GLEW\n");
}

void saciTerminate(void) {
    scglTerminate();
    scglDeleteRenderer(saciCompositor.Render.renderer);
}

bool saciWindowShouldClose() {
    return glfwWindowShouldClose(saciCompositor.Canvas.window);
}

//----------------------------------------------------------------------------//
// Composition
//----------------------------------------------------------------------------//

void saciInitCompositor() {
    saciCompositor.Render.renderer = scglCreateRenderer();
    scglSetMousePosHandler(saciCompositor.Canvas.window, __saci_defaultedMousePosHandler);
}

void saciSetCanvasColor(const Color color) {
    saciCompositor.Canvas.canvasColor = color;
}

void saciBeginComposition() {
    scglClearBackground(saciCompositor.Canvas.canvasColor);
    scglRenderBegin(saciCompositor.Render.renderer);
}

void saciEndComposition() {
    scglRenderEnd(saciCompositor.Render.renderer);
    scglPresentDrawing(saciCompositor.Canvas.window);
    scgl_PollEvents();
}

void saciComposeTriangle(const saciTri triangle, const Color fillColor) {
    scglRenderPushTriangle(saciCompositor.Render.renderer, triangle.a, triangle.b, triangle.c, fillColor, fillColor, fillColor);
}

void saciComposeRect(const saciRect rect, const Color fillColor) {
    Vec2 a = {rect.pos.x, rect.pos.y};
    Vec2 b = {rect.pos.x + rect.width, rect.pos.y};
    Vec2 c = {rect.pos.x + rect.width, rect.pos.y + rect.height};
    Vec2 d = {rect.pos.x, rect.pos.y + rect.height};
    scglRenderPushTriangle(saciCompositor.Render.renderer,
                           a, b, c,
                           fillColor, fillColor, fillColor);
    scglRenderPushTriangle(saciCompositor.Render.renderer,
                           a, c, d,
                           fillColor, fillColor, fillColor);
}

void saciComposeQuad(const saciQuad quad, const Color fillColor) {
    scglRenderPushTriangle(saciCompositor.Render.renderer,
                           quad.a, quad.b, quad.d,
                           fillColor, fillColor, fillColor);
    scglRenderPushTriangle(saciCompositor.Render.renderer,
                           quad.b, quad.d, quad.c,
                           fillColor, fillColor, fillColor);
}

void saciComposeCuboid(const saciCuboid cuboid, const Color fillColor) {
    printf("TODO\n");
}

//----------------------------------------------------------------------------//
// Camera
//----------------------------------------------------------------------------//

void saciSetCameraMode(saciCameraMovementMode cameraMode) {
    saciCompositor.Camera.cameraMode = cameraMode;
}

void saciSetCameraPos(Vec3 newPos) {
    saciCompositor.Camera.pos = newPos;
}

void saciSetCameraAngle(Vec3 angleRad) {
    saciCompositor.Camera.angleRad = angleRad;
}

void saciSetCameraTarget(Vec3 lookingAtPos) {
    saciCompositor.Camera.target = lookingAtPos;
}

Vec3 saciGetCameraPos() {
    return saciCompositor.Camera.pos;
}

Vec3 saciGetCameraAngleRad() {
    return saciCompositor.Camera.angleRad;
}

//----------------------------------------------------------------------------//
// Event
//----------------------------------------------------------------------------//

Vec2 saciGetMousePos() {
    return (Vec2){.x = saciCompositor.Input.Mouse.x,
                  .y = saciCompositor.Input.Mouse.y};
}

bool saciIsKeyPressed(saciKeycode keycode) {
    return scgl_IsKeyPressed(saciCompositor.Canvas.window, keycode);
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __saci_defaultedMousePosHandler(saciWindow* window, double posx, double posy) {
    SCAST_TO(void)
    (window);
    saciCompositor.Input.Mouse.x = posx;
    saciCompositor.Input.Mouse.y = posy;
}
