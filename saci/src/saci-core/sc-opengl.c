#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-types.h"

#include <assert.h>

#define __SC_VBO_UNBIND() glBindBuffer(GL_ARRAY_BUFFER, 0);

/* === Main Implementation === */

void sc_GL_ResizeVertexBuffer(saci_u32 vaoID, saci_u32 vboID, saci_u64 newSize) {
    glBindVertexArray(vaoID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, newSize, NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

saci_u32 sc_GL_CreateIndexBuffer(saci_u32* indices, saci_u64 indiceAmount) {
    saci_u32 ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceAmount * sizeof(saci_u32), &indices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    assert(ibo);
    return ibo;
}

// todo export
void sc_GL_CreateVertexArray(saci_u64 size, saci_u32* arrays) {
    return glGenVertexArrays(size, arrays);
}

void sc_GL_BindVertexArray(saci_u32 array) {
    return glBindVertexArray(array);
}

void sc_GL_BindVertexBuffer(saci_u32 vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

saci_u32 sc_GL_CreateVertexBuffer(saci_u64 size, const void* data, saci_u32 usage) {
    saci_u32 vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    __SC_VBO_UNBIND();
    return vbo;
}

void sc_GL_SetVertexAttribPointer(saci_u32 index, int size, saci_u32 type, saci_Bool normalized, saci_u64 stride, void* ptr) {
    glVertexAttribPointer(index, size, type, normalized, stride, ptr);
}

void sc_GL_EnableVertexAttribArray(saci_u32 id) {
    glEnableVertexAttribArray(id);
}
