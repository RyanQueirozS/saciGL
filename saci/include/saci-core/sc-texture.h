/**
 * @file sc_texture.h
 * @brief This header defines functions and structures for loading and managing textures.
 *
 * @details
 * This file includes the necessary functions and structures for loading texture data from
 * a file, managing texture memory, and providing utilities to handle textures in the SACI
 * engine.
 */

#ifndef __SACI_CORE_SC_TEXTURE_H__
#define __SACI_CORE_SC_TEXTURE_H__

#include "saci-utils/su-types.h"

/* === Texture Data Structure === */

/**
 * @struct sc_TextureData
 * @brief Structure to hold texture information and pixel data.
 *
 * @details
 * This structure is used to represent the raw texture data, including width, height,
 * number of channels, and a pointer to the actual image data.
 *
 * @var sc_TextureData::width
 * The width of the texture in pixels.
 *
 * @var sc_TextureData::height
 * The height of the texture in pixels.
 *
 * @var sc_TextureData::nrChannels
 * The number of color channels in the texture (e.g., 3 for RGB, 4 for RGBA).
 *
 * @var sc_TextureData::data
 * A pointer to the texture's raw pixel data.
 */
typedef struct sc_TextureData {
    int width;      /**< Width of the texture */
    int height;     /**< Height of the texture */
    int nrChannels; /**< Number of color channels (e.g., RGB or RGBA) */
    saci_u8* data;  /**< Pointer to the raw texture data */
} sc_TextureData;

/* === Texture Loading Functions === */

/**
 * @brief Loads texture data from a file into a sc_TextureData* without creating an OpenGL
 * texture.
 *
 * @param path The file path of the texture to load.
 * @param flipImg A boolean indicating whether the image should be flipped vertically.
 * @param texData A pointer to an sc_TextureData structure that will hold the loaded
 * texture information.
 */
void sc_Texture_LoadData(const char* path, saci_Bool flipImg, sc_TextureData* texData);

/**
 * @brief Loads a texture from a file and creates an OpenGL texture.
 *
 * @details
 * This function reads the image file at the specified path, uploads the texture to the
 * GPU, and returns a texture ID that can be used for rendering. It also supports vertical
 * flipping of the image.
 *
 * @param path The file path of the texture to load.
 * @param flipImg A boolean indicating whether the image should be flipped vertically.
 * @return A saci_TextureID representing the loaded texture in the GPU.
 */
saci_TextureID sc_Texture_Load(const char* path, saci_Bool flipImg);

/**
 * @brief Frees the memory associated with an OpenGL texture.
 *
 * @details
 * This function releases the texture associated with the given texture ID from the GPU,
 * freeing its memory.
 *
 * @param textureID The saci_TextureID of the texture to free.
 */
void sc_Texture_Free(saci_TextureID textureID);

#endif
