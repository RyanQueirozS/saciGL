#include "./sb-memmanager.h"

#include "saci-utils/su-log.h"
#include "saci-utils/su-general.h"

#include <arena/arena.h>
#include <saci-utils/su-types.h>
#include <stdio.h>
#include <string.h>

/* === Internal === */

su_Bool sb__mem_safe_copy(void* dest_ptr, size_t dest_capacity, size_t dest_offset,
                          const void* src_ptr, size_t src_size, size_t src_offset,
                          size_t copy_length);

su_Bool sb__mem_alloc_arena(void* pool, const su_U64 capacity, const su_U64 size, void** mem_out);

su_Bool sb__mem_malloc(su_U64* capacity_out, const su_U64 size, void** mem_out);

/* === Header impl === */

struct sb_MemChunk {
    enum sb_MemContext ctx;
    su_U64 capacity_bytes;
    su_U64 used_bytes;
    void* data;
};

struct {
    su_U64 size_now;
    su_U64 capacity;

    void* pool; // Will either be a continuously reallocated chunk or an
                // pool depending on build type
} sb__mem_manager[sb_MEM_CONTEXT_COUNT] = {0};

struct {
    su_Bool is_arena_based;
    su_Bool is_initialized;
} sb__mem_manager_cfg = {
    .is_arena_based = su_FALSE,
    .is_initialized = su_FALSE,
};

void sb_mem_init(const su_Bool use_arenas) {
    if (sb__mem_manager_cfg.is_initialized) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Trying to reinitialize memory allocator");
        return;
    }
    sb__mem_manager_cfg.is_arena_based = use_arenas;
}

struct sb_MemChunk* sb_mem_alloc(const enum sb_MemContext ctx,
                                 const su_U64 count,
                                 const su_U64 element_size) {
    void* memctx = NULL;
    if (element_size <= sizeof(struct sb_MemChunk)) {
        su_LOG_ERRORF_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Allocating less then %lu bytes (size of MemChunk)", sizeof(struct sb_MemChunk));
        return NULL;
    }
    if (element_size != 0 && count > UINT64_MAX / element_size) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Overflow in allocation size");
        return NULL;
    }
    su_U64 total_size = count * element_size;
    if (sb__mem_manager_cfg.is_arena_based) {
        sb__mem_alloc_arena(
            sb__mem_manager[ctx].pool,
            sb__mem_manager[ctx].capacity,
            total_size, &memctx);
    } else {
        sb__mem_malloc(&sb__mem_manager[ctx].capacity, total_size, &memctx);
    }
    if (!memctx) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Memory context could not be created");
        return NULL;
    }
    struct sb_MemChunk* chunk = (struct sb_MemChunk*)(memctx);
    chunk->ctx = ctx;
    chunk->capacity_bytes = total_size;
    chunk->used_bytes = 0;
    chunk->data = (void*)((char*)memctx + sizeof(struct sb_MemChunk));
    return chunk;
}

su_Bool sb_mem_chunk_push(struct sb_MemChunk* dest, const struct sb_MemChunk* src) {
    return sb__mem_safe_copy(dest->data, dest->capacity_bytes, dest->used_bytes, src->data, src->capacity_bytes, 0, src->used_bytes);
}

su_Bool sb_mem_chunk_push_data(struct sb_MemChunk* dest, const void* src, const su_U64 src_size) {
    return sb__mem_safe_copy(dest->data, dest->capacity_bytes, dest->used_bytes, src, src_size, 0, src_size);
}

void sb_mem_print_info(void) {
    su_U64 capacity_total = 0, size_total = 0;
    for (su_U64 i = 0; i < sb_MEM_CONTEXT_COUNT; ++i) {
        capacity_total += sb__mem_manager[i].capacity;
        size_total += sb__mem_manager[i].size_now;
    }
    if (sb__mem_manager_cfg.is_arena_based) {
        printf("Allocated %lu memory and used %lu", capacity_total, size_total);
        return;
    }

    printf("Allocated %lu bytes of memory. Could not calculate memory used: Not using "
           "pool based allocator",
           capacity_total);
}

/* === Internal Implementation === */

su_Bool sb__mem_safe_copy(void* dest_ptr, size_t dest_capacity, size_t dest_offset,
                          const void* src_ptr, size_t src_size, size_t src_offset,
                          size_t copy_length) {
    if (dest_ptr == NULL) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                        su_LOG_CONTEXT_CORE_MEMORY,
                        "Destination pointer is NULL in safe memcpy");
        return su_FALSE;
    }

    if (src_ptr == NULL) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                        su_LOG_CONTEXT_CORE_MEMORY,
                        "Source pointer is NULL in safe memcpy");
        return su_FALSE;
    }

    if (copy_length == 0) {
        su_LOG_WARNF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_LOW,
                       su_LOG_CONTEXT_CORE_MEMORY,
                       "Zero-length copy operation requested");
        return su_TRUE; // Zero-length copy is technically valid
    }

    if (dest_offset > dest_capacity) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                        su_LOG_CONTEXT_CORE_MEMORY,
                        "Destination offset exceeds capacity (offset: %zu, capacity: %zu)",
                        dest_offset, dest_capacity);
        return su_FALSE;
    }

    if (dest_offset + copy_length > dest_capacity) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                        su_LOG_CONTEXT_CORE_MEMORY,
                        "Copy would exceed destination capacity (offset: %zu, length: %zu, capacity: %zu)",
                        dest_offset, copy_length, dest_capacity);
        return su_FALSE;
    }

    if (src_offset > src_size) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                        su_LOG_CONTEXT_CORE_MEMORY,
                        "Source offset exceeds source size (offset: %zu, size: %zu)",
                        src_offset, src_size);
        return su_FALSE;
    }

    if (src_offset + copy_length > src_size) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                        su_LOG_CONTEXT_CORE_MEMORY,
                        "Copy would exceed source bounds (offset: %zu, length: %zu, size: %zu)",
                        src_offset, copy_length, src_size);
        return su_FALSE;
    }

    uint8_t* dest_start = (uint8_t*)dest_ptr + dest_offset;
    const uint8_t* src_start = (const uint8_t*)src_ptr + src_offset;

    if ((src_start < dest_start && src_start + copy_length > dest_start) ||
        (dest_start < src_start && dest_start + copy_length > src_start)) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
                        su_LOG_CONTEXT_CORE_MEMORY,
                        "Overlapping memory regions in safe memcpy (src: %p, dest: %p, length: %zu)",
                        (void*)src_start, (void*)dest_start, copy_length);
        return su_FALSE;
    }

    su_LOG_ASSERTF_M(dest_ptr != NULL, su_LOG_CONTEXT_CORE_MEMORY,
                     "Destination pointer assertion failed");
    su_LOG_ASSERTF_M(src_ptr != NULL, su_LOG_CONTEXT_CORE_MEMORY,
                     "Source pointer assertion failed");
    su_LOG_ASSERTF_M(dest_offset + copy_length <= dest_capacity,
                     su_LOG_CONTEXT_CORE_MEMORY,
                     "Destination bounds assertion failed");
    su_LOG_ASSERTF_M(src_offset + copy_length <= src_size,
                     su_LOG_CONTEXT_CORE_MEMORY,
                     "Source bounds assertion failed");

    memcpy(dest_start, src_start, copy_length);

    if (copy_length > 0) {
        su_LOG_DUMMY_CHECKF_M(*src_start == *dest_start,
                              su_LOG_CONTEXT_CORE_MEMORY,
                              "First byte verification in safe memcpy");

        if (copy_length > 1) {
            su_LOG_DUMMY_CHECKF_M(*(src_start + copy_length - 1) == *(dest_start + copy_length - 1),
                                  su_LOG_CONTEXT_CORE_MEMORY,
                                  "Last byte verification in safe memcpy");
        }
    }

    su_LOG_INFOF_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_CORE_MEMORY,
                   "Safe memcpy completed successfully (dest: %p+%zu, src: %p+%zu, length: %zu)",
                   dest_ptr, dest_offset, src_ptr, src_offset, copy_length);

    return su_TRUE;
}

// This function expects pool and mem_out to be initialized,
// it is not it's responsability to check it.
su_Bool sb__mem_alloc_arena(void* pool, const su_U64 capacity, const su_U64 size, void** mem_out) {
    if (capacity < size) {
        su_LOG_ERRORF_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Could not allocate memory: capacity is %lu and asking for %lu",
            capacity, size);
        return su_FALSE;
    }
    *mem_out = ArenaPush(pool, size);
    return (*mem_out) ? su_TRUE : su_FALSE;
}

su_Bool sb__mem_malloc(su_U64* capacity_out, const su_U64 size, void** mem_out) {
    if (!size) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Could not allocate memory: asking for 0 bytes");
    }
    *mem_out = su_MALLOC_M(size);
    if (!*mem_out) {
        return su_FALSE;
    }
    *capacity_out += size;
    return su_TRUE;
}
