#ifndef SACI_UTILS_SU_DARRAY_H
#define SACI_UTILS_SU_DARRAY_H

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

#include <stdlib.h>

typedef struct sa_u8Array {
    sa_u8* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_u8Array;

static inline void sa_U8_Array_Init(sa_u8Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_u8));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_U8_Array");
    }
}

static inline void sa_U8_Array_Free(sa_u8Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_U8_Array_Resize(sa_u8Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_U8_Array");
        return sa_FALSE;
    }
    sa_u8* new_data = realloc(array->data, new_cap * sizeof(sa_u8));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_U8_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_U8_Array_Push(sa_u8Array* array, sa_u8 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_U8_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_U8_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_U8_Array_Pop(sa_u8Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_u8 sa_U8_Array_Get(sa_u8Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_U8_Array_Set(sa_u8Array* array, sa_u64 index, sa_u8 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_u16Array {
    sa_u16* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_u16Array;

static inline void sa_U16_Array_Init(sa_u16Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_u16));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_U16_Array");
    }
}

static inline void sa_U16_Array_Free(sa_u16Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_U16_Array_Resize(sa_u16Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_U16_Array");
        return sa_FALSE;
    }
    sa_u16* new_data = realloc(array->data, new_cap * sizeof(sa_u16));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_U16_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_U16_Array_Push(sa_u16Array* array, sa_u16 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_U16_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_U16_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_U16_Array_Pop(sa_u16Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_u16 sa_U16_Array_Get(sa_u16Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_U16_Array_Set(sa_u16Array* array, sa_u64 index, sa_u16 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_u32Array {
    sa_u32* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_u32Array;

static inline void sa_U32_Array_Init(sa_u32Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_u32));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_U32_Array");
    }
}

static inline void sa_U32_Array_Free(sa_u32Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_U32_Array_Resize(sa_u32Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_U32_Array");
        return sa_FALSE;
    }
    sa_u32* new_data = realloc(array->data, new_cap * sizeof(sa_u32));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_U32_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_U32_Array_Push(sa_u32Array* array, sa_u32 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_U32_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_U32_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_U32_Array_Pop(sa_u32Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_u32 sa_U32_Array_Get(sa_u32Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_U32_Array_Set(sa_u32Array* array, sa_u64 index, sa_u32 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_u64Array {
    sa_u64* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_u64Array;

static inline void sa_U64_Array_Init(sa_u64Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_u64));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_U64_Array");
    }
}

static inline void sa_U64_Array_Free(sa_u64Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_U64_Array_Resize(sa_u64Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_U64_Array");
        return sa_FALSE;
    }
    sa_u64* new_data = realloc(array->data, new_cap * sizeof(sa_u64));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_U64_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_U64_Array_Push(sa_u64Array* array, sa_u64 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_U64_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_U64_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_U64_Array_Pop(sa_u64Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_u64 sa_U64_Array_Get(sa_u64Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_U64_Array_Set(sa_u64Array* array, sa_u64 index, sa_u64 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_s8Array {
    sa_s8* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_s8Array;

static inline void sa_S8_Array_Init(sa_s8Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_s8));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_S8_Array");
    }
}

static inline void sa_S8_Array_Free(sa_s8Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_S8_Array_Resize(sa_s8Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_S8_Array");
        return sa_FALSE;
    }
    sa_s8* new_data = realloc(array->data, new_cap * sizeof(sa_s8));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_S8_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_S8_Array_Push(sa_s8Array* array, sa_s8 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_S8_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_S8_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_S8_Array_Pop(sa_s8Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_s8 sa_S8_Array_Get(sa_s8Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_S8_Array_Set(sa_s8Array* array, sa_u64 index, sa_s8 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_s16Array {
    sa_s16* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_s16Array;

static inline void sa_S16_Array_Init(sa_s16Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_s16));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_S16_Array");
    }
}

static inline void sa_S16_Array_Free(sa_s16Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_S16_Array_Resize(sa_s16Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_S16_Array");
        return sa_FALSE;
    }
    sa_s16* new_data = realloc(array->data, new_cap * sizeof(sa_s16));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_S16_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_S16_Array_Push(sa_s16Array* array, sa_s16 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_S16_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_S16_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_S16_Array_Pop(sa_s16Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_s16 sa_S16_Array_Get(sa_s16Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_S16_Array_Set(sa_s16Array* array, sa_u64 index, sa_s16 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_s32Array {
    sa_s32* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_s32Array;

static inline void sa_S32_Array_Init(sa_s32Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_s32));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_S32_Array");
    }
}

static inline void sa_S32_Array_Free(sa_s32Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_S32_Array_Resize(sa_s32Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_S32_Array");
        return sa_FALSE;
    }
    sa_s32* new_data = realloc(array->data, new_cap * sizeof(sa_s32));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_S32_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_S32_Array_Push(sa_s32Array* array, sa_s32 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_S32_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_S32_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_S32_Array_Pop(sa_s32Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_s32 sa_S32_Array_Get(sa_s32Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_S32_Array_Set(sa_s32Array* array, sa_u64 index, sa_s32 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_s64Array {
    sa_s64* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_s64Array;

static inline void sa_S64_Array_Init(sa_s64Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_s64));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_S64_Array");
    }
}

static inline void sa_S64_Array_Free(sa_s64Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_S64_Array_Resize(sa_s64Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_S64_Array");
        return sa_FALSE;
    }
    sa_s64* new_data = realloc(array->data, new_cap * sizeof(sa_s64));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_S64_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_S64_Array_Push(sa_s64Array* array, sa_s64 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_S64_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_S64_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_S64_Array_Pop(sa_s64Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_s64 sa_S64_Array_Get(sa_s64Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_S64_Array_Set(sa_s64Array* array, sa_u64 index, sa_s64 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_vec2Array {
    sa_vec2* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_vec2Array;

static inline void sa_Vec2_Array_Init(sa_vec2Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_vec2));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_Vec2_Array");
    }
}

static inline void sa_Vec2_Array_Free(sa_vec2Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_Vec2_Array_Resize(sa_vec2Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_Vec2_Array");
        return sa_FALSE;
    }
    sa_vec2* new_data = realloc(array->data, new_cap * sizeof(sa_vec2));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_Vec2_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_Vec2_Array_Push(sa_vec2Array* array, sa_vec2 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_Vec2_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_Vec2_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_Vec2_Array_Pop(sa_vec2Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_vec2 sa_Vec2_Array_Get(sa_vec2Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_Vec2_Array_Set(sa_vec2Array* array, sa_u64 index, sa_vec2 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_vec3Array {
    sa_vec3* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_vec3Array;

static inline void sa_Vec3_Array_Init(sa_vec3Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_vec3));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_Vec3_Array");
    }
}

static inline void sa_Vec3_Array_Free(sa_vec3Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_Vec3_Array_Resize(sa_vec3Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_Vec3_Array");
        return sa_FALSE;
    }
    sa_vec3* new_data = realloc(array->data, new_cap * sizeof(sa_vec3));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_Vec3_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_Vec3_Array_Push(sa_vec3Array* array, sa_vec3 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_Vec3_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_Vec3_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_Vec3_Array_Pop(sa_vec3Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_vec3 sa_Vec3_Array_Get(sa_vec3Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_Vec3_Array_Set(sa_vec3Array* array, sa_u64 index, sa_vec3 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_vec4Array {
    sa_vec4* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_vec4Array;

static inline void sa_Vec4_Array_Init(sa_vec4Array* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_vec4));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_Vec4_Array");
    }
}

static inline void sa_Vec4_Array_Free(sa_vec4Array* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_Vec4_Array_Resize(sa_vec4Array* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_Vec4_Array");
        return sa_FALSE;
    }
    sa_vec4* new_data = realloc(array->data, new_cap * sizeof(sa_vec4));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_Vec4_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_Vec4_Array_Push(sa_vec4Array* array, sa_vec4 value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_Vec4_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_Vec4_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_Vec4_Array_Pop(sa_vec4Array* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_vec4 sa_Vec4_Array_Get(sa_vec4Array* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_Vec4_Array_Set(sa_vec4Array* array, sa_u64 index, sa_vec4 val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

typedef struct sa_colorArray {
    sa_color* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} sa_colorArray;

static inline void sa_Color_Array_Init(sa_colorArray* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(sa_color));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array sa_Color_Array");
    }
}

static inline void sa_Color_Array_Free(sa_colorArray* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool sa_Color_Array_Resize(sa_colorArray* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array sa_Color_Array");
        return sa_FALSE;
    }
    sa_color* new_data = realloc(array->data, new_cap * sizeof(sa_color));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array sa_Color_Array, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool sa_Color_Array_Push(sa_colorArray* array, sa_color value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array sa_Color_Array");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!sa_Color_Array_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void sa_Color_Array_Pop(sa_colorArray* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline sa_color sa_Color_Array_Get(sa_colorArray* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void sa_Color_Array_Set(sa_colorArray* array, sa_u64 index, sa_color val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

#endif // SACI_UTILS_SU_DARRAY_H
