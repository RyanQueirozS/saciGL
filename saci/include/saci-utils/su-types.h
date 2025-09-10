/**
 * @file su-types.h
 * @brief This header defines common types and structures used in the saciCORE and
 * saciLIB.
 *
 * @details
 * This file provides typedefs for both signed and unsigned integer types, boolean
 * constants, as well as common vector, color, and matrix structures. It aims to simplify
 * the use of types across the saciCORE and saciLIB and ensure consistency.
 */

#ifndef SACI_UTILS_SU_TYPES_H
#define SACI_UTILS_SU_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "saci-utils/su-general.h"

/* === Typedefs === */

typedef uint64_t su_U64;

typedef uint32_t su_U32;

typedef uint16_t su_U16;

typedef uint8_t su_U8;

typedef int64_t su_S64;

typedef int32_t su_S32;

typedef int16_t su_S16;

typedef int8_t su_S8;

/* === Boolean Constants === */

#define su_TRUE true

#define su_FALSE false

typedef bool su_Bool;

/* === Identifiers === */

typedef su_U32 su_ShaderId;

typedef su_U32 su_TextureId;

typedef su_U32 su_BufferId;

/* === Vector Structures === */

typedef struct {
    float u; /**< U compoonent */
    float v; /**< V compoonent */
} su_Uv;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} su_Vec2;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} su_Vec3;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
    float w; /**< W compoonent */
} su_Vec4;

/* === Color Structure === */

typedef struct {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} su_Color;

/* === Matrix Structure === */

typedef struct {
    float data[4][4]; /**< 4x4 matrix of floats */
} su_Mat4;

/* === Types === */

typedef enum {
    su_TYPE_U8 = 1,  // unsigned int8
    su_TYPE_U16 = 2, // unsigned int16
    su_TYPE_U32 = 3, // unsigned int32
    su_TYPE_U64 = 4, // unsigned int64
    su_TYPE_S8 = 5,  // signed int8
    su_TYPE_S16 = 6, // signed int16
    su_TYPE_S32 = 7, // signed int32
    su_TYPE_S64 = 8, // signed int64

    su_TYPE_BOOL = 9, // boolean

    su_TYPE_SHADERID = 10,  // unsgined int32
    su_TYPE_TEXTUREID = 11, // unsgined int32
    su_TYPE_BUFFERID = 12,  // unsigned int32

    su_TYPE_UV = 13,   // 2 floats (u, v)
    su_TYPE_VEC2 = 14, // 2 floats (x, y)
    su_TYPE_VEC3 = 15, // 3 floats (x, y, z)
    su_TYPE_VEC4 = 16, // 4 floats (x, y, z, w)

    su_TYPE_COLOR = 17, // 4 floats (r, g, b, a)

    su_TYPE_MAT2 = 18, // 2x2 matrix
    su_TYPE_MAT3 = 19, // 3x3 matrix
    su_TYPE_MAT4 = 20, // 4x4 matrix

    su_TYPE_MAT2X3 = 21, // 2 columns, 3 rows
    su_TYPE_MAT2X4 = 22, // 2 columns, 4 rows
    su_TYPE_MAT3X2 = 23, // 3 columns, 2 rows
    su_TYPE_MAT3X4 = 24, // 3 columns, 4 rows
    su_TYPE_MAT4X2 = 25, // 4 columns, 2 rows
    su_TYPE_MAT4X3 = 26, // 4 columns, 3 rows

    su_TYPE_SAMPLER2D = 27,
} su_DataType;

#define su_TYPE_MAX 26
static const su_U64 su_SIZE_OF_TYPE[] = {
    0, // index 0 (unused)

    1, // su_TYPE_U8        = 1
    2, // su_TYPE_U16       = 2
    4, // su_TYPE_U32       = 3
    8, // su_TYPE_U64       = 4
    1, // su_TYPE_S8        = 5
    2, // su_TYPE_S16       = 6
    4, // su_TYPE_S32       = 7
    8, // su_TYPE_S64       = 8

    1, // su_TYPE_BOOL      = 9

    4, // su_TYPE_SHADERID  = 10
    4, // su_TYPE_TEXTUREID = 11
    4, // su_TYPE_BUFFERID  = 12

    8,  // su_TYPE_UV        = 13 (2 floats)
    8,  // su_TYPE_VEC2      = 14 (2 floats)
    12, // su_TYPE_VEC3      = 15 (3 floats)
    16, // su_TYPE_VEC4      = 16 (4 floats)

    16, // su_TYPE_COLOR     = 17 (4 floats, e.g. RGBA)

    16, // su_TYPE_MAT2      = 18 (2x2 floats = 4 floats)
    36, // su_TYPE_MAT3      = 19 (3x3 = 9 floats)
    64, // su_TYPE_MAT4      = 20 (4x4 = 16 floats)

    24, // su_TYPE_MAT2X3    = 21 (2 columns * 3 rows = 6 floats)
    32, // su_TYPE_MAT2X4    = 22 (2 * 4 = 8 floats)
    24, // su_TYPE_MAT3X2    = 23 (3 * 2 = 6 floats)
    48, // su_TYPE_MAT3X4    = 24 (3 * 4 = 12 floats)
    32, // su_TYPE_MAT4X2    = 25 (4 * 2 = 8 floats)
    48  // su_TYPE_MAT4X3    = 26 (4 * 3 = 12 floats)
};

typedef enum su_ReallocationKind {
    su_REALLOCATION_KIND_REALLOC = 0,
    su_REALLOCATION_KIND_MALLOC_FREE = 1,
    su_REALLOCATION_KIND_FIXED_SIZE = 2,
} su_ReallocationKind;

/* === Rendering Specific === */

typedef void* (*su_GLProc)(const char* name);
typedef void* su_Window;
typedef void* su_Monitor;
typedef void (*su_WindowPosHandler)(su_Window* window, int pos_x, int pos_y);
typedef void (*su_WindowSizeHandler)(su_Window* window, int width, int height);
typedef void (*su_EventMousePosHandler)(su_Window window, double posx, double posy);

/* === DArray === */

typedef struct su_DArray su_DArray;

#define su_SIZE_OF_DARRAY 40

SA_API su_DArray* su_darray_create(su_U64 capacity, su_U64 elem_size, su_Bool fixed_size);

SA_API su_DArray* su_darray_create_ctx(void* memctx, su_U64 memctx_size, su_U64 capacity, su_U64 elem_size, su_Bool fixed_size);

SA_API void su_darray_free(su_DArray* array);

SA_API su_Bool su_darray_clear(su_DArray* array);

SA_API su_Bool su_darray_resize(su_DArray* array, su_U64 new_cap);

SA_API su_Bool su_darray_push(su_DArray* array, const void* value);

SA_API void su_darray_pop(su_DArray* array);

SA_API su_Bool su_darray_get(const su_DArray* array, su_U64 index, void* out_value);

SA_API void* su_darray_get_ptr(const su_DArray* array, su_U64 index);

SA_API su_U64 su_darray_get_elem_size(const su_DArray* array);

SA_API void su_darray_set(su_DArray* array, su_U64 index, const void* value);

SA_API su_U64 su_darray_length(const su_DArray* array);

SA_API su_U64 su_darray_capacity(const su_DArray* array);

SA_API su_Bool su_darray_append(su_DArray* dest, const su_DArray* src);

SA_API void su_darray_debug_print(const su_DArray* arr);

SA_API su_Bool su_darray_is_null(const su_DArray* arr);

SA_API su_Bool su_darray_is_empty(const su_DArray* arr);

/* === String === */

typedef struct su_String su_String;

SA_API su_String* su_string_create(const char* data, su_ReallocationKind realloc_kind);

SA_API su_String* su_string_create_ctx(const char* data, su_ReallocationKind realloc_kind, void* ctx, su_U64 ctx_size);

SA_API su_String* su_string_create_cap(const char* data, su_U64 capacity, su_ReallocationKind realloc_kind);

SA_API const char* su_string_data(const su_String* str);

SA_API su_U64 su_string_length(const su_String* str);

SA_API su_Bool su_string_reserve(su_String* str, su_U64 new_cap);

SA_API void su_string_free(su_String* str);

su_Bool su_string_append(su_String* str, const char* suffix, su_U64 suffix_len);

su_Bool su_string_append_str(su_String* dest, const su_String* src);

su_Bool su_string_set(su_String* str, const char* new_data, su_U64 new_len);

su_Bool su_string_set_str(su_String* dest, const su_String* src);

void su_string_clear(su_String* str);

su_Bool su_string_shrink_to_fit(su_String* str);

int su_string_compare(const su_String* a, const char* b, su_U64 b_len);

int su_string_compare_str(const su_String* a, const su_String* b);

su_Bool su_string_format(su_String* str, const char* fmt, ...);

su_Bool su_string_append_format(su_String* str, const char* fmt, ...);

su_Bool su_string_is_empty(su_String* str);

#endif // SACI_UTILS_SU_TYPES_H
