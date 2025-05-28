#include "saci-utils/su-general.h"
#include <saci-utils/su-debug.h>
#include <string.h>

sa_u64 sa_Size_Of_Type(sa_dataType data_type) {
    switch (data_type) {
    case SA_TYPE_U8:
    case SA_TYPE_S8:
    case SA_TYPE_BOOL:
        return 1;

    case SA_TYPE_U16:
    case SA_TYPE_S16:
        return 2;

    case SA_TYPE_U32:
    case SA_TYPE_S32:
    case SA_TYPE_SHADERID:
    case SA_TYPE_TEXTUREID:
    case SA_TYPE_BUFFERID:
        return 4;

    case SA_TYPE_U64:
    case SA_TYPE_S64:
    case SA_TYPE_UV:
    case SA_TYPE_MAT2:
    case SA_TYPE_VEC2:
        return 8;

    case SA_TYPE_VEC4:
    case SA_TYPE_COLOR:
    case SA_TYPE_MAT4:
        return 16;

    case SA_TYPE_VEC3:
    case SA_TYPE_MAT3:
    case SA_TYPE_MAT2X3:
    case SA_TYPE_MAT3X2:
        return 12;

    case SA_TYPE_MAT2X4:
    case SA_TYPE_MAT3X4:
    case SA_TYPE_MAT4X2:
    case SA_TYPE_MAT4X3:
        return 16;

    default:
        return 0;
    }
}

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

SA_API void sa_DArray_Free(sa_dArray* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
    array->elem_size = 0;
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

/* === DArray impl === */

SA_INTERNAL sa_bool sa_DArray_Can_Append(const sa_dArray* dest, const sa_dArray* src) {
    if (!dest || !src) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY,
                              "sa_DArray_Append received NULL pointer");
        return sa_FALSE;
    }

    if (dest->elem_size != src->elem_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY,
                              "Element sizes do not match: %lu != %lu",
                              dest->elem_size, src->elem_size);
        return sa_FALSE;
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
