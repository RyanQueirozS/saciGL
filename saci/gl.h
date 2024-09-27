// NOTE: This is an abstraction layer for default OpenGL. It can be used
// alongside saci.h, but it's not recommended due to the added complexity.
// saci.h simplifies these steps. Use saci.c as the standard, and this only
// as an exception.

#ifndef __SACI_saciGL__H__
#define __SACI_saciGL__H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

//----------------------------------------------------------------------------//

typedef GLFWmonitor saciGL_Monitor;
typedef GLFWwindow saciGL_Window;

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

bool saciGL_GLFWInit(void);
bool saciGL_GLEWInit(void);

// Creates a window, does not check if it's null
saciGL_Window* saciGL_CreateWindow(int width, int height, const char* title,
                                   saciGL_Monitor* monitor, saciGL_Window* share);
void saciGL_MakeWindowContext(saciGL_Window* window);

typedef void (*saciGL__WindowPosHandler)(saciGL_Window* window, int width, int height);
void saciGL_SetWindowPosHandler(saciGL_Window* window, saciGL__WindowPosHandler windowPosHandler);
typedef void (*saciGL__WindowSizeHandler)(saciGL_Window* window, int posx, int posy);
void saciGL_SetWindowSizeHandler(saciGL_Window* window, saciGL__WindowSizeHandler windowSizeHandler);

void saciGL_Terminate(void);

//----------------------------------------------------------------------------//
// Shadering
//----------------------------------------------------------------------------//

saci_u32 saciGL_CompileShaderV(const char* source);
saci_u32 saciGL_CompileShaderF(const char* source);
saci_u32 saciGL_CompileShaderG(const char* source);

saci_u32 saciGL_GetShaderProgram(saci_u32 vshader, saci_u32 fshader);
saci_u32 saciGL_GetShaderProgramg(saci_u32 vshader, saci_u32 fshader, saci_u32 gshader);

//----------------------------------------------------------------------------//
// Render
//----------------------------------------------------------------------------//

typedef struct saciGL_Renderer saciGL_Renderer;

saciGL_Renderer* saciGL_CreateRenderer(void);
void saciGL_DeleteRenderer(saciGL_Renderer* renderer);

void saciGL_RenderBegin(saciGL_Renderer* renderer);
void saciGL_RenderEnd(saciGL_Renderer* renderer);

// Renderer config
void saciGL_RenderSetNoFillMode(void);
void saciGL_RenderSetFillMode(void);

// Triangles
void saciGL_RenderPushTriangle(saciGL_Renderer* renderer,
                               const Vec2 a, const Vec2 b, const Vec2 c,
                               const Color aColor, const Color bColor, const Color cColor);

//----------------------------------------------------------------------------//
// Draw
//----------------------------------------------------------------------------//

void saciGL_ClearBackground(const Color color);

void saciGL_PresentDrawing(saciGL_Window* window);

//----------------------------------------------------------------------------//
// Event
//----------------------------------------------------------------------------//

void saciGL__PollEvents();
void saciGL__WaitForEvents();
void saciGL__WaitForEventsTimeout(double timeout);
void saciGL__PostEmptyEvent();

typedef void (*saciGL__MousePosHandlerFunction)(saciGL_Window* window, double posx, double posy);
void saciGL_SetMousePosHandler(saciGL_Window* window, saciGL__MousePosHandlerFunction mousePosHandlerFunction);

typedef enum saciGL_Keycode { // view https://www.glfw.org/docs/3.3/group__keys.html
    SACI_KEY_SPACE = 32,
    SACI_KEY_APOSTROPHE = 39, /* ' */
    SACI_KEY_COMMA = 44,      /* , */
    SACI_KEY_MINUS = 45,      /* - */
    SACI_KEY_PERIOD = 46,     /* . */
    SACI_KEY_SLASH = 47,      /* / */
    SACI_KEY_0 = 48,
    SACI_KEY_1 = 49,
    SACI_KEY_2 = 50,
    SACI_KEY_3 = 51,
    SACI_KEY_4 = 52,
    SACI_KEY_5 = 53,
    SACI_KEY_6 = 54,
    SACI_KEY_7 = 55,
    SACI_KEY_8 = 56,
    SACI_KEY_9 = 57,
    SACI_KEY_SEMICOLON = 59, /* ; */
    SACI_KEY_EQUAL = 61,     /* = */
    SACI_KEY_A = 65,
    SACI_KEY_B = 66,
    SACI_KEY_C = 67,
    SACI_KEY_D = 68,
    SACI_KEY_E = 69,
    SACI_KEY_F = 70,
    SACI_KEY_G = 71,
    SACI_KEY_H = 72,
    SACI_KEY_I = 73,
    SACI_KEY_J = 74,
    SACI_KEY_K = 75,
    SACI_KEY_L = 76,
    SACI_KEY_M = 77,
    SACI_KEY_N = 78,
    SACI_KEY_O = 79,
    SACI_KEY_P = 80,
    SACI_KEY_Q = 81,
    SACI_KEY_R = 82,
    SACI_KEY_S = 83,
    SACI_KEY_T = 84,
    SACI_KEY_U = 85,
    SACI_KEY_V = 86,
    SACI_KEY_W = 87,
    SACI_KEY_X = 88,
    SACI_KEY_Y = 89,
    SACI_KEY_Z = 90,
    SACI_KEY_LEFT_BRACKET = 91,  /* [ */
    SACI_KEY_BACKSLASH = 92,     /* \ */
    SACI_KEY_RIGHT_BRACKET = 93, /* ] */
    SACI_KEY_GRAVE_ACCENT = 96,  /* ` */
    SACI_KEY_WORLD_1 = 161,      /* non-US #1 */
    SACI_KEY_WORLD_2 = 162,      /* non-US #2 */
    SACI_KEY_ESCAPE = 256,
    SACI_KEY_ENTER = 257,
    SACI_KEY_TAB = 258,
    SACI_KEY_BACKSPACE = 259,
    SACI_KEY_INSERT = 260,
    SACI_KEY_DELETE = 261,
    SACI_KEY_RIGHT = 262,
    SACI_KEY_LEFT = 263,
    SACI_KEY_DOWN = 264,
    SACI_KEY_UP = 265,
    SACI_KEY_PAGE_UP = 266,
    SACI_KEY_PAGE_DOWN = 267,
    SACI_KEY_HOME = 268,
    SACI_KEY_END = 269,
    SACI_KEY_CAPS_LOCK = 280,
    SACI_KEY_SCROLL_LOCK = 281,
    SACI_KEY_NUM_LOCK = 282,
    SACI_KEY_PRINT_SCREEN = 283,
    SACI_KEY_PAUSE = 284,
    SACI_KEY_F1 = 290,
    SACI_KEY_F2 = 291,
    SACI_KEY_F3 = 292,
    SACI_KEY_F4 = 293,
    SACI_KEY_F5 = 294,
    SACI_KEY_F6 = 295,
    SACI_KEY_F7 = 296,
    SACI_KEY_F8 = 297,
    SACI_KEY_F9 = 298,
    SACI_KEY_F10 = 299,
    SACI_KEY_F11 = 300,
    SACI_KEY_F12 = 301,
    SACI_KEY_F13 = 302,
    SACI_KEY_F14 = 303,
    SACI_KEY_F15 = 304,
    SACI_KEY_F16 = 305,
    SACI_KEY_F17 = 306,
    SACI_KEY_F18 = 307,
    SACI_KEY_F19 = 308,
    SACI_KEY_F20 = 309,
    SACI_KEY_F21 = 310,
    SACI_KEY_F22 = 311,
    SACI_KEY_F23 = 312,
    SACI_KEY_F24 = 313,
    SACI_KEY_F25 = 314,
    SACI_KEY_KP_0 = 320,
    SACI_KEY_KP_1 = 321,
    SACI_KEY_KP_2 = 322,
    SACI_KEY_KP_3 = 323,
    SACI_KEY_KP_4 = 324,
    SACI_KEY_KP_5 = 325,
    SACI_KEY_KP_6 = 326,
    SACI_KEY_KP_7 = 327,
    SACI_KEY_KP_8 = 328,
    SACI_KEY_KP_9 = 329,
    SACI_KEY_KP_DECIMAL = 330,
    SACI_KEY_KP_DIVIDE = 331,
    SACI_KEY_KP_MULTIPLY = 332,
    SACI_KEY_KP_SUBTRACT = 333,
    SACI_KEY_KP_ADD = 334,
    SACI_KEY_KP_ENTER = 335,
    SACI_KEY_KP_EQUAL = 336,
    SACI_KEY_LEFT_SHIFT = 340,
    SACI_KEY_LEFT_CONTROL = 341,
    SACI_KEY_LEFT_ALT = 342,
    SACI_KEY_LEFT_SUPER = 343,
    SACI_KEY_RIGHT_SHIFT = 344,
    SACI_KEY_RIGHT_CONTROL = 345,
    SACI_KEY_RIGHT_ALT = 346,
    SACI_KEY_RIGHT_SUPER = 347,
    SACI_KEY_MENU = 348,
    SACI_KEY_LAST = SACI_KEY_MENU,
} saciGL_Keycode;
// returns true if last key state was "pressed"
bool saciGL_IsKeyPressed(saciGL_Window* window, saciGL_Keycode keycode);

#endif
