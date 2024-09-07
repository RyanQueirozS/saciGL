#include "../saci/core.h"
#include "../saci/shapes.h"

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_VERTICES_RECTANGLE 12
#define MAX_INDICES_RECTANGLE 6

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

typedef struct Renderer {
    uint32_t vao, vbo, ebo;

    uint32_t shaderProgram;
} Renderer;
static Renderer renderer;

void InitRenderer() {
    glGenVertexArrays(1, &renderer.vao);
    glGenBuffers(1, &renderer.vbo);
    glGenBuffers(1, &renderer.ebo);

    glBindVertexArray(renderer.vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_RECTANGLE * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.ebo);
    uint32_t indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attribute setup
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DrawRectangle() {
    glUseProgram(renderer.shaderProgram);
    glBindVertexArray(renderer.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void updateRectangles(const saci_Rect rect, uint32_t vbo) {
    const float windowWidth = 700.0f;
    const float windowHeight = 700.0f;

    // Convert the rectangle's pixel position and size to NDC
    float rectX = (2.0f * rect.x / windowWidth) - 1.0f;
    float rectY = (2.0f * rect.y / windowHeight) - 1.0f;
    float rectWidth = (2.0f * rect.width / windowWidth);
    float rectHeight = (2.0f * rect.height / windowHeight);

    // Add randomness symmetrically (optional)
    float randomx = (float)(rand() % 10) / 10;
    float randomy = (float)(rand() % 10) / 10;
    float randomh = (float)(rand() % 10) / 10;
    float randomw = (float)(rand() % 10) / 10;

    rectX += randomx;  // Apply randomness to the entire rectangle
    rectY += randomy;
    rectWidth += randomw;
    rectHeight += randomh;

    float vertices[] = {
        rectX + rectWidth, rectY, 0.0f,               // top right
        rectX + rectWidth, rectY + rectHeight, 0.0f,  // bottom right
        rectX, rectY + rectHeight, 0.0f,              // bottom left
        rectX, rectY, 0.0f,                           // top left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);  // Update buffer with new vertices
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main(void) {
    saciWindowSpecs windowSpecs = {700, 700, "title", NULL, NULL};

    saciWindow* window = saciCreateWindow(windowSpecs);
    assert(window != NULL);

    // TODO link dynamically
    uint32_t vShader = saciLoadShaderFromPath("../examples/shaders/rectangle.vert", GL_VERTEX_SHADER);
    uint32_t fShader = saciLoadShaderFromPath("../examples/shaders/rectangle.frag", GL_FRAGMENT_SHADER);
    assert(vShader != 0 && fShader != 0);

    uint32_t shaderArr[] = {vShader, fShader};
    renderer.shaderProgram = saciCreateProgram(shaderArr, 2);
    assert(renderer.shaderProgram != 0);
    InitRenderer();

    saci_Rect rect = {50, 50, 20, 30};

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Update vertices if geometry changes
        updateRectangles(rect, renderer.vbo);
        saciClearCanvas(saciColorFromHex(0x00000000));
        DrawRectangle();

        // Render
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &renderer.vbo);
    glDeleteBuffers(1, &renderer.ebo);
    glDeleteVertexArrays(1, &renderer.vao);

    saciTerminate();

    return 0;
}
