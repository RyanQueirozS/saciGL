#ifndef TEST_MOCKS_GL_MOCK_FUNC_POINTERS_H
#define TEST_MOCKS_GL_MOCK_FUNC_POINTERS_H

#include "gl-mock.h"
#include "saci-backend/sb-config-manager.h"

SA_INTERNAL struct sb_RenderApiFuncs func_pointers = {
    .gl = {
        .active_texture = glActiveTexture,
    },
};

#endif // TEST_MOCKS_GL_MOCK_FUNC_POINTERS_H
