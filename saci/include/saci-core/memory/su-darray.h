#ifndef SACI_UTILS_MEMORY_SU_DARRAY_H
#define SACI_UTILS_MEMORY_SU_DARRAY_H

#include "../su-general.h"
#include "../su-types-common.h"

#include "./su-memory.h"

typedef struct su_DArray su_DArray;

#define su_SIZE_OF_DARRAY 40

SA_API su_DArray* su_darray_create(su_U64 capacity, su_U64 elem_size, su_Bool fixed_size);

SA_API su_DArray* su_darray_create_ctx(struct su_MemChunk* memctx, su_U64 elem_size);

SA_API su_DArray* su_darray_create_ctx_void(void* memctx, su_U64 ctx_size, su_U64 capacity, su_U64 elem_size);

SA_API void su_darray_free(su_DArray* array);

SA_API su_Bool su_darray_clear(su_DArray* array);

SA_API su_Bool su_darray_resize(su_DArray* array, su_U64 new_cap);

SA_API su_Bool su_darray_push(su_DArray* array, const void* value);

SA_API void su_darray_pop(su_DArray* array);

SA_API const void* su_darray_get(const su_DArray* array, su_U64 index);

SA_API void* su_darray_get_ptr(const su_DArray* array, su_U64 index);

SA_API su_U64 su_darray_get_elem_size(const su_DArray* array);

SA_API void su_darray_set(su_DArray* array, su_U64 index, const void* value, su_U64 value_size);

SA_API su_U64 su_darray_length(const su_DArray* array);

SA_API su_U64 su_darray_capacity(const su_DArray* array);

SA_API su_Bool su_darray_append(su_DArray* dest, const su_DArray* src);

SA_API void su_darray_debug_print(const su_DArray* arr);

SA_API su_Bool su_darray_is_null(const su_DArray* arr);

SA_API su_Bool su_darray_is_empty(const su_DArray* arr);

#endif // SACI_UTILS_MEMORY_SU_DARRAY_H
