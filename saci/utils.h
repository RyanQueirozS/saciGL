#ifndef __SACI_UTILS__
#define __SACI_UTILS__

#include <stdint.h>

#ifdef __cplusplus
#define SCAST_TO(type) static_cast<type>
#else
#define SCAST_TO(type) (type)
#endif

#define ARRLEN(array) (sizeof(array) / sizeof(array[0]))

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef struct Vec2 {
    float x, y;
} Vec2;

typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Vec4 {
    float x, y, z, w;
} Vec4;

typedef struct Color {
    float r, g, b, a;
} Color;

#endif
