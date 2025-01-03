/** Copyright 2022 Ryan Queiroz <ryanqueiroz@proton.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __SACI_TESTING_H__
#define __SACI_TESTING_H__

#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#ifndef SACI_EPSILON
/**
 * @def SACI_EPSILON
 * @brief A small threshold value used for floating-point comparisons.
 */
#define SACI_EPSILON 1e-6
#endif // SACI_EPSILON

#ifndef SACI_TEST_VEC3_IS_EQUAL
/**
 * @def SACI_TEST_VEC3_IS_EQUAL
 * @brief Macro to test if two 3D vectors are equal within a tolerance.
 *
 * @param v1 The first vector to compare, must have members `x`, `y`, and `z`.
 * @param v2 The second vector to compare, must have members `x`, `y`, and `z`.
 * @return A boolean value: true if the vectors are equal within the tolerance,
 *         false otherwise.
 */
#define SACI_TEST_VEC3_IS_EQUAL(v1, v2)                                                      \
    (fabs((v1.m_x) - (v2.m_x)) < SACI_EPSILON && fabs((v1.m_y) - (v2.m_y)) < SACI_EPSILON && \
     fabs((v1.m_z) - (v2.m_z)) < SACI_EPSILON)
#endif // SACI_TEST_VEC3_IS_EQUAL

#ifndef SACI_TEST_VEC2_IS_EQUAL
/**
 * @def SACI_TEST_VEC2_IS_EQUAL
 * @brief Macro to test if two 2D vectors are equal within a tolerance.
 *
 * @param v1 The first vector to compare, must have members `x`, `y`.
 * @param v2 The second vector to compare, must have members `x`, `y`.
 * @return A boolean value: true if the vectors are equal within the tolerance,
 *         false otherwise.
 */
#define SACI_TEST_VEC2_IS_EQUAL(v1, v2) \
    (fabs((v1.m_x) - (v2.m_x)) < SACI_EPSILON && fabs((v1.m_y) - (v2.m_y)) < SACI_EPSILON)
#endif // SACI_TEST_VEC2_IS_EQUAL

#ifndef SACI_TEST_COLOR_IS_EQUAL
/**
 * @def SACI_TEST_COLOR_IS_EQUAL
 * @brief Macro to test if two 2D vectors are equal within a tolerance.
 *
 * @param c1 The first vector to compare, must have members `r`, `g`, 'b' and 'a'.
 * @param c2 The second vector to compare, must have members `r`, `g`, 'b' and 'a'.
 * @return A boolean value: true if the vectors are equal within the tolerance,
 *         false otherwise.
 */
#define SACI_TEST_COLOR_IS_EQUAL(c1, c2)                                                     \
    (fabs((c1.m_r) - (c2.m_r)) < SACI_EPSILON && fabs((c1.m_g) - (c2.m_g)) < SACI_EPSILON && \
     fabs((c1.m_b) - (c2.m_b)) < SACI_EPSILON && fabs((c1.m_a) - (c2.m_a)) < SACI_EPSILON)
#endif // SACI_TEST_COLOR_IS_EQUAL

#ifndef SACI_RAND_RANGE_INT
/**
 * @def SACI_RAND_RANGE_INT
 * @brief Macro to generate a maximum integer value given a maximum range.
 *
 * @param max The maximum value the integer can be.
 * @param goNegative A boolean value if the integer can go below 0.
 * @return A integer value given the range and goNegative.
 */
#define SACI_RAND_RANGE_INT(max, goNegative) \
    ((goNegative) ? (rand() % ((max) * 2 + 1) - (max)) : (rand() % ((max) + 1)))
#endif // SACI_RAND_RANGE_INT

#ifndef SACI_RAND_RANGE_FLOAT
/**
 * @def SACI_RAND_RANGE_FLOAT
 * @brief Macro to generate a maximum float value given a maximum range.
 *
 * @param max The maximum value the float can be.
 * @param goNegative A boolean value if the float can go below 0.
 * @return A float value given the range and goNegative.
 */
#define SACI_RAND_RANGE_FLOAT(max, goNegative)                              \
    ((goNegative) ? (((float)rand() / (float)RAND_MAX) * (max) * 2 - (max)) \
                  : (((float)rand() / (float)RAND_MAX) * (max)))
#endif // SACI_RAND_RANGE_FLOAT

/**
 * @brief Ends the testing and prints the results.
 */
void saci_Test_End(void);

/**
 * @brief Enables or disables if should print passed values.
 *
 * @param enable A boolean to enable or disable printing.
 */
void saci_Test_PrintPassed(bool enable);

/**
 * @brief Creates and pushes a test to success. Should not be used by the end-user.
 *
 * @param func The function name, passed as __func__.
 * @param file The file name, passed as __FILE__.
 * @param line The line name, passed as __LINE__.
 * @param description The test description.
 */
void saci_Test_Passed(char* func, char* file, int line, const char* description);

/**
 * @brief Creates and pushes a test to errors. Should not be used by the end-user.
 *
 * @param func The function name, passed as __func__.
 * @param file The file name, passed as __FILE__.
 * @param line The line name, passed as __LINE__.
 * @param description The test description.
 */
void saci_Test_DidNotPass(char* func, char* file, int line, const char* description);

/**
 * @brief Begins the clock.
 */
void saci_Test_ClockBegin(void);

/**
 * @brief Ends the clock.
 */
void saci_Test_ClockEnd(void);

/**
 * @brief Returns the elapsed time from ClockBegin and ClockEnd.
 *
 * @return A double with the value of the time elapsed in ms.
 */
double saci_Test_GetElapsedTimeMS(void);

#ifndef SACI_TEST_ASSERT
/**
 * @def SACI_TEST_ASSERT
 * @brief checks if a condition is true.
 *
 * @param condition The condition to be evaluated.
 * @param description The test description.
 */
#define SACI_TEST_ASSERT(condition, description)                                    \
    do {                                                                            \
        if (condition) {                                                            \
            saci_Test_Passed((char*)__func__, __FILE__, __LINE__, description);     \
        } else {                                                                    \
            saci_Test_DidNotPass((char*)__func__, __FILE__, __LINE__, description); \
        }                                                                           \
    } while (0);
#endif // SACI_TEST_ASSERT

/* === Implementation === */

#ifdef SACI_TEST_IMPLEMENTATION

#ifndef SACI_TEST_MAXIMUM
/**
 * @def SACI_TEST_MAXIMUM
 * @brief The max value of successes and/or errors.
 */
#define SACI_TEST_MAXIMUM 0x1000
#endif // SACI_TEST_ERROR_MAXIMUM

static bool __printPassed = 0;

static size_t __successIndex = 0;
struct Success {
    char* function;
    char* file;
    int line;
    char* description;
} __successes[SACI_TEST_MAXIMUM];

static size_t __errorIndex = 0;
struct Error {
    char* function;
    char* file;
    int line;
    char* description;
} __errors[SACI_TEST_MAXIMUM];

void saci_Test_End(void) {
    if (__printPassed) {
        for (size_t i = 0; i < __successIndex; ++i) {
            struct Success success = __successes[i];
            printf("\033[1;32m%s:%d - %s\033[0m\n", success.file, success.line,
                   success.description);
        }
    }

    for (size_t i = 0; i < __errorIndex; ++i) {
        struct Error error = __errors[i];
        printf("\033[1;31m%s:%d - %s\033[0m\n", error.file, error.line, error.description);
    }
    printf("\033[0;32mPassed: %lu\033[0m\n", __successIndex);
    printf("\033[0;31mDid NOT pass: %lu\033[0m\n", __errorIndex);
}

void saci_Test_PrintPassed(bool enable) {
    __printPassed = enable;
}

static clock_t __saci_testTimeStart = 0;
static clock_t __saci_testTimeEnd = 0;

void saci_Test_Passed(char* func, char* file, int line, const char* description) {
    for (size_t i = 0; i < __successIndex; ++i) {
        struct Success success = __successes[i];
        if (success.description == description && success.file == file && success.line == line &&
            success.function == func)
            return;
    }
    __successes[__successIndex++] = (struct Success){
        .function = func,
        .file = file,
        .line = line,
        .description = (char*)description,
    };
}

void saci_Test_DidNotPass(char* func, char* file, int line, const char* description) {
    for (size_t i = 0; i < __errorIndex; ++i) {
        struct Error error = __errors[i];
        if (error.description == description && error.file == file && error.line == line &&
            error.function == func)
            return;
    }
    __errors[__errorIndex++] = (struct Error){
        .function = func,
        .file = file,
        .line = line,
        .description = (char*)description,
    };
}

void saci_Test_ClockBegin(void) {
    __saci_testTimeStart = clock();
}

void saci_Test_ClockEnd(void) {
    __saci_testTimeEnd = clock();
}

double saci_Test_GetElapsedTimeMS(void) {
    return (double)(1000.0f * (__saci_testTimeEnd - __saci_testTimeStart) / CLOCKS_PER_SEC);
}

#endif

#endif
