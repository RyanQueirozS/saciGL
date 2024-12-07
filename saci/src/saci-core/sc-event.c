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

void sc_Event_Set_Mouse_Pos_Handler(sc_Window_t* window,
                                    sc_Event_Mouse_Pos_Handler_t mouse_pos_handler) {
    glfwSetCursorPosCallback(window, mouse_pos_handler);
}

sa_Bool_t sc_Event_Is_Key_Pressed(sc_Window_t* window, enum sc_Keycode_e keycode) {
    return glfwGetKey(window, keycode) == GLFW_PRESS;
}
