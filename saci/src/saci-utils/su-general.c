#include "saci-utils/su-general.h"

sa_u64 sa_Size_Of_Type(sa_dataType data_type) {
    switch (data_type) {
    case SA_TYPE_U8:
    case SA_TYPE_S8:
    case SA_TYPE_BOOL:
        return 1;

    case SA_TYPE_U16:
    case SA_TYPE_S16:
        return 2;

    case SA_TYPE_U32:
    case SA_TYPE_S32:
    case SA_TYPE_SHADERID:
    case SA_TYPE_TEXTUREID:
    case SA_TYPE_BUFFERID:
        return 4;

    case SA_TYPE_U64:
    case SA_TYPE_S64:
    case SA_TYPE_UV:
    case SA_TYPE_MAT2:
    case SA_TYPE_VEC2:
        return 8;

    case SA_TYPE_VEC4:
    case SA_TYPE_COLOR:
    case SA_TYPE_MAT4:
        return 16;

    case SA_TYPE_VEC3:
    case SA_TYPE_MAT3:
    case SA_TYPE_MAT2X3:
    case SA_TYPE_MAT3X2:
        return 12;

    case SA_TYPE_MAT2X4:
    case SA_TYPE_MAT3X4:
    case SA_TYPE_MAT4X2:
    case SA_TYPE_MAT4X3:
        return 16;

    default:
        return 0;
    }
}
