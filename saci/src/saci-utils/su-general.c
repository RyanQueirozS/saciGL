#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include <string.h>

/* === DArray === */

SA_INTERNAL sa_bool sa_DArray_Can_Append(const sa_dArray* dest, const sa_dArray* src);

SA_INTERNAL sa_bool sa_DArray_Ensure_Capacity(sa_dArray* dest, sa_u64 required_capacity);

typedef struct sa_dArray {
    void* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_u64 elem_size;
    sa_bool is_fixed_size;
} sa_dArray;

SA_API sa_dArray* sa_DArray_Create(sa_u64 capacity, sa_u64 elem_size, sa_bool fixed_size) {
    sa_dArray* array = sa_Calloc_m(1, sizeof(sa_dArray));
    sa_Log_Assert_Message_m(array, "dArray couldn't be created");
    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, elem_size);
    sa_Log_Assert_Message_m(array->data, "Could not allocate memory for sa_dArray");
    return array;
}

SA_API sa_dArray* sa_DArray_Create_Ctx(void* memctx, sa_u64 memctx_size, sa_u64 capacity, sa_u64 elem_size, sa_bool fixed_size) {
    sa_u64 struct_size = sizeof(sa_dArray);
    sa_u64 data_size = capacity * elem_size;
    sa_u64 total_size = struct_size + data_size;

    sa_Log_AssertF_Message_m(memctx && memctx_size >= total_size,
                             "Memory context is too small for sa_dArray and its "
                             "data. Passed: (%lu) expected min: (%lu)",
                             memctx_size, total_size);

    sa_dArray* array = (sa_dArray*)memctx;

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = (void*)((char*)memctx + struct_size);

    return array;
}

SA_API void sa_DArray_Free(sa_dArray* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
    array->elem_size = 0;
}

SA_API void sa_DArray_Clear(sa_dArray* array) {
    sa_Log_Assert_Message_m(array, "sa_DArray_Clear: array is NULL");
    array->length = 0;
}

SA_API sa_bool sa_DArray_Resize(sa_dArray* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY,
                              "Cannot resize fixed-size sa_dArray");
        return sa_FALSE;
    }
    void* new_data = realloc(array->data, new_cap * array->elem_size);
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY,
                              "Could not allocate for sa_dArray when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

SA_API sa_bool sa_DArray_Push(sa_dArray* array, const void* value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY,
                                  "Cannot push to full fixed-size sa_dArray");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_DArray_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    void* dest = (char*)array->data + array->length * array->elem_size;
    memcpy(dest, value, array->elem_size);
    array->length++;
    return sa_TRUE;
}

SA_API void sa_DArray_Pop(sa_dArray* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop sa_dArray");
    array->length--;
}

SA_API void sa_DArray_Get(const sa_dArray* array, sa_u64 index, void* out_value) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "sa_dArray accessed at %lu while length is %lu", index, array->length);
    const void* src = (const char*)array->data + index * array->elem_size;
    memcpy(out_value, src, array->elem_size);
}

SA_API void* sa_DArray_Get_Ptr(const sa_dArray* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "sa_dArray accessed at %lu while length is %lu", index, array->length);
    return (char*)array->data + index * array->elem_size;
}

SA_API void sa_DArray_Set(sa_dArray* array, sa_u64 index, const void* value) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "sa_dArray accessed at %lu while length is %lu", index, array->length);
    void* dest = (char*)array->data + index * array->elem_size;
    memcpy(dest, value, array->elem_size);
}

SA_API sa_u64 sa_DArray_Length(const sa_dArray* array) {
    return array->length;
}

SA_API sa_u64 sa_DArray_Capacity(const sa_dArray* array) {
    return array->capacity;
}

SA_API void sa_DArray_Append(sa_dArray* dest, const sa_dArray* src) {
    if (!sa_DArray_Can_Append(dest, src)) {
        return;
    }

    sa_u64 required_capacity = dest->length + src->length;
    if (!sa_DArray_Ensure_Capacity(dest, required_capacity)) {
        return;
    }

    void* dest_ptr = (char*)dest->data + (dest->length * dest->elem_size);
    const void* src_ptr = src->data;
    memcpy(dest_ptr, src_ptr, src->length * src->elem_size);

    dest->length += src->length;
}

void sa_DArray_Debug_Print(const sa_dArray* arr) {
    printf("sa_dArray Debug: data=%p, length=%lu, capacity=%lu, elem_size=%lu, fixed=%d\n",
           arr->data, arr->length, arr->capacity, arr->elem_size, arr->is_fixed_size);
}

/* === DArray impl === */

SA_INTERNAL sa_bool sa_DArray_Can_Append(const sa_dArray* dest, const sa_dArray* src) {
    sa_Log_Assert_Message_m(dest, "dest is NULL");
    sa_Log_Assert_Message_m(src, "src is NULL");
    sa_Log_Assert_Message_m(dest->data != NULL, "dest->data is NULL");
    sa_Log_Assert_Message_m(src->data != NULL, "src->data is NULL");
    sa_Log_Assert_Message_m(dest->elem_size == src->elem_size, "dest->elem_size is not src->elem_size");
    sa_Log_Assert_Message_m(dest->elem_size > 0, "dest->elem_size is less than or equal to 0");
    sa_Log_Assert_Message_m(src->elem_size > 0, "dest->elem_size is less than or equal to 0");
    if (src->length == 0) {
        return false;
    }

    return sa_TRUE;
}

SA_INTERNAL sa_bool sa_DArray_Ensure_Capacity(sa_dArray* dest, sa_u64 required_capacity) {
    if (required_capacity <= dest->capacity) {
        return sa_TRUE;
    }

    if (dest->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY,
                              "Cannot append to fixed-size sa_dArray");
        return sa_FALSE;
    }

    sa_u64 new_capacity = dest->capacity ? dest->capacity : 1;
    while (new_capacity < required_capacity) {
        new_capacity *= 2;
    }

    return sa_DArray_Resize(dest, new_capacity);
}
