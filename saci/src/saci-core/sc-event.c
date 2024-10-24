#include "saci-core.h"

void sc_Event_Poll() { glfwPollEvents(); }
void sc_Event_Wait() { glfwWaitEvents(); }
void sc_Event_WaitForTimeout(double timeout) { glfwWaitEventsTimeout(timeout); }

void sc_Event_PostEmpty() { glfwPostEmptyEvent(); }

void sc_Event_SetMousePosHandler(
    sc_Window* window, sc_Event_MousePosHandlerFunction mousePosHandlerFunction) {
    glfwSetCursorPosCallback(window, mousePosHandlerFunction);
}

saci_Bool sc_Event_IsKeyPressed(sc_Window* window, sc_Keycode keycode) {
    int keyState = glfwGetKey(window, keycode);
    if (keyState == GLFW_PRESS) return SACI_TRUE;
    return false;
}
