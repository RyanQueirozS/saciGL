#ifndef TEST_MOCKS_GLFW_MOCK_H
#define TEST_MOCKS_GLFW_MOCK_H
#include <stdio.h>

int glfwInit() {
    printf("glfwInit called\n");
    return 1; // Mock success
}

void glfwWindowHint(int hint, int value) {
    printf("glfwWindowHint called: hint=%d, value=%d\n", hint, value);
    (void)hint;
    (void)value;
}

void* glfwCreateWindow(int width, int height, const char* title, void* monitor, void* share) {
    printf("glfwCreateWindow called: %dx%d, title=%s\n", width, height, title);
    (void)width;
    (void)height;
    (void)title;
    (void)monitor;
    (void)share;
    static int mock_window = 1;
    return &mock_window; // Mock window pointer
}

void* glfwGetProcAddress(const char* procname) {
    printf("glfwGetProcAddress called: %s\n", procname);
    (void)procname;
    // Return mock function pointers - you'd need to map these to actual mock functions
    return NULL;
}

void glfwDestroyWindow(void* window) {
    printf("glfwDestroyWindow called\n");
    (void)window;
}

void glfwMakeContextCurrent(void* window) {
    printf("glfwMakeContextCurrent called\n");
    (void)window;
}

int glfwWindowShouldClose(void* window) {
    printf("glfwWindowShouldClose called\n");
    (void)window;
    return 0; // Mock not closing
}

void glfwSetWindowPosCallback(void* window, void* callback) {
    printf("glfwSetWindowPosCallback called\n");
    (void)window;
    (void)callback;
}

void glfwSetWindowSizeCallback(void* window, void* callback) {
    printf("glfwSetWindowSizeCallback called\n");
    (void)window;
    (void)callback;
}

void glfwTerminate() {
    printf("glfwTerminate called\n");
}

void glfwSwapBuffers(void* window) {
    printf("glfwSwapBuffers called\n");
    (void)window;
}

void glfwPollEvents() {
    printf("glfwPollEvents called\n");
}

void glfwWaitEvents() {
    printf("glfwWaitEvents called\n");
}

void glfwWaitEventsTimeout(double timeout) {
    printf("glfwWaitEventsTimeout called: timeout=%.2f\n", timeout);
    (void)timeout;
}

void glfwPostEmptyEvent() {
    printf("glfwPostEmptyEvent called\n");
}

void glfwSetCursorPosCallback(void* window, void* callback) {
    printf("glfwSetCursorPosCallback called\n");
    (void)window;
    (void)callback;
}

int glfwGetKey(void* window, int key) {
    printf("glfwGetKey called: key=%d\n", key);
    (void)window;
    (void)key;
    return 0; // Mock not pressed
}

#endif // TEST_MOCKS_GLFW_MOCK_H
