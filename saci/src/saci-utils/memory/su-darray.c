#include "saci-utils/memory/su-darray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SA_INTERNAL su_Bool su__darray_can_append(const su_DArray* dest, const su_DArray* src);

SA_INTERNAL su_Bool su__darray_ensure_capacity(su_DArray* dest, su_U64 required_capacity);

typedef struct su_DArray {
    void* data;
    su_U64 length;
    su_U64 capacity;
    su_U64 elem_size;
    su_Bool is_fixed_size;
} su_DArray;

su_DArray* su_darray_create(su_U64 capacity, su_U64 elem_size, su_Bool fixed_size) {
    su_LOG_DUMMY_CHECK_M(capacity > 0, su_LOG_CONTEXT_CORE_DARRAY, "Capacity must be greater than zero");
    su_LOG_DUMMY_CHECK_M(elem_size > 0, su_LOG_CONTEXT_CORE_DARRAY, "Element size must be greater than zero");

    su_DArray* array = calloc(1, sizeof(su_DArray));
    su_LOG_ASSERT_M(array, su_LOG_CONTEXT_CORE_DARRAY, "Could not create dynamic array");

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = calloc(capacity, elem_size);
    su_LOG_ASSERT_M(array->data, su_LOG_CONTEXT_CORE_DARRAY, "Could not allocate memory for su_DArray");
    return array;
}

#if 0
su_DArray* su_darray_create_ctx(struct su_MemChunk* memctx, su_U64 elem_size) {
    su_U64 struct_size = sizeof(su_DArray);
    su_U64 data_size = su_mem_chunk_get_capacity(memctx);
    su_U64 total_size = struct_size + data_size;

    su_LOG_DUMMY_CHECK_M(memctx, su_LOG_CONTEXT_CORE_DARRAY, "Creating darray with context while context is NULL");
    if (memctx_size < total_size) {
        su_LOG_ERRORF_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_MEDIUM,
            su_LOG_CONTEXT_CORE_DARRAY,
            "Memory context is too small for su_DArray and its data. Passed: (%lu) expected min: (%lu)",
            memctx_size, total_size);
        return NULL;
    }

    su_DArray* array = (su_DArray*)memctx;

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = (void*)((char*)memctx + struct_size);

    return array;
}
#endif

void su_darray_free(su_DArray* array) {
    free(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
    array->elem_size = 0;
}

su_Bool su_darray_clear(su_DArray* array) {
    if (!array) {
        return su_FALSE;
    }
    if (!array->data) {
        return su_FALSE;
    }
    array->length = 0;
    return su_TRUE;
}

su_Bool su_darray_resize(su_DArray* array, su_U64 new_cap) {
    su_LOG_ASSERT_M(new_cap > array->capacity, su_LOG_CONTEXT_CORE_DARRAY, "New capacity must be greater than current capacity");

    if (array->is_fixed_size) {
        su_LOG_ERRORF_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_DARRAY,
                        "Cannot resize fixed-size su_DArray");
        return su_FALSE;
    }

    void* new_data = realloc(array->data, new_cap * array->elem_size);
    su_LOG_ASSERT_M(new_data, su_LOG_CONTEXT_CORE_DARRAY, "Memory allocation failed during resizing");

    array->data = new_data;
    array->capacity = new_cap;
    return su_TRUE;
}

su_Bool su_darray_push(su_DArray* array, const void* value) {
    su_LOG_ASSERT_M(array, su_LOG_CONTEXT_CORE_DARRAY, "Array is NULL");
    su_LOG_ASSERT_M(value, su_LOG_CONTEXT_CORE_DARRAY, "Value to push is NULL");

    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            su_LOG_ERRORF_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_CORE_DARRAY,
                            "Cannot push to full fixed-size su_DArray");
            return su_FALSE;
        }
        su_U64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!su_darray_resize(array, new_cap)) {
            return su_FALSE;
        }
    }
    void* dest = (char*)array->data + array->length * array->elem_size;
    memcpy(dest, value, array->elem_size);
    array->length++;
    return su_TRUE;
}

void su_darray_pop(su_DArray* array) {
    if (array->length == 0) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_DARRAY, "Cannot pop dynamic array, length is 0");
    }
    array->length--;
}

const void* su_darray_get(const su_DArray* array, su_U64 index) {
    su_LOG_DUMMY_CHECK_M(array, su_LOG_CONTEXT_CORE_DARRAY, "DArray is NULL");
    su_LOG_DUMMY_CHECK_M(array->data, su_LOG_CONTEXT_CORE_DARRAY, "DArray data is NULL");
    su_LOG_ASSERTF_M(index < array->length, su_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    return (char*)array->data + index * array->elem_size;
}

void* su_darray_get_ptr(const su_DArray* array, su_U64 index) {
    su_LOG_DUMMY_CHECK_M(array, su_LOG_CONTEXT_CORE_DARRAY, "DArray is NULL");
    su_LOG_DUMMY_CHECK_M(array->data, su_LOG_CONTEXT_CORE_DARRAY, "DArray data is NULL");
    su_LOG_ASSERTF_M(index < array->length, su_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    return (char*)array->data + index * array->elem_size;
}

SA_API su_U64 su_darray_get_elem_size(const su_DArray* array) {
    return array->elem_size;
}

// TODO
void su_darray_set(su_DArray* array, su_U64 index, const void* value, su_U64 value_size) {
    su_LOG_DUMMY_CHECK_M(array, su_LOG_CONTEXT_CORE_DARRAY, "DArray is NULL");
    su_LOG_DUMMY_CHECK_M(array->data, su_LOG_CONTEXT_CORE_DARRAY, "DArray data is NULL");
    su_LOG_DUMMY_CHECK_M(array->elem_size == value_size, su_LOG_CONTEXT_CORE_DARRAY, "Element size is not equal to value size");
    su_LOG_ASSERTF_M(value, su_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    su_LOG_ASSERTF_M(index < array->length, su_LOG_CONTEXT_CORE_DARRAY, "DArray has length %lu cannot get at index %lu", array->length, index);
    void* dest = (char*)array->data + index * array->elem_size;
    memcpy(dest, value, array->elem_size);
}

su_U64 su_darray_length(const su_DArray* array) {
    return array->length;
}

su_U64 su_darray_capacity(const su_DArray* array) {
    return array->capacity;
}

su_Bool su_darray_append(su_DArray* dest, const su_DArray* src) {
    if (!su__darray_can_append(dest, src)) {
        return su_FALSE;
    }

    su_U64 required_capacity = dest->length + src->length;
    if (!su__darray_ensure_capacity(dest, required_capacity)) {
        return su_FALSE;
    }

    void* dest_ptr = (char*)dest->data + (dest->length * dest->elem_size);
    const void* src_ptr = src->data;
    memcpy(dest_ptr, src_ptr, src->length * src->elem_size);

    dest->length += src->length;
    return su_TRUE;
}

void su_darray_debug_print(const su_DArray* arr) {
    printf("su_DArray Debug: data=%p, length=%lu, capacity=%lu, elem_size=%lu, fixed=%d\n",
           arr->data, arr->length, arr->capacity, arr->elem_size, arr->is_fixed_size);
}

su_Bool su_darray_is_null(const su_DArray* arr) {
    return arr->data == NULL;
}

su_Bool su_darray_is_empty(const su_DArray* arr) {
    return arr->length == 0;
}

/* === DArray impl === */

SA_INTERNAL su_Bool su__darray_can_append(const su_DArray* dest, const su_DArray* src) {
    su_LOG_ASSERT_M(dest, su_LOG_CONTEXT_CORE_DARRAY, "dest is NULL");
    su_LOG_ASSERT_M(src, su_LOG_CONTEXT_CORE_DARRAY, "src is NULL");
    su_LOG_ASSERT_M(dest->data, su_LOG_CONTEXT_CORE_DARRAY, "dest->data is NULL");
    su_LOG_ASSERT_M(src->data, su_LOG_CONTEXT_CORE_DARRAY, "src->data is NULL");
    su_LOG_ASSERT_M(dest->elem_size == src->elem_size, su_LOG_CONTEXT_CORE_DARRAY, "dest->elem_size is not src->elem_size");
    su_LOG_ASSERT_M(dest->elem_size > 0, su_LOG_CONTEXT_CORE_DARRAY, "dest->elem_size is less than or equal to 0");
    su_LOG_ASSERT_M(src->elem_size > 0, su_LOG_CONTEXT_CORE_DARRAY, "dest->elem_size is less than or equal to 0");
    if (src->length == 0) {
        return false;
    }

    return su_TRUE;
}

SA_INTERNAL su_Bool su__darray_ensure_capacity(su_DArray* dest, su_U64 required_capacity) {
    if (required_capacity <= dest->capacity) {
        return su_TRUE;
    }

    if (dest->is_fixed_size) {
        su_LOG_ERROR_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_CORE_DARRAY,
                       "Cannot append to fixed-size su_DArray");
        return su_FALSE;
    }

    su_U64 new_capacity = dest->capacity ? dest->capacity : 1;
    while (new_capacity < required_capacity) {
        new_capacity *= 2;
    }

    return su_darray_resize(dest, new_capacity);
}
