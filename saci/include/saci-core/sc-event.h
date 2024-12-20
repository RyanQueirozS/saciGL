/**
 * @file sc-windowing.h
 * @brief This header defines event related funtions regarding saciCORE
 */

#ifndef __SACI_CORE_SC_EVENT_H__
#define __SACI_CORE_SC_EVENT_H__

#include "saci-core/sc-gl.h"

/* === Event Handling === */

typedef void (*sc_Event_MousePosHandler_t)(sc_Window_t* window, double posx, double posy);

enum sc_Keycode_e {
    sc_KEY_SPACE = 32,
    sc_KEY_APOSTROPHE = 39, /* ' */
    sc_KEY_COMMA = 44,      /* , */
    sc_KEY_MINUS = 45,      /* - */
    sc_KEY_PERIOD = 46,     /* . */
    sc_KEY_SLASH = 47,      /* / */
    sc_KEY_0 = 48,
    sc_KEY_1 = 49,
    sc_KEY_2 = 50,
    sc_KEY_3 = 51,
    sc_KEY_4 = 52,
    sc_KEY_5 = 53,
    sc_KEY_6 = 54,
    sc_KEY_7 = 55,
    sc_KEY_8 = 56,
    sc_KEY_9 = 57,
    sc_KEY_SEMICOLON = 59, /* ; */
    sc_KEY_EQUAL = 61,     /* = */
    sc_KEY_A = 65,
    sc_KEY_B = 66,
    sc_KEY_C = 67,
    sc_KEY_D = 68,
    sc_KEY_E = 69,
    sc_KEY_F = 70,
    sc_KEY_G = 71,
    sc_KEY_H = 72,
    sc_KEY_I = 73,
    sc_KEY_J = 74,
    sc_KEY_K = 75,
    sc_KEY_L = 76,
    sc_KEY_M = 77,
    sc_KEY_N = 78,
    sc_KEY_O = 79,
    sc_KEY_P = 80,
    sc_KEY_Q = 81,
    sc_KEY_R = 82,
    sc_KEY_S = 83,
    sc_KEY_T = 84,
    sc_KEY_U = 85,
    sc_KEY_V = 86,
    sc_KEY_W = 87,
    sc_KEY_X = 88,
    sc_KEY_Y = 89,
    sc_KEY_Z = 90,
    sc_KEY_LEFT_BRACKET = 91,  /* [ */
    sc_KEY_BACKSLASH = 92,     /* \ */
    sc_KEY_RIGHT_BRACKET = 93, /* ] */
    sc_KEY_GRAVE_ACCENT = 96,  /* ` */
    sc_KEY_WORLD_1 = 161,      /* non-US #1 */
    sc_KEY_WORLD_2 = 162,      /* non-US #2 */
    sc_KEY_ESCAPE = 256,
    sc_KEY_ENTER = 257,
    sc_KEY_TAB = 258,
    sc_KEY_BACKSPACE = 259,
    sc_KEY_INSERT = 260,
    sc_KEY_DELETE = 261,
    sc_KEY_RIGHT = 262,
    sc_KEY_LEFT = 263,
    sc_KEY_DOWN = 264,
    sc_KEY_UP = 265,
    sc_KEY_PAGE_UP = 266,
    sc_KEY_PAGE_DOWN = 267,
    sc_KEY_HOME = 268,
    sc_KEY_END = 269,
    sc_KEY_CAPS_LOCK = 280,
    sc_KEY_SCROLL_LOCK = 281,
    sc_KEY_NUM_LOCK = 282,
    sc_KEY_PRINT_SCREEN = 283,
    sc_KEY_PAUSE = 284,
    sc_KEY_F1 = 290,
    sc_KEY_F2 = 291,
    sc_KEY_F3 = 292,
    sc_KEY_F4 = 293,
    sc_KEY_F5 = 294,
    sc_KEY_F6 = 295,
    sc_KEY_F7 = 296,
    sc_KEY_F8 = 297,
    sc_KEY_F9 = 298,
    sc_KEY_F10 = 299,
    sc_KEY_F11 = 300,
    sc_KEY_F12 = 301,
    sc_KEY_F13 = 302,
    sc_KEY_F14 = 303,
    sc_KEY_F15 = 304,
    sc_KEY_F16 = 305,
    sc_KEY_F17 = 306,
    sc_KEY_F18 = 307,
    sc_KEY_F19 = 308,
    sc_KEY_F20 = 309,
    sc_KEY_F21 = 310,
    sc_KEY_F22 = 311,
    sc_KEY_F23 = 312,
    sc_KEY_F24 = 313,
    sc_KEY_F25 = 314,
    sc_KEY_KP_0 = 320,
    sc_KEY_KP_1 = 321,
    sc_KEY_KP_2 = 322,
    sc_KEY_KP_3 = 323,
    sc_KEY_KP_4 = 324,
    sc_KEY_KP_5 = 325,
    sc_KEY_KP_6 = 326,
    sc_KEY_KP_7 = 327,
    sc_KEY_KP_8 = 328,
    sc_KEY_KP_9 = 329,
    sc_KEY_KP_DECIMAL = 330,
    sc_KEY_KP_DIVIDE = 331,
    sc_KEY_KP_MULTIPLY = 332,
    sc_KEY_KP_SUBTRACT = 333,
    sc_KEY_KP_ADD = 334,
    sc_KEY_KP_ENTER = 335,
    sc_KEY_KP_EQUAL = 336,
    sc_KEY_LEFT_SHIFT = 340,
    sc_KEY_LEFT_CONTROL = 341,
    sc_KEY_LEFT_ALT = 342,
    sc_KEY_LEFT_SUPER = 343,
    sc_KEY_RIGHT_SHIFT = 344,
    sc_KEY_RIGHT_CONTROL = 345,
    sc_KEY_RIGHT_ALT = 346,
    sc_KEY_RIGHT_SUPER = 347,
    sc_KEY_MENU = 348,
    sc_KEY_LAST = sc_KEY_MENU,
};

void sc_Event_Poll(void);

void sc_Event_Wait(void);

void sc_Event_Wait_For_Timeout(double timeout);

void sc_Event_Post_Empty(void);

void sc_Event_Set_Mouse_Pos_Handler(sc_Window_t* window, sc_Event_MousePosHandler_t mouse_pos_handler);

sa_bool_t sc_Event_Is_Key_Pressed(sc_Window_t* window, enum sc_Keycode_e keycode);

#endif
