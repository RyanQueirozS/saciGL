/**
 * @file sc-windowing.h
 * @brief This header defines event related funtions regarding saciCORE
 */

#ifndef __SACI_CORE_SC_EVENT_H__
#define __SACI_CORE_SC_EVENT_H__

#include "saci-core/sc-gl.h"

/* === Event Handling === */

typedef void (*sc_Event_Mouse_Pos_Handler_t)(sc_Window_t* window, double posx, double posy);

enum sc_Keycode_e {
    sa_KEY_SPACE         = 32,
    sa_KEY_APOSTROPHE    = 39, /* ' */
    sa_KEY_COMMA         = 44, /* , */
    sa_KEY_MINUS         = 45, /* - */
    sa_KEY_PERIOD        = 46, /* . */
    sa_KEY_SLASH         = 47, /* / */
    sa_KEY_0             = 48,
    sa_KEY_1             = 49,
    sa_KEY_2             = 50,
    sa_KEY_3             = 51,
    sa_KEY_4             = 52,
    sa_KEY_5             = 53,
    sa_KEY_6             = 54,
    sa_KEY_7             = 55,
    sa_KEY_8             = 56,
    sa_KEY_9             = 57,
    sa_KEY_SEMICOLON     = 59, /* ; */
    sa_KEY_EQUAL         = 61, /* = */
    sa_KEY_A             = 65,
    sa_KEY_B             = 66,
    sa_KEY_C             = 67,
    sa_KEY_D             = 68,
    sa_KEY_E             = 69,
    sa_KEY_F             = 70,
    sa_KEY_G             = 71,
    sa_KEY_H             = 72,
    sa_KEY_I             = 73,
    sa_KEY_J             = 74,
    sa_KEY_K             = 75,
    sa_KEY_L             = 76,
    sa_KEY_M             = 77,
    sa_KEY_N             = 78,
    sa_KEY_O             = 79,
    sa_KEY_P             = 80,
    sa_KEY_Q             = 81,
    sa_KEY_R             = 82,
    sa_KEY_S             = 83,
    sa_KEY_T             = 84,
    sa_KEY_U             = 85,
    sa_KEY_V             = 86,
    sa_KEY_W             = 87,
    sa_KEY_X             = 88,
    sa_KEY_Y             = 89,
    sa_KEY_Z             = 90,
    sa_KEY_LEFT_BRACKET  = 91,  /* [ */
    sa_KEY_BACKSLASH     = 92,  /* \ */
    sa_KEY_RIGHT_BRACKET = 93,  /* ] */
    sa_KEY_GRAVE_ACCENT  = 96,  /* ` */
    sa_KEY_WORLD_1       = 161, /* non-US #1 */
    sa_KEY_WORLD_2       = 162, /* non-US #2 */
    sa_KEY_ESCAPE        = 256,
    sa_KEY_ENTER         = 257,
    sa_KEY_TAB           = 258,
    sa_KEY_BACKSPACE     = 259,
    sa_KEY_INSERT        = 260,
    sa_KEY_DELETE        = 261,
    sa_KEY_RIGHT         = 262,
    sa_KEY_LEFT          = 263,
    sa_KEY_DOWN          = 264,
    sa_KEY_UP            = 265,
    sa_KEY_PAGE_UP       = 266,
    sa_KEY_PAGE_DOWN     = 267,
    sa_KEY_HOME          = 268,
    sa_KEY_END           = 269,
    sa_KEY_CAPS_LOCK     = 280,
    sa_KEY_SCROLL_LOCK   = 281,
    sa_KEY_NUM_LOCK      = 282,
    sa_KEY_PRINT_SCREEN  = 283,
    sa_KEY_PAUSE         = 284,
    sa_KEY_F1            = 290,
    sa_KEY_F2            = 291,
    sa_KEY_F3            = 292,
    sa_KEY_F4            = 293,
    sa_KEY_F5            = 294,
    sa_KEY_F6            = 295,
    sa_KEY_F7            = 296,
    sa_KEY_F8            = 297,
    sa_KEY_F9            = 298,
    sa_KEY_F10           = 299,
    sa_KEY_F11           = 300,
    sa_KEY_F12           = 301,
    sa_KEY_F13           = 302,
    sa_KEY_F14           = 303,
    sa_KEY_F15           = 304,
    sa_KEY_F16           = 305,
    sa_KEY_F17           = 306,
    sa_KEY_F18           = 307,
    sa_KEY_F19           = 308,
    sa_KEY_F20           = 309,
    sa_KEY_F21           = 310,
    sa_KEY_F22           = 311,
    sa_KEY_F23           = 312,
    sa_KEY_F24           = 313,
    sa_KEY_F25           = 314,
    sa_KEY_KP_0          = 320,
    sa_KEY_KP_1          = 321,
    sa_KEY_KP_2          = 322,
    sa_KEY_KP_3          = 323,
    sa_KEY_KP_4          = 324,
    sa_KEY_KP_5          = 325,
    sa_KEY_KP_6          = 326,
    sa_KEY_KP_7          = 327,
    sa_KEY_KP_8          = 328,
    sa_KEY_KP_9          = 329,
    sa_KEY_KP_DECIMAL    = 330,
    sa_KEY_KP_DIVIDE     = 331,
    sa_KEY_KP_MULTIPLY   = 332,
    sa_KEY_KP_SUBTRACT   = 333,
    sa_KEY_KP_ADD        = 334,
    sa_KEY_KP_ENTER      = 335,
    sa_KEY_KP_EQUAL      = 336,
    sa_KEY_LEFT_SHIFT    = 340,
    sa_KEY_LEFT_CONTROL  = 341,
    sa_KEY_LEFT_ALT      = 342,
    sa_KEY_LEFT_SUPER    = 343,
    sa_KEY_RIGHT_SHIFT   = 344,
    sa_KEY_RIGHT_CONTROL = 345,
    sa_KEY_RIGHT_ALT     = 346,
    sa_KEY_RIGHT_SUPER   = 347,
    sa_KEY_MENU          = 348,
    sa_KEY_LAST          = sa_KEY_MENU,
};

void      sc_Event_Poll();

void      sc_Events_Wait();

void      sc_Event_Wait_For_Timeout(double timeout);

void      sc_Event_Post_Empty();

void      sc_Event_SetMousePosHandler(sc_Window_t* window, sc_Event_Mouse_Pos_Handler_t mouse_pos_handler);

sa_Bool_t sc_Event_IsKeyPressed(sc_Window_t* window, enum sc_Keycode_e keycode);

#endif
