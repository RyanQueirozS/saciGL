#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-types.h"

#include <assert.h>

/* === Main Implementation === */

void sc_GL_ResizeVBO(saci_u32 vaoID, saci_u32 vboID, saci_u32 newCapacity,
                     saci_u64 vertexDataStructureSize) {
    glBindVertexArray(vaoID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, newCapacity * vertexDataStructureSize, NULL, GL_DYNAMIC_DRAW);

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
