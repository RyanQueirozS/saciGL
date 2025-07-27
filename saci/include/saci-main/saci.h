#ifndef SACI_MAIN_SACI_H
#define SACI_MAIN_SACI_H

#include "saci-backend/sb-windowing.h"

#include "saci-utils/su-general.h"

typedef struct saci_Cube {
    su_vec3 pos_center;
    su_vec3 dimentions;
    su_vec3 rotation;
    su_color color;
} saci_Cube;

typedef void (*saci_LoopFunc)(const double delta);

typedef enum {
    saci_RENDERING_FLAG_ROTATION_RTS = 1, // Default is TRS
} saci_RenderingFlags;

typedef struct {
    struct {
        su_bool key_held[sb_EVENT_KEY_AMOUNT];
        su_bool key_was_pressed[sb_EVENT_KEY_AMOUNT];
        su_bool key_was_released[sb_EVENT_KEY_AMOUNT];
    } keyboard;
    struct {
        su_bool button_held[sb_EVENT_MOUSE_BUTTON_AMOUNT];
        su_bool button_was_pressed[sb_EVENT_MOUSE_BUTTON_AMOUNT];
        su_bool button_was_released[sb_EVENT_MOUSE_BUTTON_AMOUNT];
    } mouse;
    struct {
        su_bool button_held[sb_EVENT_CONTROLLER_BUTTON_AMOUNT];
        su_bool button_was_pressed[sb_EVENT_MOUSE_BUTTON_AMOUNT];
        su_bool button_was_released[sb_EVENT_MOUSE_BUTTON_AMOUNT];
    } controller;
} saci_Event;

SA_API void saci_Init(void);

SA_API void saci_Enable(saci_RenderingFlags flag, su_bool enable);

SA_API void saci_Begin(void);

SA_API void saci_Set_Background_Color(const su_color color);

SA_API void saci_Set_Loop_Func(saci_LoopFunc loop_func);

SA_API const saci_Event* saci_Get_Event(void);

SA_API void saci_Loop(void);

SA_API void saci_Draw_Cube(const saci_Cube cube);

SA_API void saci_Present(void);

SA_API void saci_Free(void);

#endif // SACI_MAIN_SACI_H
