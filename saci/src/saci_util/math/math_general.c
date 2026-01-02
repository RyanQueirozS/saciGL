#include "saci_util/math.h"

#include <math.h>

SACI_STATIC double saci__default_sqrt_s(double x)
{ // wrapps math.h sqrt func
    return sqrt(x);
}

SACI_STATIC double saci__default_tan_s(double x)
{ // wrapps math.h tan func
    return tan(x);
}

SACI_STATIC struct
{
    double (*sqrt_function)(double);
    double (*tan_function)(double);
} saci_g_math_preferences = {
    .sqrt_function = saci__default_sqrt_s,
    .tan_function = saci__default_tan_s,
};

double saci_math_sqrt(double val)
{
    return saci_g_math_preferences.sqrt_function(val);
}

double saci_math_tan(double val)
{
    return saci_g_math_preferences.tan_function(val);
}
