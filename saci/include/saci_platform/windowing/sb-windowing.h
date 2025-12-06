#ifndef SACI_BACKEND_WINDOWING_SC_WINDOWING_H
#define SACI_BACKEND_WINDOWING_SC_WINDOWING_H

#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"
#include "./sb-windowing-common.h"

su_Bool sb_window_load(void);
union sb_Window sb_window_create(int width, int height, const char* title,
                                 union sb_WindowOpts opts);
SA_API void sb_window_free(union sb_Window window);
SA_API void sb_window_make_context(union sb_Window window);
SA_API su_Bool sb_window_should_close(union sb_Window window);
SA_API void sb_window_set_pos_handler(union sb_Window window, su_WindowSizeHandler window_pos_handler);
SA_API void sb_window_set_size_handler(union sb_Window window, su_WindowSizeHandler window_size_handler);
SA_API void sb_window_terminate(void);
SA_API void sb_window_swap_buffer(union sb_Window window);

/* === Event === */

// TODO GLFW_KEY_LAST
#define sb_EVENT_KEY_AMOUNT 1000

#define sb_EVENT_MOUSE_BUTTON_AMOUNT 8

#define sb_EVENT_CONTROLLER_BUTTON_AMOUNT 16

enum sb_Key {
    sb_KEY_SPACE = 32,
    sb_KEY_APOSTROPHE = 39, /* ' */
    sb_KEY_COMMA = 44,      /* , */
    sb_KEY_MINUS = 45,      /* - */
    sb_KEY_PERIOD = 46,     /* . */
    sb_KEY_SLASH = 47,      /* / */
    sb_KEY_0 = 48,
    sb_KEY_1 = 49,
    sb_KEY_2 = 50,
    sb_KEY_3 = 51,
    sb_KEY_4 = 52,
    sb_KEY_5 = 53,
    sb_KEY_6 = 54,
    sb_KEY_7 = 55,
    sb_KEY_8 = 56,
    sb_KEY_9 = 57,
    sb_KEY_SEMICOLON = 59, /* ; */
    sb_KEY_EQUAL = 61,     /* = */
    sb_KEY_A = 65,
    sb_KEY_B = 66,
    sb_KEY_C = 67,
    sb_KEY_D = 68,
    sb_KEY_E = 69,
    sb_KEY_F = 70,
    sb_KEY_G = 71,
    sb_KEY_H = 72,
    sb_KEY_I = 73,
    sb_KEY_J = 74,
    sb_KEY_K = 75,
    sb_KEY_L = 76,
    sb_KEY_M = 77,
    sb_KEY_N = 78,
    sb_KEY_O = 79,
    sb_KEY_P = 80,
    sb_KEY_Q = 81,
    sb_KEY_R = 82,
    sb_KEY_S = 83,
    sb_KEY_T = 84,
    sb_KEY_U = 85,
    sb_KEY_V = 86,
    sb_KEY_W = 87,
    sb_KEY_X = 88,
    sb_KEY_Y = 89,
    sb_KEY_Z = 90,
    sb_KEY_LEFT_BRACKET = 91,  /* [ */
    sb_KEY_BACKSLASH = 92,     /* \ */
    sb_KEY_RIGHT_BRACKET = 93, /* ] */
    sb_KEY_GRAVE_ACCENT = 96,  /* ` */
    sb_KEY_WORLD_1 = 161,      /* non-US #1 */
    sb_KEY_WORLD_2 = 162,      /* non-US #2 */
    sb_KEY_ESCAPE = 256,
    sb_KEY_ENTER = 257,
    sb_KEY_TAB = 258,
    sb_KEY_BACKSPACE = 259,
    sb_KEY_INSERT = 260,
    sb_KEY_DELETE = 261,
    sb_KEY_RIGHT = 262,
    sb_KEY_LEFT = 263,
    sb_KEY_DOWN = 264,
    sb_KEY_UP = 265,
    sb_KEY_PAGE_UP = 266,
    sb_KEY_PAGE_DOWN = 267,
    sb_KEY_HOME = 268,
    sb_KEY_END = 269,
    sb_KEY_CAPS_LOCK = 280,
    sb_KEY_SCROLL_LOCK = 281,
    sb_KEY_NUM_LOCK = 282,
    sb_KEY_PRINT_SCREEN = 283,
    sb_KEY_PAUSE = 284,
    sb_KEY_F1 = 290,
    sb_KEY_F2 = 291,
    sb_KEY_F3 = 292,
    sb_KEY_F4 = 293,
    sb_KEY_F5 = 294,
    sb_KEY_F6 = 295,
    sb_KEY_F7 = 296,
    sb_KEY_F8 = 297,
    sb_KEY_F9 = 298,
    sb_KEY_F10 = 299,
    sb_KEY_F11 = 300,
    sb_KEY_F12 = 301,
    sb_KEY_F13 = 302,
    sb_KEY_F14 = 303,
    sb_KEY_F15 = 304,
    sb_KEY_F16 = 305,
    sb_KEY_F17 = 306,
    sb_KEY_F18 = 307,
    sb_KEY_F19 = 308,
    sb_KEY_F20 = 309,
    sb_KEY_F21 = 310,
    sb_KEY_F22 = 311,
    sb_KEY_F23 = 312,
    sb_KEY_F24 = 313,
    sb_KEY_F25 = 314,
    sb_KEY_KP_0 = 320,
    sb_KEY_KP_1 = 321,
    sb_KEY_KP_2 = 322,
    sb_KEY_KP_3 = 323,
    sb_KEY_KP_4 = 324,
    sb_KEY_KP_5 = 325,
    sb_KEY_KP_6 = 326,
    sb_KEY_KP_7 = 327,
    sb_KEY_KP_8 = 328,
    sb_KEY_KP_9 = 329,
    sb_KEY_KP_DECIMAL = 330,
    sb_KEY_KP_DIVIDE = 331,
    sb_KEY_KP_MULTIPLY = 332,
    sb_KEY_KP_SUBTRACT = 333,
    sb_KEY_KP_ADD = 334,
    sb_KEY_KP_ENTER = 335,
    sb_KEY_KP_EQUAL = 336,
    sb_KEY_LEFT_SHIFT = 340,
    sb_KEY_LEFT_CONTROL = 341,
    sb_KEY_LEFT_ALT = 342,
    sb_KEY_LEFT_SUPER = 343,
    sb_KEY_RIGHT_SHIFT = 344,
    sb_KEY_RIGHT_CONTROL = 345,
    sb_KEY_RIGHT_ALT = 346,
    sb_KEY_RIGHT_SUPER = 347,
    sb_KEY_MENU = 348,
    sb_KEY_LAST = sb_KEY_MENU,
};

SA_API void sb_event_poll(void);
SA_API void sb_event_wait(void);
SA_API void sb_event_wait_for_timeout(double timeout);
SA_API void sb_event_post_empty(void);
SA_API void sb_event_set_mouse_pos_handler(union sb_Window window, su_EventMousePosHandler mouse_pos_handler);
SA_API su_Bool sb_event_is_key_pressed(union sb_Window window, int keycode);

#endif // SACI_BACKEND_WINDOWING_SC_WINDOWING_H
