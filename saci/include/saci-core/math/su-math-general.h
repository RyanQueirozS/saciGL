#ifndef SACI_UTILS_MATH_SU_MATH_H
#define SACI_UTILS_MATH_SU_MATH_H

/* === Constants === */

/**
 * @define su_PI
 * @brief PI declaration up to 14 digits
 */
#define su_PI 3.141592653589793f

/**
 * @define su_DEG2RAD_MULT
 * @brief Multiplication constant value for degree to radian operations
 */
#define su_DEG2RAD_MULT (su_PI / 180.0f)

/**
 * @define su_DEG_TO_RAD_M
 * @brief Macro that returns a degree from a radian
 */
#define su_DEG_TO_RAD_M(x) ((x) * su_DEG2RAD_MULT)

/**
 * @define su_RAD2DEG_MULT
 * @brief Multiplication constant value for radian to degree operations
 */
#define su_RAD2DEG_MULT (180.0f / SACI_PI)

/**
 * @define su_RAD_TO_DEG_M
 * @brief Macro that returns a radian from a degree
 */
#define su_RAD_TO_DEG_M(x) ((x) * su_RAD2DEG_MULT)

struct su_MathPreferences {
    double (*sqrt_function)(double);
    double (*tan_function)(double);
};

struct su_MathPreferences su_math_get_preferences(void);

double su_math_sqrt(double val);

double su_math_tan(double val);

#endif // SACI_UTILS_MATH_SU_MATH_H
