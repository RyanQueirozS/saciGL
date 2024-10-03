#ifndef __SACI_SACIGL_SHADERING_H__
#define __SACI_SACIGL_SHADERING_H__

#include "saci-utils.h"

saci_ShaderID saciGL_CompileShaderV(const char* source);
saci_ShaderID saciGL_CompileShaderF(const char* source);
saci_ShaderID saciGL_CompileShaderG(const char* source);

saci_ShaderID saciGL_GetShaderProgram(saci_ShaderID vshader, saci_ShaderID fshader);
saci_ShaderID saciGL_GetShaderProgramg(saci_ShaderID vshader, saci_ShaderID fshader, saci_ShaderID gshader);

#endif
