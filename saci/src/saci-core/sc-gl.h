#ifndef SACI_CORE_SC_RENDERING_H
#define SACI_CORE_SC_RENDERING_H

#include <GLFW/glfw3.h>

#include "saci-core/sc-model.h"
#include "saci-utils/su-types.h"
#include "saci-core/sc-windowing.h"
#include "saci-utils/su-general.h"

/* === Event === */

#ifndef SC_EVENT_MOUSE_POS_HANDLER_t
#  define SC_EVENT_MOUSE_POS_HANDLER_t
typedef void (*sc_event_mousePosHandler_t)(sc_window_t* window, double posx, double posy);
#endif // SC_EVENT_MOUSE_POS_HANDLER_t

enum sc_key {
    sc_KEY_SPACE = 32,
    sc_KEY_APOSTROPHE = 39, /* ' */
    sc_KEY_COMMA = 44,      /* , */
    sc_KEY_MINUS = 45,      /* - */
    sc_KEY_PERIOD = 46,     /* . */
    sc_KEY_SLASH = 47,      /* / */
    sc_KEY_0 = 48,
    sc_KEY_1 = 49,
    sc_KEY_2 = 50,
    sc_KEY_3 = 51,
    sc_KEY_4 = 52,
    sc_KEY_5 = 53,
    sc_KEY_6 = 54,
    sc_KEY_7 = 55,
    sc_KEY_8 = 56,
    sc_KEY_9 = 57,
    sc_KEY_SEMICOLON = 59, /* ; */
    sc_KEY_EQUAL = 61,     /* = */
    sc_KEY_A = 65,
    sc_KEY_B = 66,
    sc_KEY_C = 67,
    sc_KEY_D = 68,
    sc_KEY_E = 69,
    sc_KEY_F = 70,
    sc_KEY_G = 71,
    sc_KEY_H = 72,
    sc_KEY_I = 73,
    sc_KEY_J = 74,
    sc_KEY_K = 75,
    sc_KEY_L = 76,
    sc_KEY_M = 77,
    sc_KEY_N = 78,
    sc_KEY_O = 79,
    sc_KEY_P = 80,
    sc_KEY_Q = 81,
    sc_KEY_R = 82,
    sc_KEY_S = 83,
    sc_KEY_T = 84,
    sc_KEY_U = 85,
    sc_KEY_V = 86,
    sc_KEY_W = 87,
    sc_KEY_X = 88,
    sc_KEY_Y = 89,
    sc_KEY_Z = 90,
    sc_KEY_LEFT_BRACKET = 91,  /* [ */
    sc_KEY_BACKSLASH = 92,     /* \ */
    sc_KEY_RIGHT_BRACKET = 93, /* ] */
    sc_KEY_GRAVE_ACCENT = 96,  /* ` */
    sc_KEY_WORLD_1 = 161,      /* non-US #1 */
    sc_KEY_WORLD_2 = 162,      /* non-US #2 */
    sc_KEY_ESCAPE = 256,
    sc_KEY_ENTER = 257,
    sc_KEY_TAB = 258,
    sc_KEY_BACKSPACE = 259,
    sc_KEY_INSERT = 260,
    sc_KEY_DELETE = 261,
    sc_KEY_RIGHT = 262,
    sc_KEY_LEFT = 263,
    sc_KEY_DOWN = 264,
    sc_KEY_UP = 265,
    sc_KEY_PAGE_UP = 266,
    sc_KEY_PAGE_DOWN = 267,
    sc_KEY_HOME = 268,
    sc_KEY_END = 269,
    sc_KEY_CAPS_LOCK = 280,
    sc_KEY_SCROLL_LOCK = 281,
    sc_KEY_NUM_LOCK = 282,
    sc_KEY_PRINT_SCREEN = 283,
    sc_KEY_PAUSE = 284,
    sc_KEY_F1 = 290,
    sc_KEY_F2 = 291,
    sc_KEY_F3 = 292,
    sc_KEY_F4 = 293,
    sc_KEY_F5 = 294,
    sc_KEY_F6 = 295,
    sc_KEY_F7 = 296,
    sc_KEY_F8 = 297,
    sc_KEY_F9 = 298,
    sc_KEY_F10 = 299,
    sc_KEY_F11 = 300,
    sc_KEY_F12 = 301,
    sc_KEY_F13 = 302,
    sc_KEY_F14 = 303,
    sc_KEY_F15 = 304,
    sc_KEY_F16 = 305,
    sc_KEY_F17 = 306,
    sc_KEY_F18 = 307,
    sc_KEY_F19 = 308,
    sc_KEY_F20 = 309,
    sc_KEY_F21 = 310,
    sc_KEY_F22 = 311,
    sc_KEY_F23 = 312,
    sc_KEY_F24 = 313,
    sc_KEY_F25 = 314,
    sc_KEY_KP_0 = 320,
    sc_KEY_KP_1 = 321,
    sc_KEY_KP_2 = 322,
    sc_KEY_KP_3 = 323,
    sc_KEY_KP_4 = 324,
    sc_KEY_KP_5 = 325,
    sc_KEY_KP_6 = 326,
    sc_KEY_KP_7 = 327,
    sc_KEY_KP_8 = 328,
    sc_KEY_KP_9 = 329,
    sc_KEY_KP_DECIMAL = 330,
    sc_KEY_KP_DIVIDE = 331,
    sc_KEY_KP_MULTIPLY = 332,
    sc_KEY_KP_SUBTRACT = 333,
    sc_KEY_KP_ADD = 334,
    sc_KEY_KP_ENTER = 335,
    sc_KEY_KP_EQUAL = 336,
    sc_KEY_LEFT_SHIFT = 340,
    sc_KEY_LEFT_CONTROL = 341,
    sc_KEY_LEFT_ALT = 342,
    sc_KEY_LEFT_SUPER = 343,
    sc_KEY_RIGHT_SHIFT = 344,
    sc_KEY_RIGHT_CONTROL = 345,
    sc_KEY_RIGHT_ALT = 346,
    sc_KEY_RIGHT_SUPER = 347,
    sc_KEY_MENU = 348,
    sc_KEY_LAST = sc_KEY_MENU,
};

SA_API void sc_Event_Poll(void);
SA_API void sc_Event_Wait(void);
SA_API void sc_Event_Wait_For_Timeout(double timeout);
SA_API void sc_Event_Post_Empty(void);
SA_API void sc_Event_Set_Mouse_Pos_Handler(sc_window_t* window, sc_event_mousePosHandler_t mouse_pos_handler);
SA_API sa_bool sc_Event_Is_Key_Pressed(sc_window_t* window, int keycode);

/* === Renderer === */

typedef struct sc_renderer sc_renderer;

#ifdef SC_RENDERER_STRUCT_EXPOSE
#  ifndef SC_RENDERER_STRUCT
#    define SC_RENDERER_STRUCT

struct __sc_vertex {
    sa_vec3 pos;
    sa_color color;
    sa_uv uv;
};

// The fields are structured in a way that enforces minimum memory change over time
struct __sc_renderer {
    sa_textureId bound_texture_id;
    sa_u32 bound_index_array_length;
    sa_u32 bound_index_array_capacity;
    sa_u64 bound_uniform_struct_size;

    sa_shaderId shader_program;
    sa_bufferId ibo, ubo, vbo, vao;

    sa_u32 batch_index_capacity;
    sa_u32 batch_vertex_capacity;
    sa_u8 batch_array_capacity;
    sa_u8 batch_length; // 0 indexed

    sa_u32 call_index_capacity;
    sa_u32 call_vertex_capacity;
    sa_u8 call_array_capacity;
    sa_u8 call_in_use; // 0 indexed

    struct __sc_batch {
        sa_u64 uniform_struct_block_size;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u8 uniform_block_size;
        sa_u32* index_array;
        struct __sc_vertex* vertex_array;
        sa_u8* uniform_struct_block;
    }* batch_array;

    sa_u8* uniform_struct_block;

    struct __sc_renderCall {
        sa_u64 uniform_struct_block_size;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u8 uniform_block_size;
        sa_u32* index_array;
        struct __sc_vertex* vertex_array;
        sa_u8* uniform_struct_block;
    }* call_array;

    sa_u32* bound_index_array_buffer;
};

#  endif // SC_RENDERER_STRUCT
#endif   // SC_RENDERER_STRUCT_EXPOSE

#define sc_RENDERER_FREE_OPT_MEMORY 0b01
#define sc_RENDERER_FREE_OPT_OPENGL 0b10

#define sc_RENDERER_UNIFORM_FLAG_IS_2D 0b0
#define sc_RENDERER_UNIFORM_FLAG_IS_3D 0b1

#if 0
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_FLUSH 01
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_CRASH 02
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_SKIP 03
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_RESIZE 04

#  define sc_RENDERER_CALL_OVERFLOW_ACTION_FLUSH 01
#  define sc_RENDERER_CALL_OVERFLOW_ACTION_CRASH 02
#  define sc_RENDERER_CALL_OVERFLOW_ACTION_SKIP 03
#  define sc_RENDERER_CALL_OVERFLOW_ACTION_RESIZE 04
#endif // TODO to be implemented

SA_API sc_renderer* sc_Renderer_New_Default(void);

SA_API sc_renderer* sc_Renderer_New_Default_Ctx(void* mem_ctx,
                                                sa_u64 batch_index_capacity,
                                                sa_u64 batch_vertex_capacity,
                                                sa_u64 bound_capacity);

SA_API void sc_Renderer_Begin(struct sc_renderer* rendr);

SA_API void sc_Renderer_Bind_Texture(struct sc_renderer* rendr,
                                     const sa_textureId tex_id);

SA_API void sc_Renderer_Set_Uniform_Struct(struct sc_renderer* rendr,
                                           sa_u64 size);

SA_API void sc_Renderer_Set_Bound_Index_Buffer_Capacity(struct sc_renderer* rendr,
                                                        sa_u32 new_size);

SA_API void sc_Renderer_Bind_Uniform_Struct(struct sc_renderer* rendr,
                                            void* uniform);

SA_API void sc_Renderer_Bind_Uniform_Value(struct sc_renderer* rendr,
                                           void* value,
                                           sa_u64 start_offset,
                                           sa_u64 size);

SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr,
                                          const sa_u32* new_indices,
                                          const sa_u32 new_indices_count);

SA_API void sc_Renderer_Push_Vertex(struct sc_renderer* rendr,
                                    const sa_vec3* pos_array,
                                    const sa_uv* uv_array,
                                    const sa_color* color_array,
                                    const sa_u32 amount);

SA_API void sc_Renderer_Push_Model(struct sc_renderer* rendr, const sc_modelMesh* model_mesh);

SA_API void sc_Renderer_End(struct sc_renderer* rendr);

SA_API void sc_Renderer_Free(struct sc_renderer* rendr);

SA_API void sc_Renderer_Free_Opts(struct sc_renderer* rendr, int free_opts);

/* === OpenGL === */

SA_API sa_u32 sc_GL_Create_Index_Buffer_Dynamic(sa_u32* indices, sa_u64 indice_amount);
SA_API sa_u32 sc_GL_Create_Index_Buffer_Static(sa_u32* indices, sa_u64 indice_amount);
SA_API sa_u32 sc_GL_Create_Vertex_Buffer(sa_u64 size, const void* data, sa_u32 usage);
SA_API void sc_GL_Create_Vertex_Array(sa_u64 size, sa_u32* arrays);
SA_API void sc_GL_Resize_Vertex_Buffer(sa_u32 vao_id, sa_u32 vbo_id, sa_u64 new_size);
SA_API void sc_GL_Bind_Vertex_Array(sa_u32 array);
SA_API void sc_GL_Bind_Vertex_Buffer(sa_u32 vbo);
SA_API void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32 index, int size, sa_u32 type, sa_bool normalized, sa_u64 stride, void* ptr);
SA_API void sc_GL_Enable_Vertex_Attrib_Array(sa_u32 id);
// TODO bind program

/* === Shader === */

SA_API sa_u32 sc_Shader_Compile_Shader_Vert(const char* source);
SA_API sa_u32 sc_Shader_Compile_Shader_Frag(const char* source);
SA_API sa_u32 sc_Shader_Compile_Shader_Geom(const char* source);
SA_API sa_u32 sc_Shader_Create_Shader_Program(sa_shaderId vshader, sa_shaderId fshader);
SA_API sa_u32 sc_Shader_Create_Shader_Program_Geom(sa_shaderId vshader, sa_shaderId fshader,
                                                   sa_shaderId gshader);

#endif // SACI_CORE_SC_RENDERING_H_
