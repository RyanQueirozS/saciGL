#include "saci-core/sc-event.h"
#include "saci-utils/su-general.h"

#include <GLFW/glfw3.h>

// todo move to opengl.h

void sc_Event_Poll(void) {
    glfwPollEvents();
}
void sc_Event_Wait(void) {
    glfwWaitEvents();
}
void sc_Event_Wait_For_Timeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

void sc_Event_Post_Empty(void) {
    glfwPostEmptyEvent();
}

void sc_Event_Set_Mouse_Pos_Handler(sc_window_t* window, sc_Event_Mouse_Pos_Handler_t mouse_pos_handler) {
    glfwSetCursorPosCallback(window, mouse_pos_handler);
}

sa_bool_t sc_Event_Is_Key_Pressed(sc_window_t* window, enum sc_keycode_e keycode) {
    return glfwGetKey(window, sa_SCAST_TO_m(int)(keycode)) == GLFW_PRESS;
}
