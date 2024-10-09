#include "saci-core.h"

void sc_PollEvents() {
    glfwPollEvents();
}
void sc_WaitForEvents() {
    glfwWaitEvents();
}
void sc_WaitForEventsTimeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

void sc_PostEmptyEvent() {
    glfwPostEmptyEvent();
}

void sc_SetMousePosHandler(sc_Window* window, sc_MousePosHandlerFunction mousePosHandlerFunction) {
    glfwSetCursorPosCallback(window, mousePosHandlerFunction);
}

void sc_SetCursorHandler(sc_Window* window, void (*func)(sc_Window*, double, double)) {
    glfwSetCursorPosCallback(window, func);
}

saci_Bool sc_IsKeyPressed(sc_Window* window, sc_Keycode keycode) {
    int keyState = glfwGetKey(window, keycode);
    if (keyState == GLFW_PRESS) return SACI_TRUE;
    return false;
}
