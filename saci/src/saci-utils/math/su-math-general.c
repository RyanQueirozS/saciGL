#include "saci-utils/math/su-math-general.h"

#include <math.h>

static double su__default_sqrt_s(double x) { // wrapps math.h sqrt func
    return sqrt(x);
}

static double su__default_tan_s(double x) { // wrapps math.h tan func
    return tan(x);
}

static struct su_MathPreferences su__math_preferences = {
    .sqrt_function = su__default_sqrt_s,
    .tan_function = su__default_tan_s,
};

struct su_MathPreferences su_math_get_preferences(void) {
    return su__math_preferences;
}

double su_math_sqrt(double val) {
    return su__math_preferences.sqrt_function(val);
}

double su_math_tan(double val) {
    return su__math_preferences.tan_function(val);
}
