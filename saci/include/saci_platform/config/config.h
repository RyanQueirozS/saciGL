#ifndef SACI_PLATFORM_CONFIG_CONFIG_H
#define SACI_PLATFORM_CONFIG_CONFIG_H

#include "saci_util/memory.h"
#include "saci_util/types.h"

struct PSaciConfigField {
    union {
        double float_val;
        SaciBool bool_val;
        SaciS64 int_val;
        SaciMemChunk* string_val;
        struct PSaciConfigField* nested;
    } data;
};

#endif // SACI_PLATFORM_CONFIG_CONFIG_H
