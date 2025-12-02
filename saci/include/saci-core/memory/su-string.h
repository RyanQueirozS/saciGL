#ifndef SACI_UTILS_MEMORY_SU_STRING_H
#define SACI_UTILS_MEMORY_SU_STRING_H

#include "../su-general.h"
#include "../su-types-common.h"

typedef struct su_String su_String;

SA_API su_String* su_string_create(const char* data, su_ReallocationKind realloc_kind);

SA_API su_String* su_string_create_ctx(const char* data, su_ReallocationKind realloc_kind, void* ctx, su_U64 ctx_size);

SA_API su_String* su_string_create_cap(const char* data, su_U64 capacity, su_ReallocationKind realloc_kind);

SA_API const char* su_string_data(const su_String* str);

SA_API su_U64 su_string_length(const su_String* str);

SA_API su_Bool su_string_reserve(su_String* str, su_U64 new_cap);

SA_API void su_string_free(su_String* str);

su_Bool su_string_append(su_String* str, const char* suffix, su_U64 suffix_len);

su_Bool su_string_append_str(su_String* dest, const su_String* src);

su_Bool su_string_set(su_String* str, const char* new_data, su_U64 new_len);

su_Bool su_string_set_str(su_String* dest, const su_String* src);

void su_string_clear(su_String* str);

su_Bool su_string_shrink_to_fit(su_String* str);

int su_string_compare(const su_String* a, const char* b, su_U64 b_len);

int su_string_compare_str(const su_String* a, const su_String* b);

su_Bool su_string_format(su_String* str, const char* fmt, ...);

su_Bool su_string_append_format(su_String* str, const char* fmt, ...);

su_Bool su_string_is_empty(su_String* str);

#endif // SACI_UTILS_MEMORY_SU_STRING_H
