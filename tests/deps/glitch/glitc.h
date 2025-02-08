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

/**
 * How to use the library:
 *
 * #define GLITCH_STD
 * #include "path-to/glitc.h"
 */
#ifndef __GLITCH_H__
#define __GLITCH_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef SACI_DEV
 #define GLITCH_STD
#endif // SACI_DEV

#ifndef GLITCH_DEF

 #ifdef _WIN32
  #ifdef BUILD_GLITCH_LIB
   #define GLITCH_DEF __declspec(dllexport) // Export symbols when building the library
  #else
   #define GLITCH_DEF __declspec(dllimport) // Import symbols when using the library
  #endif
 #else
  #define GLITCH_DEF // Non-Windows platforms don't need special decoration
 #endif // _WIN32

#endif // GLITCH_DEF

#ifndef GLITCH_IMPL
 #define GLITCH_IMPL inline GLITCH_DEF
#endif // GLITCH_IMPL

// The max value of successes and/or errors. Redefine if needed and include the
// header after.
#ifndef GLITCH_MAXIMUM
 #define GLITCH_MAXIMUM 0x1000
#endif // GLITCH_MAXIMUM

typedef struct GlitchTester GlitchTester; // Struct that shoulden't be created by the user

GLITCH_DEF GlitchTester* glitch_Tester_New();

GLITCH_DEF void glitch_Print_Passed(GlitchTester* t, bool enable);
GLITCH_DEF void glitch_End(GlitchTester* t);

void __glitch_Passed(GlitchTester* t, char* func, char* file, int line, const char* description);
void __glitch_Did_Not_Pass(GlitchTester* t, char* func, char* file, int line, const char* description);

#ifndef GLITCH_ASSERT
 #define GLITCH_ASSERT(tester, condition, description)                                        \
     do {                                                                                     \
         if (condition) {                                                                     \
             __glitch_Passed(tester, (char*)__func__, __FILE__, __LINE__, description);       \
         } else {                                                                             \
             __glitch_Did_Not_Pass(tester, (char*)__func__, __FILE__, __LINE__, description); \
         }                                                                                    \
     } while (0);
#endif // GLITCH_ASSERT

/* === GLITCH_STD Implementation === */

#ifdef GLITCH_STD

struct __Error {
    char* function;
    char* file;
    int   line;
    char* description;
};

struct __Success {
    char* function;
    char* file;
    int   line;
    char* description;
};

typedef struct GlitchTester {
    bool             print_passed;
    size_t           success_index;
    struct __Success successes[GLITCH_MAXIMUM];
    size_t           error_index;
    struct __Error   errors[GLITCH_MAXIMUM];
} GlitchTester;

GLITCH_IMPL GlitchTester* glitch_Tester_New() {
    GlitchTester* t  = (GlitchTester*)malloc(sizeof(GlitchTester));
    t->print_passed  = 0;
    t->success_index = 0;
    t->error_index   = 0;

    return t;
}

GLITCH_IMPL void glitch_End(GlitchTester* t) {
    if (t->print_passed) {
        for (size_t i = 0; i < t->success_index; ++i) {
            struct __Success success = t->successes[i];
            printf("\033[1;32m%s:%d - %s\033[0m\n", success.file, success.line,
                   success.description);
        }
    }

    for (size_t i = 0; i < t->error_index; ++i) {
        struct __Error error = t->errors[i];
        printf("\033[1;31m%s:%d - %s\033[0m\n", error.file, error.line, error.description);
    }
    printf("\033[0;32mPassed: %lu\033[0m\n", t->success_index);
    printf("\033[0;31mDid NOT pass: %lu\033[0m\n", t->error_index);
}

GLITCH_IMPL void glitch_Print_Passed(GlitchTester* t, bool enable) {
    t->print_passed = enable;
}

void __glitch_Passed(GlitchTester* t, char* func, char* file, int line, const char* description) {
    for (size_t i = 0; i < t->success_index; ++i) {
        struct __Success success = t->successes[i];
        if (success.description == description && success.file == file && success.line == line &&
            success.function == func)
            return;
    }
    t->successes[t->success_index++] = (struct __Success){
        .function    = func,
        .file        = file,
        .line        = line,
        .description = (!description ? "\0" : (char*)description),
    };
}

void __glitch_Did_Not_Pass(GlitchTester* t, char* func, char* file, int line, const char* description) {
    for (size_t i = 0; i < t->error_index; ++i) {
        struct __Error error = t->errors[i];
        if (error.description == description && error.file == file && error.line == line &&
            error.function == func)
            return;
    }
    t->errors[t->error_index++] = (struct __Error){
        .function    = func,
        .file        = file,
        .line        = line,
        .description = (!description ? "\0" : (char*)description),
    };
}

#endif

#endif
