#ifndef __SACI_UTILS__
#define __SACI_UTILS__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#define SACI_SCAST_TO(type) static_cast<type>
#else
#define SACI_SCAST_TO(type) (type)
#endif

#define SACI_LOG_LEVEL_INFO    0
#define SACI_LOG_LEVEL_WARNING 1
#define SACI_LOG_LEVEL_ERROR   2

#define SACI_LOGGERR(level, infoType, msg)                                     \
    do {                                                                       \
        const char *levelStr = (level == SACI_LOG_LEVEL_INFO)      ? "INFO"    \
                               : (level == SACI_LOG_LEVEL_WARNING) ? "WARNING" \
                               : (level == SACI_LOG_LEVEL_ERROR)   ? "ERROR"   \
                                                                   : "UNKNOWN";  \
        fprintf(stderr, "%s: %s. %s\n", levelStr, infoType, msg);              \
    } while (0)

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

typedef struct saci_Mat4 {
    float m[4][4];
} saci_Mat4;

#endif
