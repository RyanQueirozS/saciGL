#ifndef SACI_MAIN_SACI_H
#define SACI_MAIN_SACI_H

#include "saci-backend/sc-windowing.h"

#include "saci-utils/su-general.h"

typedef struct saci_cube {
    su_vec3 pos_center;
    su_vec3 dimentions;
    su_vec3 rotation;
} saci_cube;

typedef void (*saci_loopFunc)(const double delta);

typedef enum {
    saci_ENABLE_ROTATION_RTS = 0,
    saci_ENABLE_ROTATION_TRS = 1,
} saci_enableFlags;

typedef struct {
    struct {
        su_bool key_held[SC_EVENT_KEY_AMOUNT];
        su_bool key_was_pressed[SC_EVENT_KEY_AMOUNT];
        su_bool key_was_released[SC_EVENT_KEY_AMOUNT];
    } keyboard;
    struct {
        su_bool button_held[SC_EVENT_MOUSE_BUTTON_AMOUNT];
        su_bool button_was_pressed[SC_EVENT_MOUSE_BUTTON_AMOUNT];
        su_bool button_was_released[SC_EVENT_MOUSE_BUTTON_AMOUNT];
    } mouse;
    struct {
        su_bool button_held[SC_EVENT_CONTROLLER_BUTTON_AMOUNT];
        su_bool button_was_pressed[SC_EVENT_MOUSE_BUTTON_AMOUNT];
        su_bool button_was_released[SC_EVENT_MOUSE_BUTTON_AMOUNT];
    } controller;
} saci_event;

SA_API void saci_Init(void);

SA_API void saci_Enable(saci_enableFlags enable_flag);

SA_API void saci_Begin(void);

SA_API void saci_Set_Background_Color(const su_color color);

SA_API void saci_Set_Loop_Func(saci_loopFunc loop_func);

SA_API const saci_event* saci_Get_Event(void);

SA_API void saci_Loop(void);

SA_API void saci_Draw_Cube(const saci_cube cube);

SA_API void saci_Present(void);

SA_API void saci_Free(void);

#endif // SACI_MAIN_SACI_H
