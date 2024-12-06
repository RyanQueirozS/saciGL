#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

#include <GLFW/glfw3.h>

/* === Windowing === */

typedef GLFWmonitor sc_Monitor_t;

typedef GLFWwindow  sc_Window_t;

typedef void (*sc_Window_PosHandler_t)(sc_Window_t* window, int pos_x, int pos_y);

typedef void (*sc_Window_SizeHandler_t)(sc_Window_t* window, int width, int height);

sa_Bool_t    sc_GLFW_Init(void);

sa_Bool_t    sc_GLAD_Init(void);

sc_Window_t* sc_Window_Create(int width, int height, const char* title,
                              sc_Monitor_t* monitor, sc_Window_t* share);

void         sc_Window_Free(sc_Window_t* window);

void         sc_Window_Make_Context(sc_Window_t* window);

sa_Bool_t    sc_Window_Should_Close(sc_Window_t* window);

void         sc_Window_Set_Pos_Handler(sc_Window_t* window, sc_Window_PosHandler_t window_pos_handler);

void         sc_Window_Set_Size_Handler(sc_Window_t* window, sc_Window_SizeHandler_t window_size_handler);

void         sc_Window_Terminate(void);

void         sc_Window_Clear_Color(const sa_Color_t color);

void         sc_Window_Swap_Buffer(sc_Window_t* window);

/* === Renderer === */

typedef struct sc_Renderer sc_Renderer_t;

typedef sa_Mat4_t (*sc_Renderer_Custom_Projection_Func)(struct sc_Camera_c camera);

struct sc_ModelMesh_c;

struct sc_Vertice_c;

enum sc_Renderer_Projection_Mode_e {
    sa_RENDER_ORTHOGRAPHIC_PROJECTION = 0,
    sa_RENDER_PERSPECTIVE_PROJECTION,
    sa_RENDER_CUSTOM_PROJECTION,
};

sa_Vec3_t            sc_Vertice_GetPos(const struct sc_Vertice_c* vertice);

sa_Color_t           sc_Vertice_GetColor(const struct sc_Vertice_c* vertice);

sa_Vec2_t            sc_Vertice_GetTexcoord(const struct sc_Vertice_c* vertice);

void                 sc_Vertice_GetArrayInfo(struct sc_Vertice_c* vertex_array, sa_U64_t vertex_array_size_out,
                                             sa_Vec3_t**  positions_out,
                                             sa_Color_t** colors_out,
                                             sa_Vec2_t**  texcoords_out);
struct sc_Vertice_c* sc_Vertice_CreateVertice(sa_Vec3_t position, sa_Color_t color, sa_Vec2_t texcood);

struct sc_Vertice_c* sc_Vertice_CreateVerticesArray(sa_Vec3_t* positions, sa_Color_t* colors,
                                                    sa_Vec2_t* texcoords, sa_U64_t amount);
struct sc_Vertice_c* sc_ModelMesh_GetVertices(const struct sc_ModelMesh_c* modelMesh);

sa_U64_t             sc_ModelMesh_GetVerticesAmount(const struct sc_ModelMesh_c* modelMesh);

sa_U32_t*            sc_ModelMesh_GetIndices(const struct sc_ModelMesh_c* modelMesh);

sa_U64_t             sc_ModelMesh_GetIndicesAmount(const struct sc_ModelMesh_c* modelMesh);

sc_Renderer_t*       sc_Renderer_CreateEmpty();

sc_Renderer_t*       sc_Renderer_CreateDefault();

void                 sc_Renderer_InitMemoryContext(sc_Renderer_t* renderer, sa_U64_t size);

void                 sc_Renderer_ResizeRenderBuffer(sc_Renderer_t* renderer, sa_U64_t size);

sa_Shader_ID         sc_Renderer_GetShaderProgram(sc_Renderer_t* renderer);

void                 sc_Renderer_SetShaderProgram(sc_Renderer_t* renderer, sa_Shader_ID shaderID);

void                 sc_Renderer_Delete(sc_Renderer_t* renderer);

void                 sc_Renderer_SetNoFillMode(void);

void                 sc_Renderer_SetFillMode(void);

void                 sc_Renderer_EnableZBuffer(void);

void                 sc_Renderer_SetProjectionMode(enum sc_Renderer_Projection_Mode_e renderProjectionMode);

void                 sc_Renderer_SetCustomProjectionModeFunction(sc_Renderer_Custom_Projection_Func
                                                                     renderCustomProjectionModeFunction);

void                 sc_Renderer_Begin(sc_Renderer_t* renderer);

void                 sc_Renderer_End(sc_Renderer_t* renderer, const struct sc_Camera_c* camera);

void                 sc_Renderer_PushVertices(sc_Renderer_t*       renderer,
                                              struct sc_Vertice_c* vertices, sa_U64_t verticeAmount,
                                              sa_U64_t indiceAmount, sa_Mat4_t modelMatrix,
                                              sa_Texture_ID texID, sa_U32_t ibo);

void                 sc_Renderer_PushModelMesh(sc_Renderer_t* renderer, struct sc_ModelMesh_c* mesh,
                                               sa_Mat4_t modelMatrix, sa_Texture_ID texID);

// TODO remove
typedef void (*sc_OBJ_ModelFileReadingFunction)(void* ctx, const char* filename, int isMtl,
                                                const char* objFilename, char** buf, size_t* len);

struct sc_ModelMesh_c* sc_ModelMesh_Load(const char* path, sc_OBJ_ModelFileReadingFunction fileReader);

/* === Model Parsing === */

struct sc_VertexIndice {
    sa_U32_t vertexIndex;
    sa_U32_t texCoordIndex;
    sa_U32_t normalIndex;
};

sa_Bool_t sc_OBJ_Parse(const char* filePath, sc_OBJ_ModelFileReadingFunction fileReader,
                       sa_Vec3_t** positions, sa_U64_t* positionsCount, sa_Vec2_t** texcoords,
                       sa_U64_t* texcoordCount, struct sc_VertexIndice** indices,
                       sa_U64_t* indicesCount);

void      sc_ModelMesh_Delete(struct sc_ModelMesh_c* modelMesh);

/* === OpenGL Helpers === */

sa_U32_t sc_GL_CreateIndexBuffer(sa_U32_t* indices, sa_U64_t indiceAmount);

sa_U32_t sc_GL_CreateIndexBuffer(sa_U32_t* indices, sa_U64_t indiceAmount);

sa_U32_t sc_GL_CreateVertexBuffer(sa_U64_t size, const void* data, sa_U32_t usage);

void     sc_GL_CreateVertexArray(sa_U64_t size, sa_U32_t* arrays);

void     sc_GL_ResizeVertexBuffer(sa_U32_t vaoID, sa_U32_t vboID, sa_U64_t newSize);

void     sc_GL_BindVertexArray(sa_U32_t array);

void     sc_GL_BindVertexBuffer(sa_U32_t vbo);

void     sc_GL_SetVertexAttribPointer(sa_U32_t index, int size, sa_U32_t type, sa_Bool_t normalized, sa_U64_t stride, void* ptr);

void     sc_GL_EnableVertexAttribArray(sa_U32_t id);

/* === Shader Functions === */

sa_U32_t sc_Shader_CompileShaderV(const char* source);

sa_U32_t sc_Shader_CompileShaderF(const char* source);

sa_U32_t sc_Shader_CompileShaderG(const char* source);

sa_U32_t sc_Shader_GetShaderProgram(sa_Shader_ID vshader, sa_Shader_ID fshader);

sa_U32_t sc_Shader_GetShaderProgramg(sa_Shader_ID vshader, sa_Shader_ID fshader,
                                     sa_Shader_ID gshader);

#endif
