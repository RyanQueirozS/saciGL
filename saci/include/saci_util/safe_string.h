#ifndef SACI_UTIL_STRING_H
#define SACI_UTIL_STRING_H

#include "saci_util/types.h"
#include "saci_util/defines.h"

SACI_API SaciU64 saci_safe_str_len(const char* s, SaciU64 maxlen);
SACI_API SaciU64 saci_safe_str_size(const char* s, SaciU64 maxlen); // Same as strlen just adds the null terminator

#endif // SACI_UTIL_STRING_H
