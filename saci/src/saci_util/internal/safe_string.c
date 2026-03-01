#include "saci_util/safe_string.h"
#include "saci_util/internal/max_values.h"

#define SACI_SAFE_STR_SIZECAP_M(strsize)    \
    if ((strsize) > SACI_MAX_STRING_SIZE) { \
        (strsize) = SACI_MAX_STRING_SIZE;   \
    }

SACI_API SaciU64 saci_safe_str_len(const char* s, SaciU64 maxlen)
{
    if (!s) {
        return 0;
    }

    if (maxlen == 0) {
        maxlen = SACI_MAX_STRING_SIZE;
    }

    SACI_SAFE_STR_SIZECAP_M(maxlen);

    SaciU64 i;
    for (i = 0; i < maxlen && s[i]; ++i) {
    }
    return i;
}

SACI_API SaciU64 saci_safe_str_size(const char* s, SaciU64 maxlen)
{
    if (!s) {
        return 0;
    }

    if (maxlen == 0) {
        maxlen = SACI_MAX_STRING_SIZE;
    }

    SACI_SAFE_STR_SIZECAP_M(maxlen);

    SaciU64 len = saci_safe_str_len(s, maxlen);

    if (len == maxlen) {
        return len;
    }

    return ++len;
}
