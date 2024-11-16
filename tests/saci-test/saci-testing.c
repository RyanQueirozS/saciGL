#include "saci-testing.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#define SACI_TEST_ERROR_MAXIMUM 0x1000

static size_t __successIndex = 0;
struct Success {
    char* function;
    char* file;
    int line;
    char* description;
} __successes[SACI_TEST_ERROR_MAXIMUM];

static size_t __errorIndex = 0;
struct Error {
    char* function;
    char* file;
    int line;
    char* description;
} __errors[SACI_TEST_ERROR_MAXIMUM];

static clock_t __saci_testTimeStart = 0;
static clock_t __saci_testTimeEnd = 0;

static bool __printPassed = 0;

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

void saci_Test_PrintPassed(bool enable) { __printPassed = enable; }

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

void saci_Test_ClockBegin(void) { __saci_testTimeStart = clock(); }

void saci_Test_ClockEnd(void) { __saci_testTimeEnd = clock(); }

double saci_GetElapsedTimeMS(void) {
    return (double)(1000.0f * (__saci_testTimeEnd - __saci_testTimeStart) / CLOCKS_PER_SEC);
}
