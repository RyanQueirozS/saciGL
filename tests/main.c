#define GLITCH_STD
#include "glitch/glitc.h"

#define GLITCH_RAND
#include "glitch/glitc-rand.h"

#define GLITCH_COMPLEX
#include "glitch/glitc-complex.h"

#define SACI_TEST_BUILD
#define SC_MODEL_MESH_EXPOSE

extern void Test_Rendering(GlitchTester* t);
extern void Test_Renderer_Integration(GlitchTester* t);

int main(void) {
    GlitchTester* t = glitch_Tester_New();

    {
        // Test_Rendering(t);
        Test_Renderer_Integration(t);
    }

    glitch_End(t);

    return 0;
}
