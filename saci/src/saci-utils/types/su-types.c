#include "saci-utils/su-types.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-log.h"

#include <stdarg.h>
#include <stdio.h>
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

    su_DArray* array = su_CALLOC_M(1, sizeof(su_DArray));
    su_LOG_ASSERT_M(array, su_LOG_CONTEXT_CORE_DARRAY, "Could not create dynamic array");

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = su_CALLOC_M(capacity, elem_size);
    su_LOG_ASSERT_M(array->data, su_LOG_CONTEXT_CORE_DARRAY, "Could not allocate memory for su_DArray");
    return array;
}

su_DArray* su_darray_create_ctx(void* memctx, su_U64 memctx_size, su_U64 capacity, su_U64 elem_size, su_Bool fixed_size) {
    su_U64 struct_size = sizeof(su_DArray);
    su_U64 data_size = capacity * elem_size;
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

void su_darray_free(su_DArray* array) {
    su_FREE_M(array->data);
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

void su_darray_set(su_DArray* array, su_U64 index, const void* value) {
    su_LOG_DUMMY_CHECK_M(array, su_LOG_CONTEXT_CORE_DARRAY, "DArray is NULL");
    su_LOG_DUMMY_CHECK_M(array->data, su_LOG_CONTEXT_CORE_DARRAY, "DArray data is NULL");
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

/* === String === */

// Helper
SA_INTERNAL su_String* su__string_alloc_init(
    const char* data,
    su_U64 capacity,
    su_ReallocationKind kind,
    void* ctx,
    su_U64 ctx_size);

// Impl

typedef struct su_String {
    char* data;
    su_U64 length;
    su_U64 capacity;
    su_ReallocationKind realloc_kind;
    void* ctx;       // optional allocator context
    su_U64 ctx_size; // size of allocator context
} su_String;

su_String* su_string_create(const char* data, su_ReallocationKind realloc_kind) {
    return su__string_alloc_init(data, 16, realloc_kind, NULL, 0);
}

su_String* su_string_create_ctx(const char* data, su_ReallocationKind realloc_kind, void* ctx, su_U64 ctx_size) {
    return su__string_alloc_init(data, 16, realloc_kind, ctx, ctx_size);
}

su_String* su_string_create_cap(const char* data, su_U64 capacity, su_ReallocationKind realloc_kind) {
    return su__string_alloc_init(data, capacity, realloc_kind, NULL, 0);
}

const char* su_string_data(const su_String* str) {
    return str ? str->data : NULL;
}

su_U64 su_string_length(const su_String* str) {
    return str ? str->length : 0;
}

su_Bool su_string_reserve(su_String* str, su_U64 new_cap) {
    if (!str || new_cap <= str->capacity) {
        return su_FALSE;
    }

    switch (str->realloc_kind) {
    case su_REALLOCATION_KIND_REALLOC:
        {
            char* new_data = realloc(str->data, new_cap);
            if (!new_data) {
                return su_FALSE;
            }
            str->data = new_data;
            str->capacity = new_cap;
        }
        break;

    case su_REALLOCATION_KIND_MALLOC_FREE:
        {
            char* new_data = su_MALLOC_M(new_cap);
            if (!new_data) {
                return su_FALSE;
            }
            memcpy(new_data, str->data, str->length + 1);
            su_FREE_M(str->data);
            str->data = new_data;
            str->capacity = new_cap;
        }
        break;

    case su_REALLOCATION_KIND_FIXED_SIZE:
        return su_FALSE;
    }
    return su_TRUE;
}

void su_string_free(su_String* str) {
    if (!str) {
        return;
    }
    su_FREE_M(str->data);
    su_FREE_M(str);
}

su_Bool su_string_append(su_String* str, const char* suffix, su_U64 suffix_len) {
    if (!str || (!suffix && suffix_len > 0))
        return su_FALSE;

    su_U64 new_len = str->length + suffix_len;

    if (new_len + 1 > str->capacity) {
        if (!su_string_reserve(str, (new_len + 1) * 2)) {
            return su_FALSE;
        }
    }

    if (suffix_len > 0) {
        memcpy(str->data + str->length, suffix, suffix_len);
    }
    str->length = new_len;
    str->data[str->length] = '\0';

    return su_TRUE;
}

su_Bool su_string_append_str(su_String* dest, const su_String* src) {
    if (!dest || !src)
        return su_FALSE;
    return su_string_append(dest, src->data, src->length);
}

su_Bool su_string_set(su_String* str, const char* new_data, su_U64 new_len) {
    if (!str || (!new_data && new_len > 0))
        return su_FALSE;

    if (new_len + 1 > str->capacity) {
        if (!su_string_reserve(str, new_len + 1)) {
            return su_FALSE;
        }
    }

    if (new_len > 0) {
        memcpy(str->data, new_data, new_len);
    }
    str->length = new_len;
    str->data[new_len] = '\0';

    return su_TRUE;
}

su_Bool su_string_set_str(su_String* dest, const su_String* src) {
    if (!dest || !src)
        return su_FALSE;
    return su_string_set(dest, src->data, src->length);
}

void su_string_clear(su_String* str) {
    if (!str)
        return;
    str->length = 0;
    if (str->data)
        str->data[0] = '\0';
}

su_Bool su_string_shrink_to_fit(su_String* str) {
    if (!str || str->capacity == str->length + 1)
        return su_FALSE;

    switch (str->realloc_kind) {
    case su_REALLOCATION_KIND_REALLOC:
        {
            char* new_data = realloc(str->data, str->length + 1);
            if (!new_data)
                return su_FALSE;
            str->data = new_data;
            str->capacity = str->length + 1;
        }
        break;

    case su_REALLOCATION_KIND_MALLOC_FREE:
        {
            char* new_data = su_MALLOC_M(str->length + 1);
            if (!new_data)
                return su_FALSE;
            memcpy(new_data, str->data, str->length + 1);
            su_FREE_M(str->data);
            str->data = new_data;
            str->capacity = str->length + 1;
        }
        break;

    case su_REALLOCATION_KIND_FIXED_SIZE:
        return su_FALSE;
    }
    return su_TRUE;
}

int su_string_compare(const su_String* a, const char* b, su_U64 b_len) {
    if (!a || (!b && b_len > 0))
        return (a == NULL && b == NULL) ? 0 : (a ? 1 : -1);

    su_U64 min_len = (a->length < b_len) ? a->length : b_len;
    int cmp = memcmp(a->data, b, min_len);
    if (cmp != 0)
        return cmp;

    if (a->length < b_len)
        return -1;
    if (a->length > b_len)
        return 1;
    return 0;
}

int su_string_compare_str(const su_String* a, const su_String* b) {
    if (!a || !b)
        return (a == NULL && b == NULL) ? 0 : (a ? 1 : -1);
    return su_string_compare(a, b->data, b->length);
}

su_Bool su_string_format(su_String* str, const char* fmt, ...) {
    if (!str || !fmt)
        return su_FALSE;

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        va_end(args);
        return su_FALSE;
    }

    if ((su_U64)needed + 1 > str->capacity) {
        if (!su_string_reserve(str, (su_U64)needed + 1)) {
            va_end(args);
            return su_FALSE;
        }
    }

    vsnprintf(str->data, str->capacity, fmt, args);
    str->length = (su_U64)needed;

    va_end(args);
    return su_TRUE;
}

su_Bool su_string_append_format(su_String* str, const char* fmt, ...) {
    if (!str || !fmt)
        return su_FALSE;

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        va_end(args);
        return su_FALSE;
    }

    su_U64 new_len = str->length + (su_U64)needed;
    if (new_len + 1 > str->capacity) {
        if (!su_string_reserve(str, (new_len + 1) * 2)) {
            va_end(args);
            return su_FALSE;
        }
    }

    vsnprintf(str->data + str->length, str->capacity - str->length, fmt, args);
    str->length = new_len;

    va_end(args);
    return su_TRUE;
}

su_Bool su_string_is_empty(su_String* str) {
    if (!str->data || !str->length) {
        return su_TRUE;
    }
    return su_FALSE;
}

// Helper
SA_INTERNAL su_String* su__string_alloc_init(
    const char* data,
    su_U64 capacity,
    su_ReallocationKind kind,
    void* ctx,
    su_U64 ctx_size) {
    if (!data) {
        data = "";
    }

    su_String* str = (su_String*)su_MALLOC_M(sizeof(su_String));
    if (!str) {
        return NULL;
    }

    str->capacity = capacity;
    str->length = strlen(data);
    if (str->length + 1 > str->capacity) {
        str->capacity = str->length + 1;
    }

    str->data = (char*)su_MALLOC_M(str->capacity);
    if (!str->data) {
        su_FREE_M(str);
        return NULL;
    }

    memcpy(str->data, data, str->length + 1);
    str->realloc_kind = kind;
    str->ctx = ctx;
    str->ctx_size = ctx_size;

    return str;
}
