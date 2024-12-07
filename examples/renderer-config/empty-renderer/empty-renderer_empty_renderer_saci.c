#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"

#include <assert.h>

#define MY_MEMORY_ARENA_CONTEXT_SIZE 1024
#define MY_RENDER_BATCH_SIZE 1024 // doesn't need to be the same as the above

void init_saci(void) {
    sa_Math_Init();

    sc_Renderer_t* renderer = sc_Renderer_Create_Empty();
    sc_Renderer_Init_Memory_Context(renderer, MY_MEMORY_ARENA_CONTEXT_SIZE);
    sc_Renderer_Resize_Render_Buffer(renderer, MY_RENDER_BATCH_SIZE);
    assert(renderer);
}

int main(void) {
    init_saci();
}
