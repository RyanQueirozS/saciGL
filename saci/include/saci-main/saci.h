#ifndef SACI_MAIN_SACI_H
#define SACI_MAIN_SACI_H

#include "saci-backend/windowing/sb-windowing.h"

#include "saci-utils/su-general.h"

typedef enum {
    saci_MODEL_FLAG_ROTATION_RTS = 1U << 0,
} saci_ModelFlag;

typedef enum {
    saci_RENDERING_FLAG_BLEND = 1U << 0,
} saci_RenderingFlags;

typedef struct saci_Cube {
    su_Vec3 pos_center;
    su_Vec3 dimentions;
    su_Vec3 rotation;
    su_Color fill_color;
    saci_ModelFlag flags;
} saci_Cube;

typedef void (*saci_LoopFunc)(const double delta);

typedef struct {
    struct {
        su_Bool key_held[sb_EVENT_KEY_AMOUNT];
        su_Bool key_was_pressed[sb_EVENT_KEY_AMOUNT];
        su_Bool key_was_released[sb_EVENT_KEY_AMOUNT];
    } keyboard;
    struct {
        su_Bool button_held[sb_EVENT_MOUSE_BUTTON_AMOUNT];
        su_Bool button_was_pressed[sb_EVENT_MOUSE_BUTTON_AMOUNT];
        su_Bool button_was_released[sb_EVENT_MOUSE_BUTTON_AMOUNT];
    } mouse;
    struct {
        su_Bool button_held[sb_EVENT_CONTROLLER_BUTTON_AMOUNT];
        su_Bool button_was_pressed[sb_EVENT_MOUSE_BUTTON_AMOUNT];
        su_Bool button_was_released[sb_EVENT_MOUSE_BUTTON_AMOUNT];
    } controller;
} saci_Event;

SA_API void saci_init(void);

SA_API void saci_enable(saci_RenderingFlags flag, su_Bool enable);

SA_API void saci_begin(void);

SA_API void saci_set_background_color(const su_Color color);

SA_API void saci_set_loop_func(saci_LoopFunc loop_func);

SA_API const saci_Event* saci_get_event(void);

SA_API void saci_loop(void);

SA_API void saci_draw_cube(const saci_Cube cube);

SA_API void saci_present(void);

SA_API void saci_free(void);

#endif // SACI_MAIN_SACI_H
