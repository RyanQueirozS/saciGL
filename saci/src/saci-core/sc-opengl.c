#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-types.h"

#include <assert.h>

#define __SC_VBO_UNBIND() glBindBuffer(GL_ARRAY_BUFFER, 0);

/* === Main Implementation === */

void sc_GL_Resize_Vertex_Buffer(sa_U32_t vaoID, sa_U32_t vboID, sa_U64_t newSize) {
    glBindVertexArray(vaoID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, newSize, NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

sa_U32_t sc_GL_Create_Index_Buffer(sa_U32_t* indices, sa_U64_t indiceAmount) {
    sa_U32_t ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceAmount * sizeof(sa_U32_t), &indices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    assert(ibo);
    return ibo;
}

// todo export
void sc_GL_Create_Vertex_Array(sa_U64_t size, sa_U32_t* arrays) {
    return glGenVertexArrays(size, arrays);
}

void sc_GL_Bind_Vertex_Array(sa_U32_t array) {
    return glBindVertexArray(array);
}

void sc_GL_Bind_Vertex_Buffer(sa_U32_t vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

sa_U32_t sc_GL_Create_Vertex_Buffer(sa_U64_t size, const void* data, sa_U32_t usage) {
    sa_U32_t vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    __SC_VBO_UNBIND();
    return vbo;
}

void sc_GL_Set_Vertex_Attrib_Pointer(sa_U32_t index, int size, sa_U32_t type, sa_Bool_t normalized, sa_U64_t stride, void* ptr) {
    glVertexAttribPointer(index, size, type, normalized, stride, ptr);
}

void sc_GL_Enable_Vertex_Attrib_Array(sa_U32_t id) {
    glEnableVertexAttribArray(id);
}
