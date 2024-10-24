/**
 * @file sc-windowing.h
 * @brief This header defines windowing related funtions regarding saciCORE
 *
 * @details
 * This header defines functions for window management within saciCORE,
 * including window creation, handling, context management, and cleanup.
 * It abstracts the usage of GLFW and GLAD to provide a simplified interface
 * for creating and managing windows and their properties.
 */
#ifndef __SACI_CORE_SC_WINDOWING_H__
#define __SACI_CORE_SC_WINDOWING_H__

#include <GLFW/glfw3.h>
#include "saci-utils/su-types.h"

/* === Typedefs === */

/**
 * @typedef sc_Monitor
 * @brief GLFWmonitor wrapper.
 * @note, use this instead of GLFWmonitor, as it may change later.
 */
typedef GLFWmonitor sc_Monitor;

/**
 * @typedef sc_Window
 * @brief GLFWwindow wrapper.
 * @note, use this instead of GLFWwindow, as it may change later.
 */
typedef GLFWwindow sc_Window;

/**
 * @typedef sc_WindowPosHandler
 * @brief Function pointer type for handling window position changes.
 *
 * @param window The window whose position has changed.
 * @param posx The new X position of the window.
 * @param posy The new Y position of the window.
 */
typedef void (*sc_Window_PosHandler)(sc_Window* window, int posx, int posy);

/**
 * @typedef sc_WindowSizeHandler
 * @brief Function pointer type for handling window size changes.
 *
 * @param window The window whose size has changed.
 * @param width The new width of the window.
 * @param height The new height of the window.
 */
typedef void (*sc_Window_SizeHandler)(sc_Window* window, int width, int height);

/* === Windowing Functions === */

/**
 * @brief Initializes the GLFW library for window management.
 *
 * @return SACI_TRUE if initialization was successful, SACI_FALSE otherwise.
 */
saci_Bool sc_GLFW_Init(void);

/**
 * @brief Initializes the GLAD library for OpenGL function loading.
 *
 * @return SACI_TRUE if initialization was successful, SACI_FALSE otherwise.
 */
saci_Bool sc_GLAD_Init(void);

/**
 * @brief Creates a new window.
 *
 * @param width The desired width of the window.
 * @param height The desired height of the window.
 * @param title The title of the window.
 * @param monitor The monitor to display the window on (use NULL for the primary monitor).
 * @param share The window to share resources with (use NULL for no sharing).
 * @return A pointer to the created sc_Window. This function does not check if the window
 * is null.
 */
sc_Window* sc_Window_Create(int width, int height, const char* title, sc_Monitor* monitor,
                            sc_Window* share);

/**
 * @brief Makes the specified window the current OpenGL context.
 *
 * @param window The window to make the current context.
 * @note Needed to initialize GLAD.
 */
void sc_Window_MakeContext(sc_Window* window);

/**
 * @brief Checks if the window should close.
 *
 * @param window The window to check.
 * @return SACI_TRUE if the window should close, SACI_FALSE otherwise.
 */
saci_Bool sc_Window_ShouldClose(sc_Window* window);

/**
 * @brief Sets a callback function for handling window position changes.
 *
 * @param window The window to set the position handler for.
 * @param windowPosHandler The callback function to handle position changes.
 */
void sc_Window_SetPosHandler(sc_Window* window, sc_Window_PosHandler windowPosHandler);

/**
 * @brief Sets a callback function for handling window size changes.
 *
 * @param window The window to set the size handler for.
 * @param windowSizeHandler The callback function to handle size changes.
 */
void sc_Window_SetSizeHandler(sc_Window* window, sc_Window_SizeHandler windowSizeHandler);

/**
 * @brief Terminates the windowing system, cleaning up resources.
 */
void sc_Window_Terminate(void);

/**
 * @brief Clears the window with the specified color.
 *
 * @param color The saci_Color to clear the window with.
 */
void sc_Window_ClearColor(const saci_Color color);

/**
 * @brief Swaps the window buffer to display rendered content.
 *
 * @param window The window whose buffer will be swapped.
 * @note If not called, nothing will show on screen.
 */
void sc_Window_SwapBuffer(sc_Window* window);

#endif
