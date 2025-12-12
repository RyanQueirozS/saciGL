#ifndef SACI_UTILS_MEMORY_SU_DARRAY_H
#define SACI_UTILS_MEMORY_SU_DARRAY_H

#include "saci_util/defines.h"
#include "saci_util/types.h"
#include "saci_util/memory.h"

typedef struct SaciDArray SaciDArray;

#define SACI_SIZE_OF_DARRAY 40

SACI_API SaciDArray* saci_darray_create(SaciU64 capacity, SaciU64 elem_size, SaciBool fixed_size);

SACI_API SaciDArray* saci_darray_create_ctx(struct SaciMemChunk* memctx, SaciU64 elem_size);

SACI_API SaciDArray* saci_darray_create_ctx_void(void* memctx, SaciU64 ctx_size, SaciU64 capacity, SaciU64 elem_size);

SACI_API void saci_darray_free(SaciDArray* array);

SACI_API SaciBool saci_darray_clear(SaciDArray* array);

SACI_API SaciBool saci_darray_resize(SaciDArray* array, SaciU64 new_cap);

SACI_API SaciBool saci_darray_push(SaciDArray* array, const void* value);

SACI_API void saci_darray_pop(SaciDArray* array);

SACI_API const void* saci_darray_get(const SaciDArray* array, SaciU64 index);

SACI_API void* saci_darray_get_ptr(const SaciDArray* array, SaciU64 index);

SACI_API SaciU64 saci_darray_get_elem_size(const SaciDArray* array);

SACI_API void saci_darray_set(SaciDArray* array, SaciU64 index, const void* value, SaciU64 value_size);

SACI_API SaciU64 saci_darray_length(const SaciDArray* array);

SACI_API SaciU64 saci_darray_capacity(const SaciDArray* array);

SACI_API SaciBool saci_darray_append(SaciDArray* dest, const SaciDArray* src);

SACI_API void saci_darray_debug_print(const SaciDArray* arr);

SACI_API SaciBool saci_darray_is_null(const SaciDArray* arr);

SACI_API SaciBool saci_darray_is_empty(const SaciDArray* arr);

#endif // SACI_UTILS_MEMORY_SU_DARRAY_H
