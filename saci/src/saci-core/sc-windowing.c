#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "saci-core.h"
#include "saci-utils/su-types.h"

saci_Bool sc_GLFWInit(void) {
    int success = glfwInit();
    if (!success) return SACI_FALSE;
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return SACI_TRUE;
}

saci_Bool sc_GLADInit(void) {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != SACI_TRUE) {
        return SACI_FALSE;
    }
    return SACI_TRUE;
}

sc_Window* sc_CreateWindow(int width, int height, const char* title,
                           sc_Monitor* monitor, sc_Window* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}
void sc_MakeWindowContext(sc_Window* window) { glfwMakeContextCurrent(window); }

saci_Bool sc_WindowShouldClose(sc_Window* window) { return glfwWindowShouldClose(window); }

void sc_SetWindowPosHandler(sc_Window* window, sc_WindowPosHandler windowPosHandler) {
    glfwSetWindowPosCallback(window, windowPosHandler);
}

void sc_SetWindowSizeHandler(sc_Window* window, sc_WindowSizeHandler windowSizeHandler) {
    glfwSetWindowSizeCallback(window, windowSizeHandler);
}

void sc_Terminate(void) { glfwTerminate(); }

void sc_ClearWindow(saci_Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sc_SwapWindowBuffer(sc_Window* window) {
    glfwSwapBuffers(window);
}
