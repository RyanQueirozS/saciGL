#include <stdio.h>
#include <stdlib.h>
#include "glitch/glitc.h"
#include "glitch/glitc-rand.h"
#include "glitch/glitc-complex.h"

// #define SC_RENDERER_STRUCT_EXPOSE // Already defined in the compiler
#define SC_RENDERER_STRUCT_EXPOSE
#include "saci-core/sc-gl.h"

#include <time.h>

#define VERTICE_AMOUNT 500
#define OFFSET_AMOUNT 10

extern struct sc_Renderer __sc_renderer;

// Helper
void Print_Vec3(sa_vec3_t v1);
void Gen_Array_Rand_Offset(sa_vec3_t** vertex_array_out, int** offset_array_out);

// Test funcs
void Test_Rendering_Bind_Index_Buffer(GlitchTester* t);
void Test_Rendering_Push_Vertex(GlitchTester* t);
void Test_Rendering_Index(GlitchTester* t);
void Test_Rendering_Begin(GlitchTester* t);
void Test_Rendering_Bind_Texture(GlitchTester* t);

// Main test
void Test_Rendering(GlitchTester* t) {
    Test_Rendering_Bind_Index_Buffer(t);
    Test_Rendering_Push_Vertex(t);
    Test_Rendering_Index(t);
    Test_Rendering_Begin(t);
    Test_Rendering_Bind_Texture(t);
}

// Test funcs

void Test_Rendering_Bind_Index_Buffer(GlitchTester* t) {
    {
        sc_Renderer_Begin();
        // Test proper index buffer binding
        sa_u32_t test_indices[] = {0, 1, 2, 2, 3, 0};
        sa_u32_t test_indices_count = sizeof(test_indices) / sizeof(sa_u32_t);

        sc_Renderer_Bind_Index_Buffer(test_indices, test_indices_count);

        GLITCH_ASSERT(t, __sc_renderer.bound_index_array_count == test_indices_count, "Index count should match");

        for (sa_u32_t i = 0; i < test_indices_count; ++i) {
            GLITCH_ASSERT(t, __sc_renderer.bound_index_array[i] == test_indices[i], "Indices should be bound correctly");
        }
    }
}

void Test_Rendering_Push_Vertex(GlitchTester* t) {
    {
        sc_Renderer_Begin();
        // Proper vertex pushing
        int* offset_array = NULL;
        sa_vec3_t* vertice_array = NULL;
        Gen_Array_Rand_Offset((sa_vec3_t**)&vertice_array, (int**)&offset_array);

        // Push vertices using calculated offsets
        {
            int sum = 0;
            for (int i = 0; i < OFFSET_AMOUNT; ++i) {
                int offset = offset_array[i];
                sc_Renderer_Push_Vertex(&vertice_array[sum], NULL, NULL, offset);
                sum += offset;
            }
        }

        { // Test
            int sum = 0;
            for (int i = 0; i < OFFSET_AMOUNT; ++i) {
                int offset = offset_array[i];
                for (int j = 0; j < offset; ++j) {
                    float vec3_expected[3] = {vertice_array[sum + j].m_x, vertice_array[sum + j].m_y, vertice_array[sum + j].m_z};
                    float vec3_received[3] = {__sc_renderer.batch.vertex_array[sum + j].pos.m_x, __sc_renderer.batch.vertex_array[sum + j].pos.m_y, __sc_renderer.batch.vertex_array[sum + j].pos.m_z};
                    GLITCH_ASSERT(t, glitch_Cmpx_Vec3_Is_Equal(vec3_expected, vec3_received), "Should contain correct vertex information");
                }
                sum += offset;
            }
        }
        free(offset_array);
        free(vertice_array);
    }
}

void Test_Rendering_Index(GlitchTester* t) {
    { // Check if binding is correct
        sc_Renderer_Begin();
        sa_u32_t indices[6] = {0, 1, 2, 2, 3, 0};
        sa_u32_t indices_count = 6;
        sc_Renderer_Bind_Index_Buffer(indices, indices_count);
        GLITCH_ASSERT(t, __sc_renderer.bound_index_array_count == indices_count, "Vertex count should match");
        for (sa_u32_t i = 0; i < indices_count; ++i) {
            GLITCH_ASSERT(t, __sc_renderer.bound_index_array[i] == indices[i], "Vertex count should match");
        }
    }
    {
        // Push model with index and vertex information
        sc_Renderer_Begin();
        sa_u32_t indices[6] = {0, 1, 2, 2, 3, 0};
        sa_u32_t indices_count = 6;
        sa_vec3_t vertices[] = {
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}};
        sa_u32_t vertex_count = sizeof(vertices) / sizeof(sa_vec3_t);

        sc_Renderer_Bind_Index_Buffer(indices, indices_count);
        sc_Renderer_Push_Vertex(vertices, NULL, NULL, vertex_count);

        GLITCH_ASSERT(t, __sc_renderer.batch.vertex_array_count == vertex_count, "Vertex count should match");
        GLITCH_ASSERT(t, __sc_renderer.batch.index_array_count == indices_count, "Index count should match");

        for (sa_u32_t i = 0; i < vertex_count; ++i) {
            GLITCH_ASSERT(t, __sc_renderer.batch.vertex_array[i].pos.m_x == vertices[i].m_x, "Vertex X should match");
            GLITCH_ASSERT(t, __sc_renderer.batch.vertex_array[i].pos.m_y == vertices[i].m_y, "Vertex Y should match");
            GLITCH_ASSERT(t, __sc_renderer.batch.vertex_array[i].pos.m_z == vertices[i].m_z, "Vertex Z should match");
        }

        for (sa_u32_t i = 0; i < indices_count; ++i) {
            GLITCH_ASSERT(t, __sc_renderer.batch.index_array[i] == indices[i], "Indices should match");
        }
    }
}

void Test_Rendering_Begin(GlitchTester* t) {
    {
        sc_Renderer_Begin();

        GLITCH_ASSERT(t, __sc_renderer.bound_index_array == NULL, "Bound index array should be NULL after Begin");
        GLITCH_ASSERT(t, __sc_renderer.bound_index_array_count == 0, "Bound index count should be zero after Begin");
        GLITCH_ASSERT(t, __sc_renderer.batch.index_array == NULL, "Batch index array should be NULL after Begin");
        GLITCH_ASSERT(t, __sc_renderer.batch.index_array_count == 0, "Batch index count should be zero after Begin");
        GLITCH_ASSERT(t, __sc_renderer.batch.vertex_array == NULL, "Batch vertex array should be NULL after Begin");
        GLITCH_ASSERT(t, __sc_renderer.batch.vertex_array_count == 0, "Batch vertex count should be zero after Begin");
        GLITCH_ASSERT(t, __sc_renderer.current_texture_id == 0, "Texture ID should be reset after Begin");
    }
}

void Test_Rendering_Bind_Texture(GlitchTester* t) {
    {
        sc_Renderer_Begin();
        sa_textureId test_texture = 123;
        sc_Renderer_Bind_Texture(test_texture);

        GLITCH_ASSERT(t, __sc_renderer.current_texture_id == test_texture, "Texture ID should be bound correctly");
    }
}

// Helper funcs

void Print_Vec3(sa_vec3_t v1) {
    printf("x:%f\ty:%f\tz:%f\n", v1.m_x, v1.m_y, v1.m_z);
}

void Gen_Array_Rand_Offset(sa_vec3_t* vertex_array_out[VERTICE_AMOUNT], int* offset_array_out[OFFSET_AMOUNT]) {
    // Initialize vertices with random values
    (*vertex_array_out) = malloc(sizeof(sa_vec3_t) * VERTICE_AMOUNT);
    {
        for (int i = 0; i < VERTICE_AMOUNT; ++i) {
            (*vertex_array_out)[i] = (sa_vec3_t){
                .m_x = glitch_Rand_Range_Float(-1, 1),
                .m_y = glitch_Rand_Range_Float(-1, 1),
                .m_z = glitch_Rand_Range_Float(-1, 1),
            };
        }
    }

    // Generate offsets ensuring the total sum doesn't exceed VERTICE_AMOUNT
    (*offset_array_out) = malloc(sizeof(int) * OFFSET_AMOUNT);
    {
        int sum = 0;
        for (int i = 0; i < OFFSET_AMOUNT - 1; ++i) {
            (*offset_array_out)[i] = rand() % (VERTICE_AMOUNT - sum);
            sum += (*offset_array_out)[i];
        }
        (*offset_array_out)[OFFSET_AMOUNT - 1] = VERTICE_AMOUNT - sum; // Ensure we cover all vertices
    }
}
