#ifndef __SACI_CORE_SC_SHADERING_H__
#define __SACI_CORE_SC_SHADERING_H__

#include "saci-utils/su-types.h"

saci_u32 sc_CompileShaderV(const char* source);
saci_u32 sc_CompileShaderF(const char* source);
saci_u32 sc_CompileShaderG(const char* source);

saci_u32 sc_GetShaderProgram(saci_ShaderID vshader, saci_ShaderID fshader);
saci_u32 sc_GetShaderProgramg(saci_ShaderID vshader, saci_ShaderID fshader, saci_ShaderID gshader);

#endif
