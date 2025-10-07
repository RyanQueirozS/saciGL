#include "saci-utils/math/su-math-color.h"

su_Color su_color_from_hex(su_U32 hex) {
    su_Color color;
    color.r = su_CAST_M(float)(((hex >> 24)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.g = su_CAST_M(float)(((hex >> 16)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.b = su_CAST_M(float)(((hex >> 8)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.a = su_CAST_M(float)((hex & 0xFF)) * su_COLOR_8BIT_INVERSE_MAX;
    return color;
}

su_Color su_color_from_u8(su_U8 r, su_U8 g, su_U8 b, su_U8 a) {
    su_Color color;
    color.r = su_CAST_M(float)(r) * su_COLOR_8BIT_INVERSE_MAX;
    color.g = su_CAST_M(float)(g) * su_COLOR_8BIT_INVERSE_MAX;
    color.b = su_CAST_M(float)(b) * su_COLOR_8BIT_INVERSE_MAX;
    color.a = su_CAST_M(float)(a) * su_COLOR_8BIT_INVERSE_MAX;
    return color;
}

su_U32 su_color_to_hex(su_Color color) {
    su_U32 hex = ((su_U32)(color.r * su_COLOR_8BIT_MAX) << 24) |
                 ((su_U32)(color.g * su_COLOR_8BIT_MAX) << 16) |
                 ((su_U32)(color.b * su_COLOR_8BIT_MAX) << 8) |
                 (su_U32)(color.a * su_COLOR_8BIT_MAX);
    return hex;
}
