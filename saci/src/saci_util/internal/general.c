#include "general.h"

size_t saci_strlen(const char* s, size_t maxlen)
{
    if (!s) {
        return 0;
    }

    if (maxlen == 0) {
        maxlen = SACI_MAX_STRING_SIZE;
    }

    size_t i;
    for (i = 0; i < maxlen && s[i]; ++i) {
    }
    return i;
}

size_t saci_strsize(const char* s, size_t maxlen)
{
    if (!s) {
        return 0;
    }

    if (maxlen == 0) {
        maxlen = SACI_MAX_STRING_SIZE;
    }

    size_t len = saci_strlen(s, maxlen);

    if (len == maxlen) {
        return len;
    }

    return ++len;
}
