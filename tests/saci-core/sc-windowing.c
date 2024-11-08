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
    {
        saci_Test_AddDescription("should initialize glfw without any issues");
        SACI_TEST_ASSERT(sc_GLFW_Init());
    }
}

static void saci_Test_GLAD_Init(void) {
    {
        saci_Test_AddDescription("should initialize glad without any issues");
        sc_GLFW_Init();
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        sc_Window_MakeContext(window);
        SACI_TEST_ASSERT(sc_GLAD_Init());
        sc_Window_Terminate();
    }
    {
        saci_Test_AddDescription("should fail if GLFW is not setup before");
        SACI_TEST_ASSERT(!sc_GLAD_Init());
    }
    {
        saci_Test_AddDescription("should fail if a window is not context");
        sc_GLFW_Init();
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        (void)window;
        SACI_TEST_ASSERT(!sc_GLAD_Init());
        sc_Window_Terminate();
    }
}

static void saci_Test_WindowCreate(void) {
    {
        saci_Test_AddDescription("should initialize a window propperly");
        sc_GLFW_Init();
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        SACI_TEST_ASSERT(window);
        sc_Window_Terminate();
    }
    {
        saci_Test_AddDescription("should not work if glfw is not setup");
        sc_Window* window = sc_Window_Create(10, 10, "Test Window", NULL, NULL);
        SACI_TEST_ASSERT(!window);
        sc_Window_Terminate();
    }
}
