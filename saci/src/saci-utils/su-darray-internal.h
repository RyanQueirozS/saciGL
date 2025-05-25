#ifndef SACI_UTILS_SU_DARRAY_INTERNAL_H
#define SACI_UTILS_SU_DARRAY_INTERNAL_H

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

#define sa_Create_Darray_m(name, type)                                                      \
    typedef struct name {                                                                   \
        type* data;                                                                         \
        sa_u64 length;                                                                      \
        sa_u64 capacity;                                                                    \
        sa_bool is_fixed_size;                                                              \
    } type##Array;                                                                          \
                                                                                            \
    static inline void name##_Init(type##Array* array, sa_u64 capacity,                     \
                                   sa_bool fixed_size) {                                    \
        array->length = 0;                                                                  \
        array->capacity = capacity;                                                         \
        array->is_fixed_size = fixed_size;                                                  \
        array->data = sa_Calloc_m(capacity, sizeof(type));                                  \
        if (!array->data) {                                                                 \
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,   \
                                  "Could not allocate memory for array %s", #name);         \
        }                                                                                   \
    }                                                                                       \
                                                                                            \
    static inline void name##_Free(type##Array* array) {                                    \
        sa_Free_m(array->data);                                                             \
        array->data = NULL;                                                                 \
        array->length = 0;                                                                  \
        array->capacity = 0;                                                                \
    }                                                                                       \
                                                                                            \
    static inline sa_bool name##_Resize(type##Array* array, sa_u64 new_cap) {               \
        if (array->is_fixed_size) {                                                         \
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH,                                     \
                                  sa_LOG_CONTEXT_MEMORY_ALLOCATION,                         \
                                  "Cannot resize fixed-size array %s", #name);              \
            return sa_FALSE;                                                                \
        }                                                                                   \
        type* new_data = realloc(array->data, new_cap * sizeof(type));                      \
        if (!new_data) {                                                                    \
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH,                                     \
                                  sa_LOG_CONTEXT_MEMORY_ALLOCATION,                         \
                                  "Could not allocate for array %s, when resizing", #name); \
            return sa_FALSE;                                                                \
        }                                                                                   \
        array->data = new_data;                                                             \
        array->capacity = new_cap;                                                          \
        return sa_TRUE;                                                                     \
    }                                                                                       \
                                                                                            \
    static inline sa_bool name##_Push(type##Array* array, type value) {                     \
        if (array->length == array->capacity) {                                             \
            if (array->is_fixed_size) {                                                     \
                sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM,                               \
                                      sa_LOG_CONTEXT_MEMORY_ALLOCATION,                     \
                                      "Cannot push to full fixed-size array %s", #name);    \
                return sa_FALSE;                                                            \
            }                                                                               \
            sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;                     \
            if (!name##_Resize(array, new_cap)) {                                           \
                return sa_FALSE;                                                            \
            }                                                                               \
        }                                                                                   \
        array->data[array->length++] = value;                                               \
        return sa_TRUE;                                                                     \
    }                                                                                       \
                                                                                            \
    static inline void name##_Pop(type##Array* array) {                                     \
        sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");      \
        array->length--;                                                                    \
    }                                                                                       \
                                                                                            \
    static inline type name##_Get(const type##Array* array, sa_u64 index) {                 \
        sa_Log_AssertF_Message_m(index < array->length,                                     \
                                 "DArray accessed at %lu while length is %lu",              \
                                 index, array->length);                                     \
        return array->data[index];                                                          \
    }                                                                                       \
                                                                                            \
    static inline void name##_Set(type##Array* array, sa_u64 index, type val) {             \
        sa_Log_AssertF_Message_m(index < array->length,                                     \
                                 "DArray accessed at %lu while length is %lu",              \
                                 index, array->length);                                     \
        array->data[index] = val;                                                           \
    }; // NOLINT

#endif
