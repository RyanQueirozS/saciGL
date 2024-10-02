#ifndef __SACI_SACIGL_SHADERING_H__
#define __SACI_SACIGL_SHADERING_H__

#include "saci-utils.h"

saci_u32 saciGL_CompileShaderV(const char* source);
saci_u32 saciGL_CompileShaderF(const char* source);
saci_u32 saciGL_CompileShaderG(const char* source);

saci_u32 saciGL_GetShaderProgram(saci_ShaderID vshader, saci_ShaderID fshader);
saci_u32 saciGL_GetShaderProgramg(saci_ShaderID vshader, saci_ShaderID fshader, saci_ShaderID gshader);

#endif
