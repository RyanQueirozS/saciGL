/**
 * @file sc-rendering.h
 * @brief This header defines graphical API related functions and structs related to saciCORE
 */
#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

#include <GLFW/glfw3.h>

/* === Windowing === */

/**
 * @typedef sc_Monitor
 * @brief GLFWmonitor wrapper.
 * @note, use this instead of GLFWmonitor, as it may change later.
 */
typedef GLFWmonitor sc_Monitor;

/**
 * @typedef sc_Window
 * @brief GLFWwindow wrapper.
 * @note, use this instead of GLFWwindow, as it may change later.
 */
typedef GLFWwindow sc_Window;

/**
 * @typedef sc_WindowPosHandler
 * @brief Function pointer type for handling window position changes.
 *
 * @param window The window whose position has changed.
 * @param posx The new X position of the window.
 * @param posy The new Y position of the window.
 */
typedef void (*sc_Window_PosHandler)(sc_Window* window, int posx, int posy);

/**
 * @typedef sc_WindowSizeHandler
 * @brief Function pointer type for handling window size changes.
 *
 * @param window The window whose size has changed.
 * @param width The new width of the window.
 * @param height The new height of the window.
 */
typedef void (*sc_Window_SizeHandler)(sc_Window* window, int width, int height);

/**
 * @brief Initializes the GLFW library for window management.
 *
 * @return SACI_TRUE if initialization was successful, SACI_FALSE otherwise.
 */
saci_Bool sc_GLFW_Init(void);

/**
 * @brief Initializes the GLAD library for OpenGL function loading.
 *
 * @return SACI_TRUE if initialization was successful, SACI_FALSE otherwise.
 */
saci_Bool sc_GLAD_Init(void);

/**
 * @brief Creates a new window.
 *
 * @param width The desired width of the window.
 * @param height The desired height of the window.
 * @param title The title of the window.
 * @param monitor The monitor to display the window on (use NULL for the primary monitor).
 * @param share The window to share resources with (use NULL for no sharing).
 * @return A pointer to the created sc_Window. This function does not check if the window
 * is null.
 */
sc_Window* sc_Window_Create(int width, int height, const char* title, sc_Monitor* monitor,
                            sc_Window* share);

// TODO doc
void sc_Window_Free(sc_Window* window);

/**
 * @brief Makes the specified window the current OpenGL context.
 *
 * @param window The window to make the current context.
 * @note Needed to initialize GLAD.
 */
void sc_Window_MakeContext(sc_Window* window);

/**
 * @brief Checks if the window should close.
 *
 * @param window The window to check.
 * @return SACI_TRUE if the window should close, SACI_FALSE otherwise.
 */
saci_Bool sc_Window_ShouldClose(sc_Window* window);

/**
 * @brief Sets a callback function for handling window position changes.
 *
 * @param window The window to set the position handler for.
 * @param windowPosHandler The callback function to handle position changes.
 */
void sc_Window_SetPosHandler(sc_Window* window, sc_Window_PosHandler windowPosHandler);

/**
 * @brief Sets a callback function for handling window size changes.
 *
 * @param window The window to set the size handler for.
 * @param windowSizeHandler The callback function to handle size changes.
 */
void sc_Window_SetSizeHandler(sc_Window* window, sc_Window_SizeHandler windowSizeHandler);

/**
 * @brief Terminates the windowing system, cleaning up resources.
 */
void sc_Window_Terminate(void);

/**
 * @brief Clears the window with the specified color.
 *
 * @param color The saci_Color to clear the window with.
 */
void sc_Window_ClearColor(const saci_Color color);

/**
 * @brief Swaps the window buffer to display rendered content.
 *
 * @param window The window whose buffer will be swapped.
 * @note If not called, nothing will show on screen.
 */
void sc_Window_SwapBuffer(sc_Window* window);

/* === Renderer === */

/**
 * @struct sc_Renderer
 * @brief Structure to hold render related information.
 *
 * @details
 * This structure is used to hold vertex information in form of triangles, squares or
 * lines. These shapes are later rendered to the screen.
 */
typedef struct sc_Renderer sc_Renderer;

/**
 * @struct sc_Renderer
 * @brief Structure to hold vertice related information.
 */
typedef struct sc_Vertice sc_Vertice;

// TODO doc
saci_Vec3  sc_Vertice_GetPos(const sc_Vertice* vertice);
saci_Color sc_Vertice_GetColor(const sc_Vertice* vertice);
saci_Vec2  sc_Vertice_GetTexcoord(const sc_Vertice* vertice);

// TODO doc
sc_Vertice* sc_Vertice_CreateVertice(saci_Vec3 position, saci_Color color, saci_Vec2 texcood);

void sc_Vertice_GetArrayInfo(sc_Vertice* vertexArray, saci_u64 vertexArraySize,
                             saci_Vec3** positions, saci_Color** colors, saci_Vec2** texcoords);

sc_Vertice* sc_Vertice_CreateVerticesArray(saci_Vec3* positions, saci_Color* colors,
                                           saci_Vec2* texcoords, saci_u64 amount);

/**
 * @struct sc_ModelMesh
 * @brief Structure to hold model related information.
 */
typedef struct sc_ModelMesh sc_ModelMesh;

// TODO doc
// TODO create a const alternative
sc_Vertice* sc_ModelMesh_GetVertices(const sc_ModelMesh* modelMesh);

saci_u64 sc_ModelMesh_GetVerticesAmount(const sc_ModelMesh* modelMesh);

saci_u32* sc_ModelMesh_GetIndices(const sc_ModelMesh* modelMesh);

saci_u64 sc_ModelMesh_GetIndicesAmount(const sc_ModelMesh* modelMesh);

/**
 * @brief Creates the sc_Renderer struct
 *
 * @details
 * This functions creates, initializes and sets all of the sc_Renderer related shaders and
 * OpenGL context.
 *
 * @param generateDefaults A boolean to generate defaulted shaders and OpenGL context.
 * @return A new sc_Renderer* either defaulted or not. Can return null
 */
sc_Renderer* sc_Renderer_Create(saci_Bool generateDefaults);

/**
 * @brief Deletes the sc_Renderer struct
 *
 * @details
 * This functions deletes a sc_Renderer
 *
 * @param renderer The renderer to be deleted
 */
void sc_Renderer_Delete(sc_Renderer* renderer);

/**
 * @brief Sets renderer to not fill shapes
 */
void sc_Renderer_SetNoFillMode(void);

/**
 * @brief Sets renderer to fill shapes
 * @note This is the defaulted option.
 */
void sc_Renderer_SetFillMode(void);

/**
 * @brief Enables the Z buffer.
 *
 * @details
 * Check OpenGL article: https://learnopengl.com/Advanced-OpenGL/Depth-testing
 */
void sc_Renderer_EnableZBuffer(void);

/**
 * @enum sc_RenderProjectionType
 * @brief Types of Projection that a sc_Renderer can use.
 */
typedef enum sc_RenderProjectionMode {
    SACI_RENDER_ORTHOGRAPHIC_PROJECTION = 0,
    SACI_RENDER_PERSPECTIVE_PROJECTION  = 1,
    SACI_RENDER_CUSTOM_PROJECTION       = 2,
} sc_RendererProjectionMode;

/**
 * @brief Sets projection mode
 *
 * @param renderProjectionMode The sc_RenderProjectionMode that the sc_Renderer should
 * use.
 */
void sc_Renderer_SetProjectionMode(sc_RendererProjectionMode renderProjectionMode);

/**
 * @typedef sc_RenderCustomProjectionFunction
 * @brief A function pointer that's used for custom projections
 *
 * @param camera The sc_Camera struct used to setup projection
 * @return A saci_Mat4 with the projection values
 */
typedef saci_Mat4 (*sc_Renderer_CustomProjectionFunction)(sc_Camera camera);

/**
 * @brief Sets custom projection mode function.
 *
 * @param renderCustomProjectionModeFunction The sc_RenderProjectionMode that the
 * sc_Renderer should use.
 */
void sc_Renderer_SetCustomProjectionModeFunction(
    sc_Renderer_CustomProjectionFunction renderCustomProjectionModeFunction);

/**
 * @brief Sets renderer to begins rendering the frame
 *
 * @param renderer The renderer to setup
 */
void sc_Renderer_Begin(sc_Renderer* renderer);

/**
 * @brief Draws the RenderCalls in the sc_Renderer
 *
 * @param renderer The renderer to setup.
 * @param camera The camera to setup rendering enviroment.
 */
void sc_Renderer_End(sc_Renderer* renderer, const sc_Camera* camera);

// TODO doc
void sc_Renderer_PushVertices(sc_Renderer* renderer, sc_Vertice* vertices, saci_u64 verticeAmount,
                              saci_u64 indiceAmount, saci_Mat4 modelMatrix,
                              saci_TextureID texID, saci_u32 ibo);

/**
 * @brief Pushes a model to the @ref sc_Renderer.
 *
 * @param renderer The renderer that will get data pushed.
 * @param mesh The mesh that will get pushed
 * @param modelMatrix The model matrix to explain how to draw the model.
 * @param texID The OpenGL index of the texture.
 */
void sc_Renderer_PushModelMesh(sc_Renderer* renderer, sc_ModelMesh* mesh, saci_Mat4 modelMatrix,
                               saci_TextureID texID);

/* === Model Creation === */

// todo doc
typedef void (*sc_OBJ_ModelFileReadingFunction)(void* ctx, const char* filename, int isMtl,
                                                const char* objFilename, char** buf, size_t* len);

/**
 * @brief Creates a @ref sc_ModelMesh containing the info in the provided path.
 *
 * @param path The path for the model file.
 * @param fileReader The file reading function to load the model information.
 */
sc_ModelMesh* sc_ModelMesh_Load(const char* path, sc_OBJ_ModelFileReadingFunction fileReader);

/**
 * @brief Structure to hold Vertex Indice information.
 *
 * @details
 * In general, only use this if you want to load OBJ files manually. NOT RECOMENDED.
 *
 * @internal
 * Do NOT typedef this, see CONVENTIONS.md
 */
struct sc_VertexIndice {
    saci_u32 vertexIndex;   /**< Indices of the vertice positions */
    saci_u32 texCoordIndex; /**< Indices of the texcoord values */
    saci_u32 normalIndex;   /**< Indices of the normal values */
};

// todo doc
saci_Bool sc_OBJ_Parse(const char* filePath, sc_OBJ_ModelFileReadingFunction fileReader,
                       saci_Vec3** positions, saci_u64* positionsCount, saci_Vec2** texcoords,
                       saci_u64* texcoordCount, struct sc_VertexIndice** indices,
                       saci_u64* indicesCount);

/**
 * @brief Frees a model mesh from memory.
 *
 * @param modelMesh The model mesh to be freed.
 */
void sc_ModelMesh_Delete(sc_ModelMesh* modelMesh);

/* === OpenGL Helpers === */

/**
 * @brief Creates a IBO (Index buffer).
 *
 * @param indices The indices to fill the IBO.
 * @param indiceAmount The amount of indices.
 */
saci_u32 sc_GL_CreateIndexBuffer(saci_u32* indices, saci_u64 indiceAmount);

/**
 * @brief resizes the VBO in the renderer to a newCapacity
 *
 * @param vaoID The ID of the vao that will get it's vbo updated
 * @param vboID The ID of the vbo that will get it's size updated
 * @param newCapacity The VBO's new capacity.
 * @param vertexDataStructureSize The size of the base structure that holds the vertex information
 */
void sc_GL_ResizeVBO(saci_u32 vaoID, saci_u32 vboID, saci_u32 newCapacity,
                     saci_u64 vertexDataStructureSize);

/* === Shader Functions === */

/**
 * @brief Compiles a vertex shader from source code.
 *
 * @details
 * This function takes the source code for a vertex shader, compiles it, and returns the
 * resulting shader ID.
 *
 * @param source The source code of the vertex shader as a string.
 * @return A saci_ShaderID representing the compiled vertex shader.
 */
saci_u32 sc_Shader_CompileShaderV(const char* source);

/**
 * @brief Compiles a fragment shader from source code.
 *
 * @details
 * This function takes the source code for a fragment shader, compiles it, and returns the
 * resulting shader ID.
 *
 * @param source The source code of the fragment shader as a string.
 * @return A saci_ShaderID representing the compiled fragment shader.
 */
saci_u32 sc_Shader_CompileShaderF(const char* source);

/**
 * @brief Compiles a geometry shader from source code.
 *
 * @details
 * This function takes the source code for a geometry shader, compiles it, and returns the
 * resulting shader ID.
 *
 * @param source The source code of the geometry shader as a string.
 * @return A saci_ShaderID representing the compiled geometry shader.
 */
saci_u32 sc_Shader_CompileShaderG(const char* source);

/**
 * @brief Links a vertex shader and fragment shader into a shader program.
 *
 * @details
 * This function takes compiled vertex and fragment shaders, links them into a shader
 * program, and returns the program ID.
 *
 * @param vshader The saci_ShaderID of the compiled vertex shader.
 * @param fshader The saci_ShaderID of the compiled fragment shader.
 * @return A saci_u32 representing the shader program ID.
 */
saci_u32 sc_Shader_GetShaderProgram(saci_ShaderID vshader, saci_ShaderID fshader);

/**
 * @brief Links a vertex, fragment, and geometry shader into a shader program.
 *
 * @details
 * This function takes compiled vertex, fragment, and geometry shaders, links them into a
 * shader program, and returns the program ID.
 *
 * @param vshader The saci_ShaderID of the compiled vertex shader.
 * @param fshader The saci_ShaderID of the compiled fragment shader.
 * @param gshader The saci_ShaderID of the compiled geometry shader.
 * @return A saci_u32 representing the shader program ID.
 */
saci_u32 sc_Shader_GetShaderProgramg(saci_ShaderID vshader, saci_ShaderID fshader,
                                     saci_ShaderID gshader);

#endif
