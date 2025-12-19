// TODO delete
#include "saci_util/darray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saci_util/log.h"
#include "saci_util/internal/log.h"

SACI_INTERNAL SaciBool saci__darray_can_append(const SaciDArray* dest, const SaciDArray* src);

SACI_INTERNAL SaciBool saci__darray_ensure_capacity(SaciDArray* dest, SaciU64 required_capacity);

typedef struct SaciDArray {
    void* data;
    SaciU64 length;
    SaciU64 capacity;
    SaciU64 elem_size;
    SaciBool is_fixed_size;
} SaciDArray;

//  TODO remove
#define SACI_LOG_CONTEXT_CORE_DARRAY 0

SaciDArray* saci_darray_create(SaciU64 capacity, SaciU64 elem_size, SaciBool fixed_size)
{
    SACI_LOG_DUMMY_CHECK_M(capacity > 0, SACI_LOG_CONTEXT_CORE_DARRAY, "Capacity must be greater than zero");
    SACI_LOG_DUMMY_CHECK_M(elem_size > 0, SACI_LOG_CONTEXT_CORE_DARRAY, "Element size must be greater than zero");

    SaciDArray* array = calloc(1, sizeof(SaciDArray));
    SACI_LOG_ASSERT_M(array, SACI_LOG_CONTEXT_CORE_DARRAY, "Could not create dynamic array");

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = calloc(capacity, elem_size);
    SACI_LOG_ASSERT_M(array->data, SACI_LOG_CONTEXT_CORE_DARRAY, "Could not allocate memory for SaciDArray");
    return array;
}

SACI_API SaciDArray* saci_darray_create_ctx_void(void* memctx, SaciU64 ctx_size, SaciU64 capacity, SaciU64 elem_size)
{
    if (!memctx || elem_size == 0 || capacity == 0) {
        return NULL;
    }

    SaciU64 required_size = sizeof(SaciDArray) + (capacity * elem_size);
    if (ctx_size < required_size) {
        return NULL;
    }

    SaciDArray* arr = (SaciDArray*)memctx;

    void* data_ptr = (void*)((unsigned char*)memctx + sizeof(SaciDArray));

    arr->data = data_ptr;
    arr->length = 0;
    arr->capacity = capacity;
    arr->elem_size = elem_size;
    arr->is_fixed_size = true;

    return arr;
}

#if 0
SaciDArray* su_darray_create_ctx(struct su_MemChunk* memctx, SaciU64 elem_size) {
    SaciU64 struct_size = sizeof(SaciDArray);
    SaciU64 data_size = su_mem_chunk_get_capacity(memctx);
    SaciU64 total_size = struct_size + data_size;

    SACI_LOG_DUMMY_CHECK_M(memctx, SACI_LOG_CONTEXT_CORE_DARRAY, "Creating darray with context while context is NULL");
    if (memctx_size < total_size) {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_MEDIUM,
            SACI_LOG_CONTEXT_CORE_DARRAY,
            "Memory context is too small for SaciDArray and its data. Passed: (%lu) expected min: (%lu)",
            memctx_size, total_size);
        return NULL;
    }

    SaciDArray* array = (SaciDArray*)memctx;

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = (void*)((char*)memctx + struct_size);

    return array;
}
#endif

void saci_darray_free(SaciDArray* array)
{
    free(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
    array->elem_size = 0;
}

SaciBool saci_darray_clear(SaciDArray* array)
{
    if (!array) {
        return SACI_FALSE;
    }
    if (!array->data) {
        return SACI_FALSE;
    }
    array->length = 0;
    return SACI_TRUE;
}

SaciBool saci_darray_resize(SaciDArray* array, SaciU64 new_cap)
{
    SACI_LOG_ASSERT_M(new_cap > array->capacity, SACI_LOG_CONTEXT_CORE_DARRAY, "New capacity must be greater than current capacity");

    if (array->is_fixed_size) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_HIGH, SACI_LOG_CONTEXT_CORE_DARRAY,
                          "Cannot resize fixed-size SaciDArray");
        return SACI_FALSE;
    }

    void* new_data = realloc(array->data, new_cap * array->elem_size);
    SACI_LOG_ASSERT_M(new_data, SACI_LOG_CONTEXT_CORE_DARRAY, "Memory allocation failed during resizing");

    array->data = new_data;
    array->capacity = new_cap;
    return SACI_TRUE;
}

SaciBool saci_darray_push(SaciDArray* array, const void* value)
{
    SACI_LOG_ASSERT_M(array, SACI_LOG_CONTEXT_CORE_DARRAY, "Array is NULL");
    SACI_LOG_ASSERT_M(value, SACI_LOG_CONTEXT_CORE_DARRAY, "Value to push is NULL");

    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            SACI_LOG_ERRORF_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_CORE_DARRAY,
                              "Cannot push to full fixed-size SaciDArray");
            return SACI_FALSE;
        }
        SaciU64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!saci_darray_resize(array, new_cap)) {
            return SACI_FALSE;
        }
    }
    void* dest = (char*)array->data + array->length * array->elem_size;
    memcpy(dest, value, array->elem_size);
    array->length++;
    return SACI_TRUE;
}

void saci_darray_pop(SaciDArray* array)
{
    if (array->length == 0) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH, SACI_LOG_CONTEXT_CORE_DARRAY, "Cannot pop dynamic array, length is 0");
    }
    array->length--;
}

const void* saci_darray_get(const SaciDArray* array, SaciU64 index)
{
    SACI_LOG_DUMMY_CHECK_M(array, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray is NULL");
    SACI_LOG_DUMMY_CHECK_M(array->data, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray data is NULL");
    SACI_LOG_ASSERTF_M(index < array->length, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    return (char*)array->data + index * array->elem_size;
}

void* saci_darray_get_ptr(const SaciDArray* array, SaciU64 index)
{
    SACI_LOG_DUMMY_CHECK_M(array, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray is NULL");
    SACI_LOG_DUMMY_CHECK_M(array->data, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray data is NULL");
    SACI_LOG_ASSERTF_M(index < array->length, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    return (char*)array->data + index * array->elem_size;
}

SACI_API SaciU64 saci_darray_get_elem_size(const SaciDArray* array)
{
    return array->elem_size;
}

// TODO
void saci_darray_set(SaciDArray* array, SaciU64 index, const void* value, SaciU64 value_size)
{
    SACI_LOG_DUMMY_CHECK_M(array, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray is NULL");
    SACI_LOG_DUMMY_CHECK_M(array->data, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray data is NULL");
    SACI_LOG_DUMMY_CHECK_M(array->elem_size == value_size, SACI_LOG_CONTEXT_CORE_DARRAY, "Element size is not equal to value size");
    SACI_LOG_ASSERTF_M(value, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    SACI_LOG_ASSERTF_M(index < array->length, SACI_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    void* dest = (char*)array->data + index * array->elem_size;
    memcpy(dest, value, array->elem_size);
}

SaciU64 saci_darray_length(const SaciDArray* array)
{
    return array->length;
}

SaciU64 saci_darray_capacity(const SaciDArray* array)
{
    return array->capacity;
}

SaciBool saci_darray_append(SaciDArray* dest, const SaciDArray* src)
{
    if (!saci__darray_can_append(dest, src)) {
        return SACI_FALSE;
    }

    SaciU64 required_capacity = dest->length + src->length;
    if (!saci__darray_ensure_capacity(dest, required_capacity)) {
        return SACI_FALSE;
    }

    void* dest_ptr = (char*)dest->data + (dest->length * dest->elem_size);
    const void* src_ptr = src->data;
    memcpy(dest_ptr, src_ptr, src->length * src->elem_size);

    dest->length += src->length;
    return SACI_TRUE;
}

void saci_darray_debug_print(const SaciDArray* arr)
{
    printf("SaciDArray Debug: data=%p, length=%lu, capacity=%lu, elem_size=%lu, fixed=%d\n",
           arr->data, arr->length, arr->capacity, arr->elem_size, arr->is_fixed_size);
}

SaciBool saci_darray_is_null(const SaciDArray* arr)
{
    return arr->data == NULL;
}

SaciBool saci_darray_is_empty(const SaciDArray* arr)
{
    return arr->length == 0;
}

/* === DArray impl === */

SACI_INTERNAL SaciBool saci__darray_can_append(const SaciDArray* dest, const SaciDArray* src)
{
    SACI_LOG_ASSERT_M(dest, SACI_LOG_CONTEXT_CORE_DARRAY, "dest is NULL");
    SACI_LOG_ASSERT_M(src, SACI_LOG_CONTEXT_CORE_DARRAY, "src is NULL");
    SACI_LOG_ASSERT_M(dest->data, SACI_LOG_CONTEXT_CORE_DARRAY, "dest->data is NULL");
    SACI_LOG_ASSERT_M(src->data, SACI_LOG_CONTEXT_CORE_DARRAY, "src->data is NULL");
    SACI_LOG_ASSERT_M(dest->elem_size == src->elem_size, SACI_LOG_CONTEXT_CORE_DARRAY, "dest->elem_size is not src->elem_size");
    SACI_LOG_ASSERT_M(dest->elem_size > 0, SACI_LOG_CONTEXT_CORE_DARRAY, "dest->elem_size is less than or equal to 0");
    SACI_LOG_ASSERT_M(src->elem_size > 0, SACI_LOG_CONTEXT_CORE_DARRAY, "dest->elem_size is less than or equal to 0");
    if (src->length == 0) {
        return false;
    }

    return SACI_TRUE;
}

SACI_INTERNAL SaciBool saci__darray_ensure_capacity(SaciDArray* dest, SaciU64 required_capacity)
{
    if (required_capacity <= dest->capacity) {
        return SACI_TRUE;
    }

    if (dest->is_fixed_size) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_HIGH, SACI_LOG_CONTEXT_CORE_DARRAY,
                         "Cannot append to fixed-size SaciDArray");
        return SACI_FALSE;
    }

    SaciU64 new_capacity = dest->capacity ? dest->capacity : 1;
    while (new_capacity < required_capacity) {
        new_capacity *= 2;
    }

    return saci_darray_resize(dest, new_capacity);
}
