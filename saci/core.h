#ifndef __SACI_CORE_H__
#define __SACI_CORE_H__

#include <stdint.h>

#include "scgl.h"
#include "texture.h"

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
// Color

saci_Color saciColorFromHex(uint32_t hexValue);
saci_Color saciColorFrom8Bit(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

//----------------------------------------------------------------------------//
// Windowing

saciWindow* saciCreateWindow(const saciWindowSpecs windowSpecs);
void saciTerminate(void);

//----------------------------------------------------------------------------//
// Canvas

void saciClearCanvas(const saci_Color color);

//----------------------------------------------------------------------------//
// Shadering
// Note: some of these could be deleted as the Renderer portion improves

uint32_t saciLoadShaderFromPath(const char* path, uint32_t shaderType);
uint32_t saciCreateProgram(uint32_t shaders[], size_t shaderAmmount);

//----------------------------------------------------------------------------//
// Renderer

#endif
