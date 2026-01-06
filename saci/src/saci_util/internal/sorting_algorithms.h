#ifndef SACI_UTIL_INTERNAL_SORTING_ALGORITHMS_H
#define SACI_UTIL_INTERNAL_SORTING_ALGORITHMS_H

#include "saci_util/types.h"

SaciBool saci_bubble_sort(void* array, SaciU64 stride, SaciU64 array_length);

SaciBool saci_bubble_sort_ptr(void* array, SaciU64 stride, SaciU64 array_length, SaciU64 offset);

#endif //  SACI_UTIL_INTERNAL_SORTING_ALGORITHMS_H
