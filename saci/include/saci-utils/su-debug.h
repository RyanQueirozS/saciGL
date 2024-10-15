#ifndef __SACI_UTILS_SU_DEBUG_H__
#define __SACI_UTILS_SU_DEBUG_H__

#include "saci-utils/su-types.h"

void saci_DebugMessageCallback(saci_u32 source, saci_u32 type, saci_u32 id,
                               saci_u32 severity, int length,
                               const char* msg, const void* data);

#endif
