#ifndef SACI_CORE_RUNTIME_EVENT_H
#define SACI_CORE_RUNTIME_EVENT_H

#include "saci_util/types.h"

typedef struct {
    struct {
        SaciBool* key_held;
        SaciBool* key_was_pressed;
        SaciBool* key_was_released;
    } keyboard;
    struct {
        SaciBool* button_held;
        SaciBool* button_was_pressed;
        SaciBool* button_was_released;
    } mouse;
    struct {
        SaciBool* button_held;
        SaciBool* button_was_pressed;
        SaciBool* button_was_released;
    } controller;
} CSaciEvent;

void csaci_event_poll(CSaciEvent* event);

#endif // SACI_CORE_RUNTIME_EVENT_H
