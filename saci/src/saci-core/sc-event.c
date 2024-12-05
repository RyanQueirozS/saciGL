#include "saci-core/sc-event.h"

#include <GLFW/glfw3.h>

void sc_Event_Poll() {
    glfwPollEvents();
}
void sc_Event_Wait() {
    glfwWaitEvents();
}
void sc_Event_WaitForTimeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

void sc_Event_PostEmpty() {
    glfwPostEmptyEvent();
}

void sc_Event_SetMousePosHandler(sc_Window_t* window,
                                 sc_Event_Mouse_Pos_Handler_t mouse_pos_handler) {
    glfwSetCursorPosCallback(window, mouse_pos_handler);
}

saci_Bool sc_Event_IsKeyPressed(sc_Window_t* window, enum sc_Keycode_e keycode) {
    int key_state = glfwGetKey(window, keycode);
    if (key_state == GLFW_PRESS)
        return SACI_TRUE;
    return false;
}
