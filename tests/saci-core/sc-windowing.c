#include "saci-core/sc-gl.h"
#include "saci-testing.h"

static void saci_Test_GLFW_Init(void);
static void saci_Test_GLAD_Init(void);
static void saci_Test_WindowCreate(void);

void saci_TestWindowing(void) {
    saci_Test_GLFW_Init();
    saci_Test_GLAD_Init();
    saci_Test_WindowCreate();
}

static void saci_Test_GLFW_Init(void) {
    { SACI_TEST_ASSERT(sc_GLFW_Init(), "should initialize glfw without any issues"); }
}

static void saci_Test_GLAD_Init(void) {
    {
        sc_GLFW_Init();
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        sc_Window_MakeContext(window);

        SACI_TEST_ASSERT(sc_GLAD_Init(), "should initialize glad without any issues");

        sc_Window_Terminate();
    }
    { SACI_TEST_ASSERT(!sc_GLAD_Init(), "should fail if GLFW is not setup before"); }
    {
        sc_GLFW_Init();
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        (void)window;

        SACI_TEST_ASSERT(!sc_GLAD_Init(), "should fail if a window is not context");

        sc_Window_Terminate();
    }
}

static void saci_Test_WindowCreate(void) {
    {
        sc_GLFW_Init();
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        SACI_TEST_ASSERT(window, "should initialize a window propperly");
        sc_Window_Terminate();
    }
    {
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        SACI_TEST_ASSERT(!window, "should not work if glfw is not setup");
        sc_Window_Terminate();
    }
}
