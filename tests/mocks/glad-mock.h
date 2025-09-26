#ifndef TEST_MOCKS_GLAD_MOCK_H
#define TEST_MOCKS_GLAD_MOCK_H
#include <stdio.h>

typedef void* (*GLADloadfunc)(const char* name);

int gladLoadGL(GLADloadfunc func) {
    printf("gladLoadGL called\n");
    (void)func;
    return 1;
}
#endif // TEST_MOCKS_GLAD_MOCK_H
