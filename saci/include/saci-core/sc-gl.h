#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include <GLFW/glfw3.h>
#include <saci-utils/su-debug.h>

#include "saci-utils/su-types.h"
#include "saci-core/sc-windowing.h"

#ifndef SA_API

#  ifdef _WIN32

#    ifdef BUILD_SACI_LIB
#      define SA_API __declspec(dllexport) // Export symbols when building the library
#    else
#      define SA_API __declspec(dllimport) // Import symbols when using the library
#    endif
#  else
#    define SA_API // Non-Windows platforms don't need special decoration

#  endif // _WIN32

#endif // SA_API

/* === Event === */

#ifndef SC_EVENT_MOUSE_POS_HANDLER_t
#  define SC_EVENT_MOUSE_POS_HANDLER_t
typedef void (*sc_event_mousePosHandler_t)(sc_window_t* window, double posx, double posy);
#endif // SC_EVENT_MOUSE_POS_HANDLER_t

#define sc_KEY_SPACE 32
#define sc_KEY_APOSTROPHE 39 /* ' */
#define sc_KEY_COMMA 44      /* , */
#define sc_KEY_MINUS 45      /* - */
#define sc_KEY_PERIOD 46     /* . */
#define sc_KEY_SLASH 47      /* / */
#define sc_KEY_0 48
#define sc_KEY_1 49
#define sc_KEY_2 50
#define sc_KEY_3 51
#define sc_KEY_4 52
#define sc_KEY_5 53
#define sc_KEY_6 54
#define sc_KEY_7 55
#define sc_KEY_8 56
#define sc_KEY_9 57
#define sc_KEY_SEMICOLON 59 /* ; */
#define sc_KEY_EQUAL 61     /* = */
#define sc_KEY_A 65
#define sc_KEY_B 66
#define sc_KEY_C 67
#define sc_KEY_D 68
#define sc_KEY_E 69
#define sc_KEY_F 70
#define sc_KEY_G 71
#define sc_KEY_H 72
#define sc_KEY_I 73
#define sc_KEY_J 74
#define sc_KEY_K 75
#define sc_KEY_L 76
#define sc_KEY_M 77
#define sc_KEY_N 78
#define sc_KEY_O 79
#define sc_KEY_P 80
#define sc_KEY_Q 81
#define sc_KEY_R 82
#define sc_KEY_S 83
#define sc_KEY_T 84
#define sc_KEY_U 85
#define sc_KEY_V 86
#define sc_KEY_W 87
#define sc_KEY_X 88
#define sc_KEY_Y 89
#define sc_KEY_Z 90
#define sc_KEY_LEFT_BRACKET 91  /* [ */
#define sc_KEY_BACKSLASH 92     /* \ */
#define sc_KEY_RIGHT_BRACKET 93 /* ] */
#define sc_KEY_GRAVE_ACCENT 96  /* ` */
#define sc_KEY_WORLD_1 161      /* non-US #1 */
#define sc_KEY_WORLD_2 162      /* non-US #2 */
#define sc_KEY_ESCAPE 256
#define sc_KEY_ENTER 257
#define sc_KEY_TAB 258
#define sc_KEY_BACKSPACE 259
#define sc_KEY_INSERT 260
#define sc_KEY_DELETE 261
#define sc_KEY_RIGHT 262
#define sc_KEY_LEFT 263
#define sc_KEY_DOWN 264
#define sc_KEY_UP 265
#define sc_KEY_PAGE_UP 266
#define sc_KEY_PAGE_DOWN 267
#define sc_KEY_HOME 268
#define sc_KEY_END 269
#define sc_KEY_CAPS_LOCK 280
#define sc_KEY_SCROLL_LOCK 281
#define sc_KEY_NUM_LOCK 282
#define sc_KEY_PRINT_SCREEN 283
#define sc_KEY_PAUSE 284
#define sc_KEY_F1 290
#define sc_KEY_F2 291
#define sc_KEY_F3 292
#define sc_KEY_F4 293
#define sc_KEY_F5 294
#define sc_KEY_F6 295
#define sc_KEY_F7 296
#define sc_KEY_F8 297
#define sc_KEY_F9 298
#define sc_KEY_F10 299
#define sc_KEY_F11 300
#define sc_KEY_F12 301
#define sc_KEY_F13 302
#define sc_KEY_F14 303
#define sc_KEY_F15 304
#define sc_KEY_F16 305
#define sc_KEY_F17 306
#define sc_KEY_F18 307
#define sc_KEY_F19 308
#define sc_KEY_F20 309
#define sc_KEY_F21 310
#define sc_KEY_F22 311
#define sc_KEY_F23 312
#define sc_KEY_F24 313
#define sc_KEY_F25 314
#define sc_KEY_KP_0 320
#define sc_KEY_KP_1 321
#define sc_KEY_KP_2 322
#define sc_KEY_KP_3 323
#define sc_KEY_KP_4 324
#define sc_KEY_KP_5 325
#define sc_KEY_KP_6 326
#define sc_KEY_KP_7 327
#define sc_KEY_KP_8 328
#define sc_KEY_KP_9 329
#define sc_KEY_KP_DECIMAL 330
#define sc_KEY_KP_DIVIDE 331
#define sc_KEY_KP_MULTIPLY 332
#define sc_KEY_KP_SUBTRACT 333
#define sc_KEY_KP_ADD 334
#define sc_KEY_KP_ENTER 335
#define sc_KEY_KP_EQUAL 336
#define sc_KEY_LEFT_SHIFT 340
#define sc_KEY_LEFT_CONTROL 341
#define sc_KEY_LEFT_ALT 342
#define sc_KEY_LEFT_SUPER 343
#define sc_KEY_RIGHT_SHIFT 344
#define sc_KEY_RIGHT_CONTROL 345
#define sc_KEY_RIGHT_ALT 346
#define sc_KEY_RIGHT_SUPER 347
#define sc_KEY_MENU 348
#define sc_KEY_LAST sc_KEY_MENU

SA_API void sc_Event_Poll(void);
SA_API void sc_Event_Wait(void);
SA_API void sc_Event_Wait_For_Timeout(double timeout);
SA_API void sc_Event_Post_Empty(void);
SA_API void sc_Event_Set_Mouse_Pos_Handler(sc_window_t* window, sc_event_mousePosHandler_t mouse_pos_handler);
SA_API sa_bool_t sc_Event_Is_Key_Pressed(sc_window_t* window, int keycode);

/* === Renderer === */

#ifdef SC_RENDERER_STRUCT_EXPOSE
#  ifndef SC_RENDERER_STRUCT
#    define SC_RENDERER_STRUCT

struct sc_Renderer {
    sa_textureId current_texture_id;

    sa_u32_t* bound_index_array;
    sa_u32_t bound_index_array_count;
    sa_u32_t vertices_overlaped;

    sa_shaderId shader_program;
    sa_bufferId ibo;
    sa_bufferId vbo;
    sa_bufferId vao;

    struct __sc_batch {
        sa_u32_t index_array_count;
        sa_u32_t vertex_array_count;

        sa_u32_t* index_array;
        struct __sc_vertex {
            sa_vec3_t pos;
            sa_color_t color;
            sa_uv uv;
        }* vertex_array;
    } batch;
    sa_u32_t batch_count; // NOT CURRENTLY IN USE
};

#  endif // SC_RENDERER_STRUCT
#endif   // SC_RENDERER_STRUCT_EXPOSE

SA_API void sc_Renderer_Init(void);
SA_API void sc_Renderer_Begin(void);
SA_API void sc_Renderer_Bind_Texture(sa_textureId tex_id);
SA_API void sc_Renderer_Bind_Index_Buffer(sa_u32_t* new_indices, sa_u32_t new_indices_count);
SA_API void sc_Renderer_Push_Vertex(sa_vec3_t* pos_array, sa_uv* uv_array, sa_color_t* color_array, sa_u32_t amount); // TODO every value could be sent as null, but pos should cause a warning
SA_API void sc_Renderer_End(void);

/* === OpenGL === */

SA_API sa_u32_t sc_GL_Create_Index_Buffer_Dynamic(sa_u32_t* indices, sa_u64_t indice_amount);
SA_API sa_u32_t sc_GL_Create_Index_Buffer_Static(sa_u32_t* indices, sa_u64_t indice_amount);
SA_API sa_u32_t sc_GL_Create_Vertex_Buffer(sa_u64_t size, const void* data, sa_u32_t usage);
SA_API void sc_GL_Create_Vertex_Array(sa_u64_t size, sa_u32_t* arrays);
SA_API void sc_GL_Resize_Vertex_Buffer(sa_u32_t vao_id, sa_u32_t vbo_id, sa_u64_t new_size);
SA_API void sc_GL_Bind_Vertex_Array(sa_u32_t array);
SA_API void sc_GL_Bind_Vertex_Buffer(sa_u32_t vbo);
SA_API void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32_t index, int size, sa_u32_t type, sa_bool_t normalized, sa_u64_t stride, void* ptr);
SA_API void sc_GL_Enable_Vertex_Attrib_Array(sa_u32_t id);
// TODO bind program

/* === Shader === */

SA_API sa_u32_t sc_Shader_Compile_Shader_Vert(const char* source);
SA_API sa_u32_t sc_Shader_Compile_Shader_Frag(const char* source);
SA_API sa_u32_t sc_Shader_Compile_Shader_Geom(const char* source);
SA_API sa_u32_t sc_Shader_Create_Shader_Program(sa_shaderId vshader, sa_shaderId fshader);
SA_API sa_u32_t sc_Shader_Create_Shader_Program_Geom(sa_shaderId vshader, sa_shaderId fshader,
                                                     sa_shaderId gshader);

#endif
