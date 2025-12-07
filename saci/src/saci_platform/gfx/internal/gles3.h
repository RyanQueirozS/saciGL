#ifndef SACI_BACKEND_GRAPHICS_SB_EMSDK_H
#define SACI_BACKEND_GRAPHICS_SB_EMSDK_H

// #include <emscripten.h>

#include "saci_util/defines.h"
#include "saci_util/types.h"

// Draw

SACI_API void psaci_emsdk_clear_color(const SaciColor color);

// Shader

SACI_API SaciShaderId psaci_emsdk_shader_compile_shader_vert(const char* source);

SACI_API SaciShaderId psaci_emsdk_shader_compile_shader_frag(const char* source);

SACI_API SaciShaderId psaci_emsdk_shader_create_shader_program(SaciShaderId vshader, SaciShaderId fshader);

SACI_API SaciShaderId psaci_emsdk_shader_create_program_code(const char* v, const char* f);

#endif // SACI_BACKEND_SACI_GRAPHICS_SB_EMSDK_GL_H
