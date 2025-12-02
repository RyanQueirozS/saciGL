#include "saci-utils/memory/su-string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
            char* new_data = malloc(new_cap);
            if (!new_data) {
                return su_FALSE;
            }
            memcpy(new_data, str->data, str->length + 1);
            free(str->data);
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
    free(str->data);
    free(str);
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
            char* new_data = malloc(str->length + 1);
            if (!new_data)
                return su_FALSE;
            memcpy(new_data, str->data, str->length + 1);
            free(str->data);
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

    su_String* str = (su_String*)malloc(sizeof(su_String));
    if (!str) {
        return NULL;
    }

    str->capacity = capacity;
    str->length = strlen(data);
    if (str->length + 1 > str->capacity) {
        str->capacity = str->length + 1;
    }

    str->data = (char*)malloc(str->capacity);
    if (!str->data) {
        free(str);
        return NULL;
    }

    memcpy(str->data, data, str->length + 1);
    str->realloc_kind = kind;
    str->ctx = ctx;
    str->ctx_size = ctx_size;

    return str;
}
