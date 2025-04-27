#include "glitch/glitc-complex.h"
#include "glitch/glitc-rand.h"
#include "glitch/glitc.h"
#include "saci-utils/su-types.h"

#include <stdio.h>
#include <stdlib.h>

// #define SC_RENDERER_STRUCT_EXPOSE // Already defined in the compiler
#define SC_RENDERER_STRUCT_EXPOSE
#include "saci-core/sc-gl.h"

// External "sc-gl" funcs
extern void __sc_Renderer_Reset_Bound(struct __sc_renderer* rendr);

// Helper
void Print_Vec3(sa_vec3 v1);
void Gen_Array_Rand_Offset(sa_vec3** vertex_array_out, int** offset_array_out);

// Test funcs
/// Static
static void __sc_Renderer_Reset_Bound_Test(GlitchTester* t);
static void __sc_Renderer_Reset_Batch_Test(GlitchTester* t);
static void __sc_Renderer_Reset_Call_Test(GlitchTester* t);
static void __sc_Renderer_Init_Test(GlitchTester* t);
static void __sc_Renderer_Init_Batch_Test(GlitchTester* t);
static void __sc_Renderer_Init_Call_Test(GlitchTester* t);
static void __sc_Renderer_Call_Array_Sort_Test(GlitchTester* t);
static void __sc_Renderer_Uniform_Is_Equal(GlitchTester* t);
static void __sc_Renderer_Batch_Flush_Test(GlitchTester* t);
static void __sc_Renderer_Batch_Calls_Test(GlitchTester* t);
/// Non static
static void sc_Renderer_New_Default_Test(GlitchTester* t);
static void sc_Renderer_New_Default_Ctx_Test(GlitchTester* t);
static void sc_Renderer_Bind_Texture_Test(GlitchTester* t);
static void sc_Renderer_Set_Uniform_Struct_Test(GlitchTester* t);
static void sc_Renderer_Bind_Uniform_Struct_Test(GlitchTester* t);
static void sc_Renderer_Bind_Uniform_Value_Test(GlitchTester* t);
static void sc_Renderer_Bind_Index_Buffer_Test(GlitchTester* t);
static void sc_Renderer_Push_Vertex_Test(GlitchTester* t);
static void sc_Renderer_End_Test(GlitchTester* t);
static void sc_Renderer_Free_Test(GlitchTester* t);
static void sc_Renderer_Free_Opts_Test(GlitchTester* t);

// Main test
void Test_Rendering(GlitchTester* t) {
    __sc_Renderer_Reset_Bound_Test(t);
    __sc_Renderer_Reset_Batch_Test(t);
    __sc_Renderer_Reset_Call_Test(t);
    __sc_Renderer_Init_Test(t);
    __sc_Renderer_Init_Batch_Test(t);
    __sc_Renderer_Init_Call_Test(t);
    __sc_Renderer_Call_Array_Sort_Test(t);
    __sc_Renderer_Uniform_Is_Equal(t);
    __sc_Renderer_Batch_Flush_Test(t);
    __sc_Renderer_Batch_Calls_Test(t);
    sc_Renderer_New_Default_Test(t);
    sc_Renderer_New_Default_Ctx_Test(t);
    sc_Renderer_Bind_Texture_Test(t);
    sc_Renderer_Set_Uniform_Struct_Test(t);
    sc_Renderer_Bind_Uniform_Struct_Test(t);
    sc_Renderer_Bind_Uniform_Value_Test(t);
    sc_Renderer_Bind_Index_Buffer_Test(t);
    sc_Renderer_Push_Vertex_Test(t);
    sc_Renderer_End_Test(t);
    sc_Renderer_Free_Test(t);
    sc_Renderer_Free_Opts_Test(t);
}

// Test funcs

/// Static
void __sc_Renderer_Reset_Bound_Test(GlitchTester* t) {
    sc_renderer* rendr = malloc(sizeof(struct __sc_renderer));
    sa_u32* buff = malloc(sizeof(sa_u32) * 1000);
    sa_u32 capacity = 1000;

    rendr->bound_texture_id = 1;
    rendr->bound_index_array_buffer = buff;
    rendr->bound_index_array_capacity = capacity;
    rendr->bound_index_array_length = 100;

    {
        __sc_Renderer_Reset_Bound(rendr);
        GLITCH_ASSERT(t, rendr->bound_texture_id == 0, "Texture id should be reset");
        GLITCH_ASSERT(t, rendr->bound_index_array_buffer == buff, "Index buffer should NOT be freed")
        GLITCH_ASSERT(t, rendr->bound_index_array_capacity == capacity, "Capacity should NOT be reset")
        GLITCH_ASSERT(t, rendr->bound_index_array_length == 0, "Length should be reset")
    }

    free(buff);
    free(rendr);
}

// TODO, this function will change, since uniform block isn't currently being freed
void __sc_Renderer_Reset_Batch_Test(GlitchTester* t) {
    (void)t;
}

// TODO same as above
void __sc_Renderer_Reset_Call_Test(GlitchTester* t) {
    (void)t;
}

void __sc_Renderer_Init_Test(GlitchTester* t) {
    (void)t;
}

void __sc_Renderer_Init_Batch_Test(GlitchTester* t) {
    (void)t;
}

void __sc_Renderer_Init_Call_Test(GlitchTester* t) {
    (void)t;
}

void __sc_Renderer_Call_Array_Sort_Test(GlitchTester* t) {
    (void)t;
}

void __sc_Renderer_Uniform_Is_Equal(GlitchTester* t) {
    (void)t;
}

void __sc_Renderer_Batch_Flush_Test(GlitchTester* t) {
    (void)t;
}

void __sc_Renderer_Batch_Calls_Test(GlitchTester* t) {
    (void)t;
}

/// Non static
void sc_Renderer_New_Default_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_New_Default_Ctx_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Bind_Texture_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Set_Uniform_Struct_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Bind_Uniform_Struct_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Bind_Uniform_Value_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Bind_Index_Buffer_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Push_Vertex_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_End_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Free_Test(GlitchTester* t) {
    (void)t;
}

void sc_Renderer_Free_Opts_Test(GlitchTester* t) {
    (void)t;
}

// Helper funcs

void Print_Vec3(sa_vec3 v1) {
    printf("x:%f\ty:%f\tz:%f\n", v1.x, v1.y, v1.z);
}
