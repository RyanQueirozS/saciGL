#include "saci_core/runtime/event.h"

#include "saci_platform/windowing/windowing.h"

#include <string.h>

void csaci_event_poll(CSaciEvent* event)
{
    memset(&event->keyboard, 0,
           sizeof(SaciBool) * sizeof(event->keyboard));

    memset(&event->mouse, 0,
           sizeof(SaciBool) * sizeof(event->mouse));

    memset(&event->controller, 0,
           sizeof(SaciBool) * sizeof(event->controller));
    psaci_event_poll();
}
