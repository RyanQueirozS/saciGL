#ifndef SACI_BACKEND_GRAPHICS_SB_EMSDK_H
#define SACI_BACKEND_GRAPHICS_SB_EMSDK_H

// #include <emscripten.h>

#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"

SA_API su_ShaderId sb_emsdk_shader_compile_shader_vert(const char* source);

SA_API su_ShaderId sb_emsdk_shader_compile_shader_frag(const char* source);

SA_API su_ShaderId sb_emsdk_shader_create_shader_program(su_ShaderId vshader, su_ShaderId fshader);

SA_API su_ShaderId sb_emsdk_shader_create_program_code(const char* v, const char* f);

#endif // SACI_BACKEND_SACI_GRAPHICS_SB_EMSDK_GL_H
