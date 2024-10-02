#ifndef __SACI_UTILS__
#define __SACI_UTILS__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#define SACI_SCAST_TO(type) static_cast<type>
#else
#define SACI_SCAST_TO(type) (type)
#endif

#define SACI_ARRLEN(array) (sizeof(array) / sizeof(array[0]))

typedef uint64_t saci_u64;
typedef uint32_t saci_u32;
typedef uint16_t saci_u16;
typedef uint8_t saci_u8;

typedef int64_t saci_s64;
typedef int32_t saci_s32;
typedef int16_t saci_s16;
typedef int8_t saci_s8;

typedef bool saci_Bool;
typedef saci_u32 saci_ShaderID;

typedef struct saci_Vec2 {
    float x, y;
} saci_Vec2;

typedef struct saci_Vec3 {
    float x, y, z;
} saci_Vec3;

typedef struct saci_Vec4 {
    float x, y, z, w;
} saci_Vec4;

typedef struct saci_Color {
    float r, g, b, a;
} saci_Color;

#endif
