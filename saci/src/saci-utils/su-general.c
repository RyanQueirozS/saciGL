#include "saci-utils/su-general.h"

int saci_Atoi(const char* c) {
    int value = 0;
    int sign = 1;
    if (*c == '+' || *c == '-') {
        if (*c == '-') sign = -1;
        c++;
    }
    while (((*c) >= '0') && ((*c) <= '9')) { /* isdigit(*c) */
        value *= 10;
        value += (int)(*c - '0');
        c++;
    }
    return value * sign;
}
