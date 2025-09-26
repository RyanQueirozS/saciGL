#ifndef TEST_MOCKS_GL_MOCK_H
#define TEST_MOCKS_GL_MOCK_H

#include <stdio.h>

void glClearColor(float r, float g, float b, float a) {
    printf("glClearColor called: %.2f, %.2f, %.2f, %.2f\n", r, g, b, a);
    (void)r;
    (void)g;
    (void)b;
    (void)a;
}

void glClear(unsigned int mask) {
    printf("glClear called: 0x%X\n", mask);
    (void)mask;
}

void glEnable(unsigned int cap) {
    printf("glEnable called: 0x%X\n", cap);
    (void)cap;
}

void glDebugMessageCallback(void* callback, void* userParam) {
    printf("glDebugMessageCallback called\n");
    (void)callback;
    (void)userParam;
}

const unsigned char* glGetString(unsigned int name) {
    printf("glGetString called: 0x%X\n", name);
    (void)name;
    static const unsigned char* version = (const unsigned char*)"4.6 Mock";
    return version;
}

void glUniform1ui(int location, unsigned int v0) {
    printf("glUniform1ui called: location=%d, v0=%u\n", location, v0);
    (void)location;
    (void)v0;
}

void glUniform1i(int location, int v0) {
    printf("glUniform1i called: location=%d, v0=%d\n", location, v0);
    (void)location;
    (void)v0;
}

void glUniform2f(int location, float v0, float v1) {
    printf("glUniform2f called: location=%d, v0=%.2f, v1=%.2f\n", location, v0, v1);
    (void)location;
    (void)v0;
    (void)v1;
}

void glUniform3f(int location, float v0, float v1, float v2) {
    printf("glUniform3f called: location=%d, v0=%.2f, v1=%.2f, v2=%.2f\n", location, v0, v1, v2);
    (void)location;
    (void)v0;
    (void)v1;
    (void)v2;
}

void glUniform4f(int location, float v0, float v1, float v2, float v3) {
    printf("glUniform4f called: location=%d, v0=%.2f, v1=%.2f, v2=%.2f, v3=%.2f\n", location, v0, v1, v2, v3);
    (void)location;
    (void)v0;
    (void)v1;
    (void)v2;
    (void)v3;
}

void glUniformMatrix2fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix2fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix3fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix3fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix4fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix4fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix2x3fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix2x3fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix2x4fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix2x4fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix3x2fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix3x2fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix3x4fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix3x4fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix4x2fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix4x2fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glUniformMatrix4x3fv(int location, int count, unsigned char transpose, const float* value) {
    printf("glUniformMatrix4x3fv called: location=%d, count=%d, transpose=%d\n", location, count, transpose);
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glBindVertexArray(unsigned int array) {
    printf("glBindVertexArray called: %u\n", array);
    (void)array;
}

void glBindBuffer(unsigned int target, unsigned int buffer) {
    printf("glBindBuffer called: target=0x%X, buffer=%u\n", target, buffer);
    (void)target;
    (void)buffer;
}

void glBufferData(unsigned int target, long long size, const void* data, unsigned int usage) {
    printf("glBufferData called: target=0x%X, size=%lld, usage=0x%X\n", target, size, usage);
    (void)target;
    (void)size;
    (void)data;
    (void)usage;
}

void glBufferSubData(unsigned int target, long long offset, long long size, const void* data) {
    printf("glBufferSubData called: target=0x%X, offset=%lld, size=%lld\n", target, offset, size);
    (void)target;
    (void)offset;
    (void)size;
    (void)data;
}

void glGenBuffers(int n, unsigned int* buffers) {
    printf("glGenBuffers called: n=%d\n", n);
    (void)n;
    if (buffers && n > 0)
        buffers[0] = 1; // Mock buffer ID
}

void glGenVertexArrays(int n, unsigned int* arrays) {
    printf("glGenVertexArrays called: n=%d\n", n);
    (void)n;
    if (arrays && n > 0)
        arrays[0] = 1; // Mock VAO ID
}

void glVertexAttribPointer(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer) {
    printf("glVertexAttribPointer called: index=%u, size=%d, type=0x%X\n", index, size, type);
    (void)index;
    (void)size;
    (void)type;
    (void)normalized;
    (void)stride;
    (void)pointer;
}

void glVertexAttribDivisor(unsigned int index, unsigned int divisor) {
    printf("glVertexAttribDivisor called: index=%u, divisor=%u\n", index, divisor);
    (void)index;
    (void)divisor;
}

void glEnableVertexAttribArray(unsigned int index) {
    printf("glEnableVertexAttribArray called: index=%u\n", index);
    (void)index;
}

unsigned int glCreateShader(unsigned int type) {
    printf("glCreateShader called: type=0x%X\n", type);
    (void)type;
    return 1; // Mock shader ID
}

void glShaderSource(unsigned int shader, int count, const char** string, const int* length) {
    printf("glShaderSource called: shader=%u, count=%d\n", shader, count);
    (void)shader;
    (void)count;
    (void)string;
    (void)length;
}

void glCompileShader(unsigned int shader) {
    printf("glCompileShader called: shader=%u\n", shader);
    (void)shader;
}

void glGetShaderiv(unsigned int shader, unsigned int pname, int* params) {
    printf("glGetShaderiv called: shader=%u, pname=0x%X\n", shader, pname);
    (void)shader;
    (void)pname;
    if (params)
        *params = 1; // Mock success
}

void glGetShaderInfoLog(unsigned int shader, int bufSize, int* length, char* infoLog) {
    printf("glGetShaderInfoLog called: shader=%u, bufSize=%d\n", shader, bufSize);
    (void)shader;
    (void)bufSize;
    if (length)
        *length = 0;
    if (infoLog && bufSize > 0)
        infoLog[0] = '\0';
}

void glDeleteShader(unsigned int shader) {
    printf("glDeleteShader called: shader=%u\n", shader);
    (void)shader;
}

void glGetIntegerv(unsigned int pname, int* data) {
    printf("glGetIntegerv called: pname=0x%X\n", pname);
    (void)pname;
    if (data)
        *data = 800; // Mock value
}

void glGetBooleanv(unsigned int pname, unsigned char* data) {
    printf("glGetBooleanv called: pname=0x%X\n", pname);
    (void)pname;
    if (data)
        *data = 1; // Mock true
}

void glBlendFunc(unsigned int sfactor, unsigned int dfactor) {
    printf("glBlendFunc called: sfactor=0x%X, dfactor=0x%X\n", sfactor, dfactor);
    (void)sfactor;
    (void)dfactor;
}

unsigned int glGetError() {
    printf("glGetError called\n");
    return 0; // Mock no error
}

unsigned int glCreateProgram() {
    printf("glCreateProgram called\n");
    return 1; // Mock program ID
}

void glUseProgram(unsigned int program) {
    printf("glUseProgram called: program=%u\n", program);
    (void)program;
}

void glAttachShader(unsigned int program, unsigned int shader) {
    printf("glAttachShader called: program=%u, shader=%u\n", program, shader);
    (void)program;
    (void)shader;
}

void glLinkProgram(unsigned int program) {
    printf("glLinkProgram called: program=%u\n", program);
    (void)program;
}

void glGetProgramiv(unsigned int program, unsigned int pname, int* params) {
    printf("glGetProgramiv called: program=%u, pname=0x%X\n", program, pname);
    (void)program;
    (void)pname;
    if (params)
        *params = 1; // Mock success
}

void glGetProgramInfoLog(unsigned int program, int bufSize, int* length, char* infoLog) {
    printf("glGetProgramInfoLog called: program=%u, bufSize=%d\n", program, bufSize);
    (void)program;
    (void)bufSize;
    if (length)
        *length = 0;
    if (infoLog && bufSize > 0)
        infoLog[0] = '\0';
}

void glDetachShader(unsigned int program, unsigned int shader) {
    printf("glDetachShader called: program=%u, shader=%u\n", program, shader);
    (void)program;
    (void)shader;
}

void glDrawElementsInstanced(unsigned int mode, int count, unsigned int type, const void* indices, int instancecount) {
    printf("glDrawElementsInstanced called: mode=0x%X, count=%d, instances=%d\n", mode, count, instancecount);
    (void)mode;
    (void)count;
    (void)type;
    (void)indices;
    (void)instancecount;
}

void glDrawElements(unsigned int mode, int count, unsigned int type, const void* indices) {
    printf("glDrawElements called: mode=0x%X, count=%d\n", mode, count);
    (void)mode;
    (void)count;
    (void)type;
    (void)indices;
}

int glGetUniformLocation(unsigned int program, const char* name) {
    printf("glGetUniformLocation called: program=%u, name=%s\n", program, name);
    (void)program;
    (void)name;
    return 1; // Mock location
}

void glGenTextures(int n, unsigned int* textures) {
    printf("glGenTextures called: n=%d\n", n);
    (void)n;
    if (textures && n > 0)
        textures[0] = 1; // Mock texture ID
}

void glBindTexture(unsigned int target, unsigned int texture) {
    printf("glBindTexture called: target=0x%X, texture=%u\n", target, texture);
    (void)target;
    (void)texture;
}

void glActiveTexture(unsigned int texture) {
    printf("glActiveTexture called: texture=%u\n", texture);
    (void)texture;
}

void glTexImage2D(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* data) {
    printf("glTexImage2D called: target=0x%X, level=%d, size=%dx%d\n", target, level, width, height);
    (void)target;
    (void)level;
    (void)internalformat;
    (void)width;
    (void)height;
    (void)border;
    (void)format;
    (void)type;
    (void)data;
}

void glGetTexLevelParameteriv(unsigned int target, int level, unsigned int pname, int* params) {
    printf("glGetTexLevelParameteriv called: target=0x%X, level=%d, pname=0x%X\n", target, level, pname);
    (void)target;
    (void)level;
    (void)pname;
    if (params)
        *params = 1024; // Mock texture size
}

void glGenerateMipmap(unsigned int target) {
    printf("glGenerateMipmap called: target=0x%X\n", target);
    (void)target;
}

void glDeleteTextures(int n, const unsigned int* textures) {
    printf("glDeleteTextures called: n=%d\n", n);
    (void)n;
    (void)textures;
}
#endif // TEST_MOCKS_GL_MOCK_H
