/**
 * @file sc_shader.h
 * @brief This header defines functions for compiling shaders and creating shader programs in the SACI engine.
 *
 * @details
 * This file contains functions for compiling vertex, fragment, and geometry shaders from source code,
 * as well as functions for linking these shaders into shader programs that can be used in rendering.
 */

#ifndef __SACI_CORE_SC_SHADERING_H__
#define __SACI_CORE_SC_SHADERING_H__

#include "saci-utils/su-types.h"

/* === Shader Compilation Functions === */

/**
 * @brief Compiles a vertex shader from source code.
 *
 * @details
 * This function takes the source code for a vertex shader, compiles it, and returns the resulting shader ID.
 *
 * @param source The source code of the vertex shader as a string.
 * @return A saci_ShaderID representing the compiled vertex shader.
 */
/**
 * @brief Compiles a fragment shader from source code.
 *
 * @details
 * This function takes the source code for a fragment shader, compiles it, and returns the resulting shader ID.
 *
 * @param source The source code of the fragment shader as a string.
 * @return A saci_ShaderID representing the compiled fragment shader.
 */
saci_u32 sc_CompileShaderF(const char* source);

/**
 * @brief Compiles a geometry shader from source code.
 *
 * @details
 * This function takes the source code for a geometry shader, compiles it, and returns the resulting shader ID.
 *
 * @param source The source code of the geometry shader as a string.
 * @return A saci_ShaderID representing the compiled geometry shader.
 */
saci_u32 sc_CompileShaderG(const char* source);

/* === Shader Program Linking Functions === */

/**
 * @brief Links a vertex shader and fragment shader into a shader program.
 *
 * @details
 * This function takes compiled vertex and fragment shaders, links them into a shader program, and returns the program ID.
 *
 * @param vshader The saci_ShaderID of the compiled vertex shader.
 * @param fshader The saci_ShaderID of the compiled fragment shader.
 * @return A saci_u32 representing the shader program ID.
 */
saci_u32 sc_GetShaderProgram(saci_ShaderID vshader, saci_ShaderID fshader);

/**
 * @brief Links a vertex, fragment, and geometry shader into a shader program.
 *
 * @details
 * This function takes compiled vertex, fragment, and geometry shaders, links them into a shader program, and returns the program ID.
 *
 * @param vshader The saci_ShaderID of the compiled vertex shader.
 * @param fshader The saci_ShaderID of the compiled fragment shader.
 * @param gshader The saci_ShaderID of the compiled geometry shader.
 * @return A saci_u32 representing the shader program ID.
 */
saci_u32 sc_GetShaderProgramg(saci_ShaderID vshader, saci_ShaderID fshader, saci_ShaderID gshader);

#endif
