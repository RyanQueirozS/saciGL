#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

#include <GLFW/glfw3.h>

/* === Windowing === */

typedef GLFWmonitor sc_Monitor;

typedef GLFWwindow  sc_Window;

typedef void (*sc_Window_PosHandler)(sc_Window* window, int posx, int posy);

typedef void (*sc_Window_SizeHandler)(sc_Window* window, int width, int height);

saci_Bool  sc_GLFW_Init(void);

saci_Bool  sc_GLAD_Init(void);

sc_Window* sc_Window_Create(int width, int height, const char* title,
                            sc_Monitor* monitor, sc_Window* share);

void       sc_Window_Free(sc_Window* window);

void       sc_Window_MakeContext(sc_Window* window);

saci_Bool  sc_Window_ShouldClose(sc_Window* window);

void       sc_Window_SetPosHandler(sc_Window* window, sc_Window_PosHandler windowPosHandler);

void       sc_Window_SetSizeHandler(sc_Window* window, sc_Window_SizeHandler windowSizeHandler);

void       sc_Window_Terminate(void);

void       sc_Window_ClearColor(const saci_Color color);

void       sc_Window_SwapBuffer(sc_Window* window);

/* === Renderer === */

typedef struct sc_Renderer  sc_Renderer;

typedef struct sc_Vertice   sc_Vertice;

typedef struct sc_ModelMesh sc_ModelMesh;

typedef saci_Mat4 (*sc_Renderer_CustomProjectionFunction)(sc_Camera camera);

typedef enum sc_RenderProjectionMode {
    SACI_RENDER_ORTHOGRAPHIC_PROJECTION = 0,
    SACI_RENDER_PERSPECTIVE_PROJECTION,
    SACI_RENDER_CUSTOM_PROJECTION,
} sc_RendererProjectionMode;

saci_Vec3     sc_Vertice_GetPos(const sc_Vertice* vertice);

saci_Color    sc_Vertice_GetColor(const sc_Vertice* vertice);

saci_Vec2     sc_Vertice_GetTexcoord(const sc_Vertice* vertice);

sc_Vertice*   sc_Vertice_CreateVertice(saci_Vec3 position, saci_Color color, saci_Vec2 texcood);

void          sc_Vertice_GetArrayInfo(sc_Vertice* vertexArray, saci_u64 vertexArraySize,
                                      saci_Vec3**  positions,
                                      saci_Color** colors,
                                      saci_Vec2**  texcoords);

sc_Vertice*   sc_Vertice_CreateVerticesArray(saci_Vec3* positions, saci_Color* colors,
                                             saci_Vec2* texcoords, saci_u64 amount);

sc_Vertice*   sc_ModelMesh_GetVertices(const sc_ModelMesh* modelMesh);

saci_u64      sc_ModelMesh_GetVerticesAmount(const sc_ModelMesh* modelMesh);

saci_u32*     sc_ModelMesh_GetIndices(const sc_ModelMesh* modelMesh);

saci_u64      sc_ModelMesh_GetIndicesAmount(const sc_ModelMesh* modelMesh);

sc_Renderer*  sc_Renderer_CreateEmpty();

sc_Renderer*  sc_Renderer_CreateDefault();

void          sc_Renderer_InitMemoryContext(sc_Renderer* renderer, saci_u64 size);

void          sc_Renderer_ResizeRenderBuffer(sc_Renderer* renderer, saci_u64 size);

saci_ShaderID sc_Renderer_GetShaderProgram(sc_Renderer* renderer);

void          sc_Renderer_SetShaderProgram(sc_Renderer* renderer, saci_ShaderID shaderID);

void          sc_Renderer_Delete(sc_Renderer* renderer);

void          sc_Renderer_SetNoFillMode(void);

void          sc_Renderer_SetFillMode(void);

void          sc_Renderer_EnableZBuffer(void);

void          sc_Renderer_SetProjectionMode(sc_RendererProjectionMode renderProjectionMode);

void          sc_Renderer_SetCustomProjectionModeFunction(sc_Renderer_CustomProjectionFunction
                                                              renderCustomProjectionModeFunction);

void          sc_Renderer_Begin(sc_Renderer* renderer);

void          sc_Renderer_End(sc_Renderer* renderer, const sc_Camera* camera);

void          sc_Renderer_PushVertices(sc_Renderer* renderer,
                                       sc_Vertice* vertices, saci_u64 verticeAmount,
                                       saci_u64 indiceAmount, saci_Mat4 modelMatrix,
                                       saci_TextureID texID, saci_u32 ibo);

void          sc_Renderer_PushModelMesh(sc_Renderer* renderer, sc_ModelMesh* mesh,
                                        saci_Mat4 modelMatrix, saci_TextureID texID);

// TODO remove
typedef void (*sc_OBJ_ModelFileReadingFunction)(void* ctx, const char* filename, int isMtl,
                                                const char* objFilename, char** buf, size_t* len);

sc_ModelMesh* sc_ModelMesh_Load(const char* path, sc_OBJ_ModelFileReadingFunction fileReader);

/* === Model Parsing === */

struct sc_VertexIndice {
    saci_u32 vertexIndex;
    saci_u32 texCoordIndex;
    saci_u32 normalIndex;
};

saci_Bool sc_OBJ_Parse(const char* filePath, sc_OBJ_ModelFileReadingFunction fileReader,
                       saci_Vec3** positions, saci_u64* positionsCount, saci_Vec2** texcoords,
                       saci_u64* texcoordCount, struct sc_VertexIndice** indices,
                       saci_u64* indicesCount);

void      sc_ModelMesh_Delete(sc_ModelMesh* modelMesh);

/* === OpenGL Helpers === */

saci_u32 sc_GL_CreateIndexBuffer(saci_u32* indices, saci_u64 indiceAmount);

saci_u32 sc_GL_CreateIndexBuffer(saci_u32* indices, saci_u64 indiceAmount);

saci_u32 sc_GL_CreateVertexBuffer(saci_u64 size, const void* data, saci_u32 usage);

void     sc_GL_CreateVertexArray(saci_u64 size, saci_u32* arrays);

void     sc_GL_ResizeVertexBuffer(saci_u32 vaoID, saci_u32 vboID, saci_u64 newSize);

void     sc_GL_BindVertexArray(saci_u32 array);

void     sc_GL_BindVertexBuffer(saci_u32 vbo);

void     sc_GL_SetVertexAttribPointer(saci_u32 index, int size, saci_u32 type, saci_Bool normalized, saci_u64 stride, void* ptr);

void     sc_GL_EnableVertexAttribArray(saci_u32 id);

/* === Shader Functions === */

saci_u32 sc_Shader_CompileShaderV(const char* source);

saci_u32 sc_Shader_CompileShaderF(const char* source);

saci_u32 sc_Shader_CompileShaderG(const char* source);

saci_u32 sc_Shader_GetShaderProgram(saci_ShaderID vshader, saci_ShaderID fshader);

saci_u32 sc_Shader_GetShaderProgramg(saci_ShaderID vshader, saci_ShaderID fshader,
                                     saci_ShaderID gshader);

#endif
