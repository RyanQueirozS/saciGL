#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

#include <string.h>

/* === DArray === */

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
    su_LOG_ASSERT_MESSAGE_M(capacity > 0, "Capacity must be greater than zero");
    su_LOG_ASSERT_MESSAGE_M(elem_size > 0, "Element size must be greater than zero");

    su_DArray* array = su_CALLOC_M(1, sizeof(su_DArray));
    su_LOG_ASSERT_MESSAGE_M(array, "dArray couldn't be created");

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = su_CALLOC_M(capacity, elem_size);
    su_LOG_ASSERT_MESSAGE_M(array->data, "Could not allocate memory for su_DArray");
    return array;
}

su_DArray* su_darray_create_ctx(void* memctx, su_U64 memctx_size, su_U64 capacity, su_U64 elem_size, su_Bool fixed_size) {
    su_U64 struct_size = sizeof(su_DArray);
    su_U64 data_size = capacity * elem_size;
    su_U64 total_size = struct_size + data_size;

    su_LOG_ASSERTF_MESSAGE_M(memctx && memctx_size >= total_size,
                             "Memory context is too small for su_DArray and its "
                             "data. Passed: (%lu) expected min: (%lu)",
                             memctx_size, total_size);

    su_DArray* array = (su_DArray*)memctx;

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = (void*)((char*)memctx + struct_size);

    return array;
}

void su_darray_free(su_DArray* array) {
    su_FREE_M(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
    array->elem_size = 0;
}

void su_darray_clear(su_DArray* array) {
    su_LOG_ASSERT_MESSAGE_M(array, "Array is NULL");
    su_LOG_ASSERT_MESSAGE_M(array->data, "Array data is NULL");
    array->length = 0;
}

su_Bool su_darray_resize(su_DArray* array, su_U64 new_cap) {
    su_LOG_ASSERT_MESSAGE_M(new_cap > array->capacity, "New capacity must be greater than current capacity");

    if (array->is_fixed_size) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_MEMORY,
                              "Cannot resize fixed-size su_DArray");
        return su_FALSE;
    }

    void* new_data = realloc(array->data, new_cap * array->elem_size);
    su_LOG_ASSERT_MESSAGE_M(new_data, "Memory allocation failed during resizing");

    array->data = new_data;
    array->capacity = new_cap;
    return su_TRUE;
}

su_Bool su_darray_push(su_DArray* array, const void* value) {
    su_LOG_ASSERT_MESSAGE_M(array, "Array is NULL");
    su_LOG_ASSERT_MESSAGE_M(value, "Value to push is NULL");

    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_MEMORY,
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
    su_LOG_ASSERT_MESSAGE_M(array->length > 0, "Length is zero cannot pop su_DArray");
    array->length--;
}

void su_darray_get(const su_DArray* array, su_U64 index, void* out_value) {
    su_LOG_ASSERT_MESSAGE_M(array, "Array is NULL");
    su_LOG_ASSERT_MESSAGE_M(array->data, "Array data is NULL");
    su_LOG_ASSERTF_MESSAGE_M(index < array->length,
                             "su_DArray accessed at %lu while length is %lu", index, array->length);
    const void* src = (const char*)array->data + index * array->elem_size;
    memcpy(out_value, src, array->elem_size);
}

void* su_darray_get_ptr(const su_DArray* array, su_U64 index) {
    su_LOG_ASSERTF_MESSAGE_M(index < array->length,
                             "su_DArray accessed at %lu while length is %lu", index, array->length);
    return (char*)array->data + index * array->elem_size;
}

void su_darray_set(su_DArray* array, su_U64 index, const void* value) {
    su_LOG_ASSERT_MESSAGE_M(array, "Array is NULL");
    su_LOG_ASSERT_MESSAGE_M(array->data, "Array data is NULL");
    su_LOG_ASSERT_MESSAGE_M(value, "Value is NULL");
    su_LOG_ASSERTF_MESSAGE_M(index < array->length,
                             "su_DArray accessed at %lu while length is %lu", index, array->length);
    void* dest = (char*)array->data + index * array->elem_size;
    memcpy(dest, value, array->elem_size);
}

su_U64 su_darray_length(const su_DArray* array) {
    return array->length;
}

su_U64 su_darray_capacity(const su_DArray* array) {
    return array->capacity;
}

void su_darray_append(su_DArray* dest, const su_DArray* src) {
    if (!su__darray_can_append(dest, src)) {
        return;
    }

    su_U64 required_capacity = dest->length + src->length;
    if (!su__darray_ensure_capacity(dest, required_capacity)) {
        return;
    }

    void* dest_ptr = (char*)dest->data + (dest->length * dest->elem_size);
    const void* src_ptr = src->data;
    memcpy(dest_ptr, src_ptr, src->length * src->elem_size);

    dest->length += src->length;
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
    su_LOG_ASSERT_MESSAGE_M(dest, "dest is NULL");
    su_LOG_ASSERT_MESSAGE_M(src, "src is NULL");
    su_LOG_ASSERT_MESSAGE_M(dest->data, "dest->data is NULL");
    su_LOG_ASSERT_MESSAGE_M(src->data, "src->data is NULL");
    su_LOG_ASSERT_MESSAGE_M(dest->elem_size == src->elem_size, "dest->elem_size is not src->elem_size");
    su_LOG_ASSERT_MESSAGE_M(dest->elem_size > 0, "dest->elem_size is less than or equal to 0");
    su_LOG_ASSERT_MESSAGE_M(src->elem_size > 0, "dest->elem_size is less than or equal to 0");
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
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_MEMORY,
                              "Cannot append to fixed-size su_DArray");
        return su_FALSE;
    }

    su_U64 new_capacity = dest->capacity ? dest->capacity : 1;
    while (new_capacity < required_capacity) {
        new_capacity *= 2;
    }

    return su_darray_resize(dest, new_capacity);
}
