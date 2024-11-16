// A poorly implemented and poorly designed testing lib
#ifndef __SACI_TESTING_H__
#define __SACI_TESTING_H__

#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#define SACI_EPSILON 1e-6 // Tolerance for floating values

#define SACI_TEST_VEC3_IS_EQUAL(v1, v2)                                              \
    (fabs((v1.x) - (v2.x)) < SACI_EPSILON && fabs((v1.y) - (v2.y)) < SACI_EPSILON && \
     fabs((v1.z) - (v2.z)) < SACI_EPSILON)

#define SACI_TEST_VEC2_IS_EQUAL(v1, v2) \
    (fabs((v1.x) - (v2.x)) < SACI_EPSILON && fabs((v1.y) - (v2.y)) < SACI_EPSILON)

extern void saci_Test_AddDescription(char* description); // TODO

extern void saci_Test_End(void);

extern void saci_Test_PrintPassed(bool enable);

extern void saci_Test_Passed(char* func, char* file, int line, const char* description);

void saci_Test_DidNotPass(char* func, char* file, int line, const char* description);

#define SACI_TEST_ASSERT(condition, description)                                    \
    do {                                                                            \
        if (condition) {                                                            \
            saci_Test_Passed((char*)__func__, __FILE__, __LINE__, description);     \
        } else {                                                                    \
            saci_Test_DidNotPass((char*)__func__, __FILE__, __LINE__, description); \
        }                                                                           \
    } while (0);

extern void saci_Test_ClockBegin(void);

extern void saci_Test_ClockEnd(void);

extern double saci_GetElapsedTimeMS(void);

#endif
