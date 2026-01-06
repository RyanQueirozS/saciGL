/* @file Source file implementing internal sorting algorithms used throughout
 * the whole library
 */

#include "saci_util/internal/sorting_algorithms.h"

#include <stddef.h>
#include <string.h>

SaciBool saci_bubble_sort(void* array, SaciU64 stride, SaciU64 array_length)
{
    if (array == NULL) {
        return SACI_FALSE;
    }
    if (array_length < 2) {
        return SACI_TRUE; // No need to sort
    }

    for (SaciU64 i = 0; i < array_length - 1; ++i) {
        for (SaciU64 j = 0; j < array_length - 1 - i; ++j) {
            void* a = (void*)((char*)array + j * stride);
            void* b = (void*)((char*)array + (j + 1) * stride);

            // Suposes that a byte comparison is correct, if not feel free to add
            // a improved or another version of the bubblesort that can cover
            // that edge case.
            if (memcmp(a, b, stride) > 0) {
                int temp = *(int*)a;
                *(int*)a = *(int*)b;
                *(int*)b = temp;
            }
        }
    }

    return SACI_TRUE;
}

SaciBool saci_bubble_sort_ptr(void* array, SaciU64 stride, SaciU64 array_length, SaciU64 offset)
{
    if (array == NULL) {
        return SACI_FALSE;
    }
    if (array_length < 2) {
        return SACI_TRUE;
    }

    for (SaciU64 i = 0; i < array_length - 1; ++i) {
        for (SaciU64 j = 0; j < array_length - 1 - i; ++j) {
            void* a = (void*)((char*)array + j * stride);
            void* b = (void*)((char*)array + (j + 1) * stride);

            void* a_offset = (void*)((char*)a + offset);
            void* b_offset = (void*)((char*)b + offset);

            // Suposes that a byte comparison is correct, if not feel free to add
            // a improved or another version of the bubblesort that can cover
            // that edge case.
            if (memcmp(a_offset, b_offset, stride - offset) > 0) {
                for (SaciU64 k = 0; k < stride; ++k) {
                    char temp = *((char*)a + k);
                    *((char*)a + k) = *((char*)b + k);
                    *((char*)b + k) = temp;
                }
            }
        }
    }

    return SACI_TRUE;
}
