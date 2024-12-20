#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

#include <assert.h>

#define __SC_VBO_UNBIND() glBindBuffer(GL_ARRAY_BUFFER, 0);

// TODO remove direct opengl usage outside of opengl only functions

/* === Main Implementation === */

void sc_GL_Resize_Vertex_Buffer(sa_u32_t vao_id, sa_u32_t vbo_id, sa_u64_t new_size) {
    glBindVertexArray(vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sa_SCAST_TO_m(sa_s64_t)(new_size), NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

sa_u32_t sc_GL_Create_Index_Buffer(sa_u32_t* indices, sa_u64_t indice_amount) {
    sa_u32_t ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sa_SCAST_TO_m(sa_s64_t)(indice_amount * sizeof(sa_u32_t)), &indices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    assert(ibo);
    return ibo;
}

// todo export
void sc_GL_Create_Vertex_Array(sa_u64_t size, sa_u32_t* arrays) {
    glGenVertexArrays(sa_SCAST_TO_m(int)(size), arrays);
}

void sc_GL_Bind_Vertex_Array(sa_u32_t array) {
    glBindVertexArray(array);
}

void sc_GL_Bind_Vertex_Buffer(sa_u32_t vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

sa_u32_t sc_GL_Create_Vertex_Buffer(sa_u64_t size, const void* data, sa_u32_t usage) {
    sa_u32_t vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sa_SCAST_TO_m(long int)(size), data, usage);
    __SC_VBO_UNBIND();
    return vbo;
}

void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32_t index, int size, sa_u32_t type, sa_bool_t normalized, sa_u64_t stride, void* ptr) {
    glVertexAttribPointer(index, size, type, normalized, sa_SCAST_TO_m(int)(stride), ptr);
}

void sc_GL_Enable_Vertex_Attrib_Array(sa_u32_t id) {
    glEnableVertexAttribArray(id);
}
