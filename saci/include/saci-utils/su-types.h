#ifndef __SACI_UTILS_SU_TYPES_H__
#define __SACI_UTILS_SU_TYPES_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t saci_u64;
typedef uint32_t saci_u32;
typedef uint16_t saci_u16;
typedef uint8_t saci_u8;

typedef int64_t saci_s64;
typedef int32_t saci_s32;
typedef int16_t saci_s16;
typedef int8_t saci_s8;

#define SACI_TRUE true
#define SACI_FALSE false
typedef bool saci_Bool;

typedef saci_u32 saci_ShaderID;
typedef saci_u32 saci_TextureID;

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

typedef struct saci_Mat4 {
    float m[4][4];
} saci_Mat4;

#endif
