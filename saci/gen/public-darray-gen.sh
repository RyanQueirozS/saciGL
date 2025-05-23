#!/bin/sh

OUTPUT_FILE="../include/saci-utils/su-darray.h"

HEADER_GUARD="SACI_UTILS_SU_DARRAY_H"

darrays='
sa_U8_Array    sa_u8
sa_U16_Array   sa_u16
sa_U32_Array   sa_u32
sa_U64_Array   sa_u64
sa_S8_Array    sa_s8
sa_S16_Array   sa_s16
sa_S32_Array   sa_s32
sa_S64_Array   sa_s64
sa_Vec2_Array  sa_vec2
sa_Vec3_Array  sa_vec3
sa_Vec4_Array  sa_vec4
sa_Color_Array sa_color
'

mkdir -p "$(dirname "$OUTPUT_FILE")"

cat > "$OUTPUT_FILE" <<EOF
#ifndef ${HEADER_GUARD}
#define ${HEADER_GUARD}

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

#include <stdlib.h>

EOF

# Loop through array type definitions
echo "$darrays" | while read name type; do
  [ -z "$type" ] && continue

  struct_name="${type}Array"
  function_prefix="${name}"

  cat >> "$OUTPUT_FILE" <<EOF
typedef struct ${struct_name} {
    ${type}* data;
    sa_u64 length;
    sa_u64 capacity;
    sa_bool is_fixed_size;
} ${struct_name};

static inline void ${function_prefix}_Init(${struct_name}* array, sa_u64 capacity, sa_bool fixed_size) {
    array->length = 0;
    array->capacity = capacity;
    array->is_fixed_size = fixed_size;
    array->data = sa_Calloc_m(capacity, sizeof(${type}));
    if (!array->data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate memory for array ${function_prefix}");
    }
}

static inline void ${function_prefix}_Free(${struct_name}* array) {
    sa_Free_m(array->data);
    array->data = NULL;
    array->length = 0;
    array->capacity = 0;
}

static inline sa_bool ${function_prefix}_Resize(${struct_name}* array, sa_u64 new_cap) {
    if (array->is_fixed_size) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Cannot resize fixed-size array ${function_prefix}");
        return sa_FALSE;
    }
    ${type}* new_data = realloc(array->data, new_cap * sizeof(${type}));
    if (!new_data) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                              "Could not allocate for array ${function_prefix}, when resizing");
        return sa_FALSE;
    }
    array->data = new_data;
    array->capacity = new_cap;
    return sa_TRUE;
}

static inline sa_bool ${function_prefix}_Push(${struct_name}* array, ${type} value) {
    if (array->length == array->capacity) {
        if (array->is_fixed_size) {
            sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                                  "Cannot push to full fixed-size array ${function_prefix}");
            return sa_FALSE;
        }
        sa_u64 new_cap = array->capacity ? array->capacity * 2 : 4;
        if (!${function_prefix}_Resize(array, new_cap)) {
            return sa_FALSE;
        }
    }
    array->data[array->length++] = value;
    return sa_TRUE;
}

static inline void ${function_prefix}_Pop(${struct_name}* array) {
    sa_Log_Assert_Message_m(array->length > 0, "Length is zero cannot pop array");
    array->length--;
}

static inline ${type} ${function_prefix}_Get(${struct_name}* array, sa_u64 index) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    return array->data[index];
}

static inline void ${function_prefix}_Set(${struct_name}* array, sa_u64 index, ${type} val) {
    sa_Log_AssertF_Message_m(index < array->length,
                             "DArray accessed at %lu while length is %lu", index, array->length);
    array->data[index] = val;
}

EOF

done

cat >> "$OUTPUT_FILE" <<EOF
#endif // ${HEADER_GUARD}
EOF

