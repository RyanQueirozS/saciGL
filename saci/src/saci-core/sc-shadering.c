#include <glad/glad.h>
#include <stdio.h>

#include "saci-core.h"
#include "saci-utils/su-types.h"

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_u32 __sc_compileShader(const char* shaderSource, saci_u32 shaderType);

//----------------------------------------------------------------------------//

saci_u32 sc_CompileShaderV(const char* source) {
    return __sc_compileShader(source, GL_VERTEX_SHADER);
}

saci_u32 sc_CompileShaderF(const char* source) {
    return __sc_compileShader(source, GL_FRAGMENT_SHADER);
}

saci_u32 sc_CompileShaderG(const char* source) {
    return __sc_compileShader(source, GL_GEOMETRY_SHADER);
}

saci_u32 sc_GetShaderProgram(saci_u32 vshader, saci_u32 fshader) {
    saci_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glLinkProgram(programID);

    saci_s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, errMessage);
        printf("ERROR: Could not link shader\n%s", errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programID;
}

saci_u32 sc_GetShaderProgramg(saci_u32 vshader, saci_u32 fshader, saci_u32 gshader) {
    saci_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glAttachShader(programID, gshader);
    glLinkProgram(programID);

    saci_s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, errMessage);
        printf("ERROR: Could not link shader\n%s", errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDetachShader(programID, gshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteShader(gshader);

    return programID;
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_u32 __sc_compileShader(const char* shaderSource, saci_u32 shaderType) {
    saci_u32 shaderID = glCreateShader(shaderType);

    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetShaderInfoLog(shaderID, 2048, &sizeReturned, &errMessage[0]);

        glDeleteShader(shaderID);
        printf("ERROR: Shader compilation failed: %s\n", errMessage);
        return 0;
    }

    return shaderID;
}
