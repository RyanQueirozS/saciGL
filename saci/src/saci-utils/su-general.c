#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

#include <string.h>

/* === DArray === */

SA_INTERNAL su_bool su_DArray_Can_Append(const su_dArray* dest, const su_dArray* src);

SA_INTERNAL su_bool su_DArray_Ensure_Capacity(su_dArray* dest, su_u64 required_capacity);

typedef struct su_dArray {
    void* data;
    su_u64 length;
    su_u64 capacity;
    su_u64 elem_size;
    su_bool is_fixed_size;
} su_dArray;

SA_API su_dArray* su_DArray_Create(su_u64 capacity, su_u64 elem_size, su_bool fixed_size) {
    su_dArray* array = su_Calloc_m(1, sizeof(su_dArray));
    su_Log_Assert_Message_m(array, "dArray couldn't be created");
    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;
    array->data = su_Calloc_m(capacity, elem_size);
    su_Log_Assert_Message_m(array->data, "Could not allocate memory for su_dArray");
    return array;
}

SA_API su_dArray* su_DArray_Create_Ctx(void* memctx, su_u64 memctx_size, su_u64 capacity, su_u64 elem_size, su_bool fixed_size) {
    su_u64 struct_size = sizeof(su_dArray);
    su_u64 data_size = capacity * elem_size;
    su_u64 total_size = struct_size + data_size;

    su_Log_AssertF_Message_m(memctx && memctx_size >= total_size,
                             "Memory context is too small for su_dArray and its "
                             "data. Passed: (%lu) expected min: (%lu)",
                             memctx_size, total_size);

    su_dArray* array = (su_dArray*)memctx;

    array->length = 0;
    array->capacity = capacity;
    array->elem_size = elem_size;
    array->is_fixed_size = fixed_size;

    array->data = (void*)((char*)memctx + struct_size);

    return array;
}

SA_API void su_DArray_Free(su_dArray* array) {
    su_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
    array->elem_size = 0;
}

SA_API void su_DArray_Clear(su_dArray* array) {
    su_Log_Assert_Message_m(array, "su_DArray_Clear: array is NULL");
    array->length = 0;
}

SA_API su_bool su_DArray_Resize(su_dArray* array, su_u64 new_cap) {
    if (array->is_fixed_size) {
        su_Log_ErrorF_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_MEMORY,
                              "Cannot resize fixed-size su_dArray");
        return su_FALSE;
    }
    void* new_data = realloc(array->data, new_cap * array->elem_size);
    if (!new_data) {
        su_Log_ErrorF_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_MEMORY,
                              "Could not allocate for su_dArray when resizing");
        return su_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return su_TRUE;
}

SA_API su_bool su_DArray_Push(su_dArray* array, const void* value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            su_Log_ErrorF_Print_m(su_LOG_SEVERITY_MEDIUM, su_LOG_CONTEXT_MEMORY,
                                  "Cannot push to full fixed-size su_dArray");
            return su_FALSE;
        }
        su_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!su_DArray_Resize(array, new_cap)) {
            return su_FALSE;
        }
    }
    void* dest = (char*)array->data + array->length * array->elem_size;
    memcpy(dest, value, array->elem_size);
    array->length++;
    return su_TRUE;
}

SA_API void su_DArray_Pop(su_dArray* array) {
    su_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop su_dArray");
    array->length--;
}

SA_API void su_DArray_Get(const su_dArray* array, su_u64 index, void* out_value) {
    su_Log_AssertF_Message_m(index < array->length,
                             "su_dArray accessed at %lu while length is %lu", index, array->length);
    const void* src = (const char*)array->data + index * array->elem_size;
    memcpy(out_value, src, array->elem_size);
}

SA_API void* su_DArray_Get_Ptr(const su_dArray* array, su_u64 index) {
    su_Log_AssertF_Message_m(index < array->length,
                             "su_dArray accessed at %lu while length is %lu", index, array->length);
    return (char*)array->data + index * array->elem_size;
}

SA_API void su_DArray_Set(su_dArray* array, su_u64 index, const void* value) {
    su_Log_AssertF_Message_m(index < array->length,
                             "su_dArray accessed at %lu while length is %lu", index, array->length);
    void* dest = (char*)array->data + index * array->elem_size;
    memcpy(dest, value, array->elem_size);
}

SA_API su_u64 su_DArray_Length(const su_dArray* array) {
    return array->length;
}

SA_API su_u64 su_DArray_Capacity(const su_dArray* array) {
    return array->capacity;
}

SA_API void su_DArray_Append(su_dArray* dest, const su_dArray* src) {
    if (!su_DArray_Can_Append(dest, src)) {
        return;
    }

    su_u64 required_capacity = dest->length + src->length;
    if (!su_DArray_Ensure_Capacity(dest, required_capacity)) {
        return;
    }

    void* dest_ptr = (char*)dest->data + (dest->length * dest->elem_size);
    const void* src_ptr = src->data;
    memcpy(dest_ptr, src_ptr, src->length * src->elem_size);

    dest->length += src->length;
}

void su_DArray_Debug_Print(const su_dArray* arr) {
    printf("su_dArray Debug: data=%p, length=%lu, capacity=%lu, elem_size=%lu, fixed=%d\n",
           arr->data, arr->length, arr->capacity, arr->elem_size, arr->is_fixed_size);
}

SA_API su_bool su_DArray_Is_Null(const su_dArray* arr) {
    return arr->data == NULL;
}

SA_API su_bool su_DArray_Is_Empty(const su_dArray* arr) {
    return arr->length == 0;
}

/* === DArray impl === */

SA_INTERNAL su_bool su_DArray_Can_Append(const su_dArray* dest, const su_dArray* src) {
    su_Log_Assert_Message_m(dest, "dest is NULL");
    su_Log_Assert_Message_m(src, "src is NULL");
    su_Log_Assert_Message_m(dest->data != NULL, "dest->data is NULL");
    su_Log_Assert_Message_m(src->data != NULL, "src->data is NULL");
    su_Log_Assert_Message_m(dest->elem_size == src->elem_size, "dest->elem_size is not src->elem_size");
    su_Log_Assert_Message_m(dest->elem_size > 0, "dest->elem_size is less than or equal to 0");
    su_Log_Assert_Message_m(src->elem_size > 0, "dest->elem_size is less than or equal to 0");
    if (src->length == 0) {
        return false;
    }

    return su_TRUE;
}

SA_INTERNAL su_bool su_DArray_Ensure_Capacity(su_dArray* dest, su_u64 required_capacity) {
    if (required_capacity <= dest->capacity) {
        return su_TRUE;
    }

    if (dest->is_fixed_size) {
        su_Log_ErrorF_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_MEMORY,
                              "Cannot append to fixed-size su_dArray");
        return su_FALSE;
    }

    su_u64 new_capacity = dest->capacity ? dest->capacity : 1;
    while (new_capacity < required_capacity) {
        new_capacity *= 2;
    }

    return su_DArray_Resize(dest, new_capacity);
}
