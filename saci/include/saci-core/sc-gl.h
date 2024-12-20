#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

#include <GLFW/glfw3.h>

/* === Windowing === */

#ifndef SACI_GLFW_MONITOR_T
typedef GLFWmonitor sc_Monitor_t;
#endif

#ifndef SACI_GLFW_WINDOW_T
typedef GLFWwindow sc_Window_t;
#endif

typedef void (*sc_Window_PosHandler_t)(sc_Window_t* window, int pos_x, int pos_y);

typedef void (*sc_Window_SizeHandler_t)(sc_Window_t* window, int width, int height);

sa_bool_t sc_GLFW_Init();

sa_bool_t sc_GLAD_Init(void);

sc_Window_t* sc_Window_Create(int width, int height, const char* title,
                              sc_Monitor_t* monitor, sc_Window_t* share);

void sc_Window_Free(sc_Window_t* window);

void sc_Window_Make_Context(sc_Window_t* window);

sa_bool_t sc_Window_Should_Close(sc_Window_t* window);

void sc_Window_Set_Pos_Handler(sc_Window_t* window, sc_Window_PosHandler_t window_pos_handler);

void sc_Window_Set_Size_Handler(sc_Window_t* window, sc_Window_SizeHandler_t window_size_handler);

void sc_Window_Terminate(void);

void sc_Window_Clear_Color(const sa_Color_t color);

void sc_Window_Swap_Buffer(sc_Window_t* window);

/* === Renderer === */

#ifndef SACI_RENDERER_T
typedef struct sc_Renderer_c sc_Renderer_t;
#endif

typedef sa_Mat4_t (*sc_Renderer_ProjectionFunc)(struct sc_Camera_c camera);

struct sc_ModelMesh_c;

struct sc_Vertice_c;

enum sc_Renderer_Projection_Mode_e {
    sa_RENDERER_PROJECTION_MODE_ORTHO = 0,
    sa_RENDERER_PROJECTION_MODE_PERSPECTIVE,
    sa_RENDERER_PROJECTION_MODE_CUSTOM,
};

sa_Vec3_t sc_Vertice_Get_Pos(const struct sc_Vertice_c* vertice);

sa_Color_t sc_Vertice_Get_Color(const struct sc_Vertice_c* vertice);

sa_Vec2_t sc_Vertice_Get_Texcoord(const struct sc_Vertice_c* vertice);

void sc_Vertice_Get_Array_Info(struct sc_Vertice_c* vertex_array, sa_u64_t vertex_array_size_out,
                               sa_Vec3_t** positions_out,
                               sa_Color_t** colors_out,
                               sa_Vec2_t** texcoords_out);
struct sc_Vertice_c* sc_Vertice_Create_Vertice(sa_Vec3_t position, sa_Color_t color, sa_Vec2_t texcood);

struct sc_Vertice_c* sc_Vertice_Create_Vertices_Array(sa_Vec3_t* positions, sa_Color_t* colors,
                                                      sa_Vec2_t* texcoords, sa_u64_t amount);
// TODO remove and use a sc_ModelMesh_Get_Array_Info
struct sc_Vertice_c* sc_ModelMesh_Get_Vertices(const struct sc_ModelMesh_c* modelMesh);

// TODO redo file organization
struct sc_VertexIndice_c {
    sa_u32_t vertexIndex;
    sa_u32_t texCoordIndex;
    sa_u32_t normalIndex;
};

struct sc_ModelMesh_c* sc_ModelMesh_Create(sa_Vec3_t* verticesPos,
                                           sa_u64_t verticePosAmount,
                                           sa_Vec2_t* verticesTexcoord,
                                           sa_u64_t verticesTexcoordAmount,
                                           struct sc_VertexIndice_c* indices,
                                           sa_u64_t indiceAmount);

sa_u64_t sc_ModelMesh_Get_Vertices_Amount(const struct sc_ModelMesh_c* modelMesh);

sa_u32_t* sc_ModelMesh_Get_Indices(const struct sc_ModelMesh_c* modelMesh);

sa_u64_t sc_ModelMesh_GetIndicesAmount(const struct sc_ModelMesh_c* modelMesh);

sc_Renderer_t* sc_Renderer_Create_Empty(void);

sc_Renderer_t* sc_Renderer_Create_Default(void);

void sc_Renderer_Init_Memory_Context(sc_Renderer_t* renderer, sa_u64_t size);

void sc_Renderer_Resize_Render_Buffer(sc_Renderer_t* renderer, sa_u64_t size);

sa_shaderId sc_Renderer_Get_Shader_Program(sc_Renderer_t* renderer);

void sc_Renderer_Set_Shader_Program(sc_Renderer_t* renderer, sa_shaderId shaderID);

void sc_Renderer_Delete(sc_Renderer_t* renderer);

void sc_Renderer_Set_No_Fill_Mode(void);

void sc_Renderer_Set_Fill_Mode(void);

void sc_Renderer_Enable_Z_Buffer(void);

void sc_Renderer_Set_Projection_Mode(enum sc_Renderer_Projection_Mode_e renderProjectionMode);

void sc_Renderer_SetCustom_Projection_Function(sc_Renderer_ProjectionFunc
                                                   renderCustomProjectionModeFunction);

void sc_Renderer_Begin(sc_Renderer_t* renderer);

void sc_Renderer_End(sc_Renderer_t* renderer, const struct sc_Camera_c* camera);

void sc_Renderer_Push_Vertices(sc_Renderer_t* renderer,
                               struct sc_Vertice_c* vertices, sa_u64_t verticeAmount,
                               sa_u64_t indiceAmount, sa_Mat4_t modelMatrix,
                               sa_textureId texID, sa_u32_t ibo);

void sc_Renderer_Push_Model_Mesh(sc_Renderer_t* renderer, struct sc_ModelMesh_c* mesh,
                                 sa_Mat4_t modelMatrix, sa_textureId texID);

// TODO remove
typedef void (*sc_OBJ_File_Reading_Function)(void* ctx, const char* filename, int isMtl,
                                             const char* objFilename, char** buf, size_t* len);

struct sc_ModelMesh_c* sc_Model_Mesh_Load(const char* path, sc_OBJ_File_Reading_Function fileReader);

/* === Model Parsing === */

sa_bool_t sc_OBJ_Parse(const char* file_path, sc_OBJ_File_Reading_Function file_reader,
                       sa_Vec3_t** positions_out, sa_u64_t* positions_count_out,
                       sa_Vec2_t** texcoords_out, sa_u64_t* texcoord_count_out,
                       struct sc_VertexIndice_c** indices_out, sa_u64_t* indices_count_out);

void sc_Model_Mesh_Delete(struct sc_ModelMesh_c* model_mesh);

/* === OpenGL Helpers === */

sa_u32_t sc_GL_Create_Index_Buffer(sa_u32_t* indices, sa_u64_t indice_amount);

sa_u32_t sc_GL_Create_Index_Buffer(sa_u32_t* indices, sa_u64_t indiceAmount);

sa_u32_t sc_GL_Create_Vertex_Buffer(sa_u64_t size, const void* data, sa_u32_t usage);

void sc_GL_Create_Vertex_Array(sa_u64_t size, sa_u32_t* arrays);

void sc_GL_Resize_Vertex_Buffer(sa_u32_t vao_id, sa_u32_t vbo_id, sa_u64_t new_size);

void sc_GL_Bind_Vertex_Array(sa_u32_t array);

void sc_GL_Bind_Vertex_Buffer(sa_u32_t vbo);

void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32_t index, int size, sa_u32_t type, sa_bool_t normalized, sa_u64_t stride, void* ptr);

void sc_GL_Enable_Vertex_Attrib_Array(sa_u32_t id);

/* === Shader Functions === */

sa_u32_t sc_Shader_Compile_Shader_Vert(const char* source);

sa_u32_t sc_Shader_Compile_Shader_Frag(const char* source);

sa_u32_t sc_Shader_Compile_Shader_Geom(const char* source);

sa_u32_t sc_Shader_Create_Shader_Program(sa_shaderId vshader, sa_shaderId fshader);

sa_u32_t sc_Shader_Create_Shader_Program_Geom(sa_shaderId vshader, sa_shaderId fshader,
                                              sa_shaderId gshader);

#endif
