#ifndef SACI_BACKEND_WINDOWING_SC_WINDOWING_H
#define SACI_BACKEND_WINDOWING_SC_WINDOWING_H

#include "saci_util/defines.h"
#include "saci_util/types.h"
#include "./windowing-common.h"

SaciBool psaci_window_load(void);
union psaciWindow psaci_window_create(int width, int height, const char* title,
                                      union PSaciWindowOpts opts);
SACI_API void psaci_window_free(union PSaciWindow window);
SACI_API void psaci_window_make_context(union PSaciWindow window);
SACI_API SaciBool psaci_window_should_close(union PSaciWindow window);
SACI_API void psaci_window_set_pos_handler(union PSaciWindow window, PSaciWindowSizeHandler window_pos_handler);
SACI_API void psaci_window_set_size_handler(union PSaciWindow window, PSaciWindowSizeHandler window_size_handler);
SACI_API void psaci_window_terminate(void);
SACI_API void psaci_window_swap_buffer(union PSaciWindow window);

/* === Event === */

// TODO GLFW_KEY_LAST
#define PSACI_EVENT_KEY_AMOUNT 1000

#define PSACI_EVENT_MOUSE_BUTTON_AMOUNT 8

#define PSACI_EVENT_CONTROLLER_BUTTON_AMOUNT 16

enum PSaciKey {
    PSACI_KEY_SPACE = 32,
    PSACI_KEY_APOSTROPHE = 39, /* ' */
    PSACI_KEY_COMMA = 44,      /* , */
    PSACI_KEY_MINUS = 45,      /* - */
    PSACI_KEY_PERIOD = 46,     /* . */
    PSACI_KEY_SLASH = 47,      /* / */
    PSACI_KEY_0 = 48,
    PSACI_KEY_1 = 49,
    PSACI_KEY_2 = 50,
    PSACI_KEY_3 = 51,
    PSACI_KEY_4 = 52,
    PSACI_KEY_5 = 53,
    PSACI_KEY_6 = 54,
    PSACI_KEY_7 = 55,
    PSACI_KEY_8 = 56,
    PSACI_KEY_9 = 57,
    PSACI_KEY_SEMICOLON = 59, /* ; */
    PSACI_KEY_EQUAL = 61,     /* = */
    PSACI_KEY_A = 65,
    PSACI_KEY_B = 66,
    PSACI_KEY_C = 67,
    PSACI_KEY_D = 68,
    PSACI_KEY_E = 69,
    PSACI_KEY_F = 70,
    PSACI_KEY_G = 71,
    PSACI_KEY_H = 72,
    PSACI_KEY_I = 73,
    PSACI_KEY_J = 74,
    PSACI_KEY_K = 75,
    PSACI_KEY_L = 76,
    PSACI_KEY_M = 77,
    PSACI_KEY_N = 78,
    PSACI_KEY_O = 79,
    PSACI_KEY_P = 80,
    PSACI_KEY_Q = 81,
    PSACI_KEY_R = 82,
    PSACI_KEY_S = 83,
    PSACI_KEY_T = 84,
    PSACI_KEY_U = 85,
    PSACI_KEY_V = 86,
    PSACI_KEY_W = 87,
    PSACI_KEY_X = 88,
    PSACI_KEY_Y = 89,
    PSACI_KEY_Z = 90,
    PSACI_KEY_LEFT_BRACKET = 91,  /* [ */
    PSACI_KEY_BACKSLASH = 92,     /* \ */
    PSACI_KEY_RIGHT_BRACKET = 93, /* ] */
    PSACI_KEY_GRAVE_ACCENT = 96,  /* ` */
    PSACI_KEY_WORLD_1 = 161,      /* non-US #1 */
    PSACI_KEY_WORLD_2 = 162,      /* non-US #2 */
    PSACI_KEY_ESCAPE = 256,
    PSACI_KEY_ENTER = 257,
    PSACI_KEY_TAB = 258,
    PSACI_KEY_BACKSPACE = 259,
    PSACI_KEY_INSERT = 260,
    PSACI_KEY_DELETE = 261,
    PSACI_KEY_RIGHT = 262,
    PSACI_KEY_LEFT = 263,
    PSACI_KEY_DOWN = 264,
    PSACI_KEY_UP = 265,
    PSACI_KEY_PAGE_UP = 266,
    PSACI_KEY_PAGE_DOWN = 267,
    PSACI_KEY_HOME = 268,
    PSACI_KEY_END = 269,
    PSACI_KEY_CAPS_LOCK = 280,
    PSACI_KEY_SCROLL_LOCK = 281,
    PSACI_KEY_NUM_LOCK = 282,
    PSACI_KEY_PRINT_SCREEN = 283,
    PSACI_KEY_PAUSE = 284,
    PSACI_KEY_F1 = 290,
    PSACI_KEY_F2 = 291,
    PSACI_KEY_F3 = 292,
    PSACI_KEY_F4 = 293,
    PSACI_KEY_F5 = 294,
    PSACI_KEY_F6 = 295,
    PSACI_KEY_F7 = 296,
    PSACI_KEY_F8 = 297,
    PSACI_KEY_F9 = 298,
    PSACI_KEY_F10 = 299,
    PSACI_KEY_F11 = 300,
    PSACI_KEY_F12 = 301,
    PSACI_KEY_F13 = 302,
    PSACI_KEY_F14 = 303,
    PSACI_KEY_F15 = 304,
    PSACI_KEY_F16 = 305,
    PSACI_KEY_F17 = 306,
    PSACI_KEY_F18 = 307,
    PSACI_KEY_F19 = 308,
    PSACI_KEY_F20 = 309,
    PSACI_KEY_F21 = 310,
    PSACI_KEY_F22 = 311,
    PSACI_KEY_F23 = 312,
    PSACI_KEY_F24 = 313,
    PSACI_KEY_F25 = 314,
    PSACI_KEY_KP_0 = 320,
    PSACI_KEY_KP_1 = 321,
    PSACI_KEY_KP_2 = 322,
    PSACI_KEY_KP_3 = 323,
    PSACI_KEY_KP_4 = 324,
    PSACI_KEY_KP_5 = 325,
    PSACI_KEY_KP_6 = 326,
    PSACI_KEY_KP_7 = 327,
    PSACI_KEY_KP_8 = 328,
    PSACI_KEY_KP_9 = 329,
    PSACI_KEY_KP_DECIMAL = 330,
    PSACI_KEY_KP_DIVIDE = 331,
    PSACI_KEY_KP_MULTIPLY = 332,
    PSACI_KEY_KP_SUBTRACT = 333,
    PSACI_KEY_KP_ADD = 334,
    PSACI_KEY_KP_ENTER = 335,
    PSACI_KEY_KP_EQUAL = 336,
    PSACI_KEY_LEFT_SHIFT = 340,
    PSACI_KEY_LEFT_CONTROL = 341,
    PSACI_KEY_LEFT_ALT = 342,
    PSACI_KEY_LEFT_SUPER = 343,
    PSACI_KEY_RIGHT_SHIFT = 344,
    PSACI_KEY_RIGHT_CONTROL = 345,
    PSACI_KEY_RIGHT_ALT = 346,
    PSACI_KEY_RIGHT_SUPER = 347,
    PSACI_KEY_MENU = 348,
    PSACI_KEY_LAST = PSACI_KEY_MENU,
};

SACI_API void psaci_event_poll(void);
SACI_API void psaci_event_wait(void);
SACI_API void psaci_event_wait_for_timeout(double timeout);
SACI_API void psaci_event_post_empty(void);
SACI_API void psaci_event_set_mouse_pos_handler(union PSaciWindow window, SaciEventMousePosHandler mouse_pos_handler);
SACI_API SaciBool psaci_event_is_key_pressed(union PSaciWindow window, int keycode);

#endif // SACI_BACKEND_WINDOWING_SC_WINDOWING_H
