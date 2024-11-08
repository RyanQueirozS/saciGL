// A poorly implemented and poorly designed testing lib
#ifndef __SACI_TESTING_H__
#define __SACI_TESTING_H__

#include <stddef.h>
#include <stdbool.h>

#define SACI_TEST_VEC3_IS_EQUAL(v1, v2) ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z))
#define SACI_TEST_VEC2_IS_EQUAL(v1, v2) ((v1.x == v2.x) && (v1.y == v2.y))

extern void saci_Test_Begin(void);

extern void saci_Test_End(void);

extern void saci_Test_End(void);

extern void saci_Test_PrintPassed(bool enable);

extern void saci_Test_Passed(char* func, char* file, int line);

extern void saci_Test_DidNotPass(char* func, char* file, int line);

extern void saci_Test_AddDescription(char* description);

extern void saci_Test_Assert(bool condition);

#define SACI_TEST_ASSERT(condition)                                    \
    do {                                                               \
        if (condition) {                                               \
            saci_Test_Passed((char*)__func__, __FILE__, __LINE__);     \
        } else {                                                       \
            saci_Test_DidNotPass((char*)__func__, __FILE__, __LINE__); \
        }                                                              \
    } while (0);

extern void saci_Test_ClockBegin(void);

extern void saci_Test_ClockEnd(void);

extern double saci_GetElapsedTimeMS(void);

#endif
