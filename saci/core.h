#ifndef __SACI_CORE_H__
#define __SACI_CORE_H__

#include <stdint.h>

#include "scgl.h"
#include "helpers.h"

//----------------------------------------------------------------------------//

typedef scglWindow saciWindow;
typedef scglMonitor saciMonitor;

typedef struct saciWindowSpecs {
    int width;
    int height;
    const char* title;
    saciMonitor* monitor;
    saciWindow* windowShare;
} saciWindowSpecs;

//----------------------------------------------------------------------------//
// Windowing

saciWindow* saciCreateWindow(const saciWindowSpecs windowSpecs);
void saciTerminate(void);

//----------------------------------------------------------------------------//
// Composition

void saciSetCanvasColor(const color color);

void saciBeginComposition();
void saciEndComposition();

#endif
