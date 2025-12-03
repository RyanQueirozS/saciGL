#include "saci-core/math.h"

#include "saci-core/general_internal.h"

#define SACI_COLOR_8BIT_MAX (255.0f)
#define SACI_COLOR_8BIT_INVERSE_MAX (1.0f / SACI_COLOR_8BIT_MAX)

SaciColor saci_color_from_hex(SaciU32 hex)
{
    SaciColor color;
    color.r = SACI_CAST_M(float)(((hex >> 24)) & 0xFF) * SACI_COLOR_8BIT_INVERSE_MAX;
    color.g = SACI_CAST_M(float)(((hex >> 16)) & 0xFF) * SACI_COLOR_8BIT_INVERSE_MAX;
    color.b = SACI_CAST_M(float)(((hex >> 8)) & 0xFF) * SACI_COLOR_8BIT_INVERSE_MAX;
    color.a = SACI_CAST_M(float)((hex & 0xFF)) * SACI_COLOR_8BIT_INVERSE_MAX;
    return color;
}

SaciColor saci_color_from_u8(SaciU8 r, SaciU8 g, SaciU8 b, SaciU8 a)
{
    SaciColor color;
    color.r = SACI_CAST_M(float)(r) * SACI_COLOR_8BIT_INVERSE_MAX;
    color.g = SACI_CAST_M(float)(g) * SACI_COLOR_8BIT_INVERSE_MAX;
    color.b = SACI_CAST_M(float)(b) * SACI_COLOR_8BIT_INVERSE_MAX;
    color.a = SACI_CAST_M(float)(a) * SACI_COLOR_8BIT_INVERSE_MAX;
    return color;
}

SaciU32 saci_color_to_hex(SaciColor color)
{
    SaciU32 hex = ((SaciU32)(color.r * SACI_COLOR_8BIT_MAX) << 24) |
                  ((SaciU32)(color.g * SACI_COLOR_8BIT_MAX) << 16) |
                  ((SaciU32)(color.b * SACI_COLOR_8BIT_MAX) << 8) |
                  (SaciU32)(color.a * SACI_COLOR_8BIT_MAX);
    return hex;
}
