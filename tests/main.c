#define GLITCH_STD
#include "glitch/glitc.h"

#define GLITCH_RAND
#include "glitch/glitc-rand.h"

#define GLITCH_COMPLEX
#include "glitch/glitc-complex.h"

#define SACI_TEST_BUILD

extern void Test_Rendering(GlitchTester* t);

int main(void) {
    GlitchTester* t = glitch_Tester_New();

    {
        Test_Rendering(t);
    }

    glitch_End(t);

    return 0;
}
