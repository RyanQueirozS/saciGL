#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"

#include <assert.h>

#define MY_MEMORY_ARENA_CONTEXT_SIZE 1024
#define MY_RENDER_BATCH_SIZE 1024 // doesn't need to be the same as the above

void init_saci(void) {
    saci_InitMath();

    sc_Renderer* renderer = sc_Renderer_CreateEmpty();
    sc_Renderer_InitMemoryContext(renderer, MY_MEMORY_ARENA_CONTEXT_SIZE);
    sc_Renderer_ResizeRenderBuffer(renderer, MY_RENDER_BATCH_SIZE);
    assert(renderer);
}

int main(void) {
    init_saci();
}
