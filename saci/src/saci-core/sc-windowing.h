#ifndef SACI_CORE_SC_WINDOWING_H
#define SACI_CORE_SC_WINDOWING_H

#include <GLFW/glfw3.h>
#include <saci-utils/su-types.h>
#ifndef SACI_GLFW_MONITOR_t
typedef GLFWmonitor sc_monitor_t;
#endif

#ifndef SACI_GLFW_WINDOW_t
typedef GLFWwindow sc_window_t;
#endif

#ifndef SACI_WINDOW_POS_HANDLER_t
#  define SACI_WINDOW_POS_HANDLER_t
typedef void (*sc_window_posHandler_t)(sc_window_t* window, int pos_x, int pos_y);
#endif

#ifndef SACI_WINDOW_SIZE_HANDLER_t
#  define SACI_WINDOW_SIZE_HANDLER_t
typedef void (*sc_window_sizeHandler_t)(sc_window_t* window, int width, int height);
#endif

sa_bool sc_GLFW_Init(void);
sa_bool sc_GLAD_Init(void);

sc_window_t* sc_Window_Create(int width, int height, const char* title,
                              sc_monitor_t* monitor, sc_window_t* share);

void sc_Window_Free(sc_window_t* window);

void sc_Window_Make_Context(sc_window_t* window);

sa_bool sc_Window_Should_Close(sc_window_t* window);

void sc_Window_Set_Pos_Handler(sc_window_t* window, sc_window_posHandler_t window_pos_handler);

void sc_Window_Set_Size_Handler(sc_window_t* window, sc_window_sizeHandler_t window_size_handler);

void sc_Window_Terminate(void);

void sc_Window_Clear_Color(const sa_color color);

void sc_Window_Swap_Buffer(sc_window_t* window);

#endif // SACI_CORE_SC_WINDOWING_H
