#include "saci-utils/memory/su-memory.h"
#include "saci-utils/su-types-common.h"

#include "saci-utils/su-log.h"
#include <stdlib.h>

#define ARENA_ASSERT(x) su_LOG_ASSERT_M(x, su_LOG_CONTEXT_CORE_MEMORY, "Error in arena function")
#define ARENA_FREE(x) free(x)
#define ARENA_MALLOC(x) malloc(x)
#define ARENA_ALLOCATOR_IMPL
#define ARENA_DEBUG
#include <arena/arena.h>
#include <stdio.h>
#include <string.h>

#define SU_MEM_KB (1024ULL)
#define SU_MEM_MB (1024ULL * SU_MEM_KB)
#define SU_MEM_GB (1024ULL * SU_MEM_MB)
#define SU_MEM_TB (1024ULL * SU_MEM_GB)

struct su_MemPool {
    Arena arena;
};

/* === Internal === */

su_Bool su__mem_alloc_arena(void* pool, const su_U64 capacity, const su_U64 size, void** mem_out);

su_Bool su__mem_malloc(su_U64* capacity_out, const su_U64 size, void** mem_out);

/* === Header impl === */

struct su_MemChunk {
    enum su_MemContext ctx;
    su_U64 element_count;
    su_U64 element_size_bytes;
    void* data;
};

struct {
    su_U64 size_now;
    su_U64 capacity;

    void* pool; // Will either be a continuously reallocated chunk or an
                // pool depending on build type
} su__mem_manager[su_MEM_CONTEXT_COUNT] = {0};

struct {
    su_Bool is_arena_based;
    su_Bool is_initialized;
    su_U64 default_sizes[su_MEM_CONTEXT_COUNT];
} su__mem_manager_cfg = {
    .is_arena_based = su_FALSE,
    .is_initialized = su_FALSE,
    .default_sizes = {
        2 * SU_MEM_KB,
        1 * SU_MEM_GB,
    },
};

void su_mem_init(const su_Bool use_arenas) {
    if (su__mem_manager_cfg.is_initialized) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Trying to reinitialize memory allocator");
        return;
    }
    su__mem_manager_cfg.is_arena_based = use_arenas;
    if (use_arenas) {
        for (su_U64 i = 0; i < su_MEM_CONTEXT_COUNT; ++i) {
            ArenaInit(su__mem_manager[i].pool, su__mem_manager_cfg.default_sizes[i]);
        }
    }
}

struct su_MemChunk* su_mem_alloc(const enum su_MemContext ctx,
                                 const su_U64 count,
                                 const su_U64 element_size) {
    void* memctx = NULL;
    if (element_size != 0 && count > UINT64_MAX / element_size) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Overflow in allocation size");
        return NULL;
    }
    su_U64 total_size = count * element_size + sizeof(struct su_MemChunk);
    if (su__mem_manager_cfg.is_arena_based) {
        su__mem_alloc_arena(
            su__mem_manager[ctx].pool,
            su__mem_manager[ctx].capacity,
            total_size, &memctx);
    } else {
        su__mem_malloc(&su__mem_manager[ctx].capacity, total_size, &memctx);
    }
    if (!memctx) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Memory context could not be created");
        return NULL;
    }
    // Allocates the memory for the chunk
    struct su_MemChunk* chunk = (struct su_MemChunk*)(memctx);
    chunk->ctx = ctx;
    chunk->element_count = count;
    chunk->element_size_bytes = element_size;
    // Data goes after the chunk structure
    chunk->data = (void*)((char*)memctx + sizeof(struct su_MemChunk));
    return chunk;
}

su_MemPool* su_mem_create_pool(const enum su_MemContext ctx,
                               const su_U64 size) {
    void* memctx = NULL;
    if (size <= 0) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Overflow in allocation size");
        return NULL;
    }
    su_U64 total_size = sizeof(su_MemPool) + size;

    // In this case will generate a partion out of the mem_manager's memory pool.
    if (su__mem_manager_cfg.is_arena_based) {
        su__mem_alloc_arena(
            su__mem_manager[ctx].pool,
            su__mem_manager[ctx].capacity,
            total_size, &memctx);
    } else {
        // And in this it will malloc the data and create an arena through it.
        su__mem_malloc(&su__mem_manager[ctx].capacity, total_size, &memctx);
    }
    if (!memctx) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_CRASH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Memory context could not be created");
        return NULL;
    }

    su_MemPool* pool = (su_MemPool*)memctx;
    ArenaInitCtx(&pool->arena, (void*)((char*)memctx + sizeof(su_MemPool)), size);

    return pool;
}

su_Bool su_mem_chunk_set(struct su_MemChunk* chunk, su_U64 idx, void* data, su_U64 data_size) {
    if (chunk->element_count < idx) {
        su_LOG_ERRORF_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_CORE_MEMORY,
            "Could not set data in chunk, idx is %lu and there are %lu elements",
            chunk->element_count, idx);
    }
    if (chunk->element_size_bytes != data_size) {
        su_LOG_ERRORF_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_CORE_MEMORY,
            "Could not set data in chunk, data (%lu) is bigger than element "
            "size (%lu) in chunk",
            chunk->element_size_bytes, idx);
    }
    const su_U64 dest_capacity = chunk->element_size_bytes * chunk->element_count;
    const su_U64 dest_offset = chunk->element_size_bytes * idx;
    // We use the chunk->element_size_bytes as the copy length to avoid overflow
    su_mem_safe_copy(chunk->data, dest_capacity, dest_offset, data, 0, data_size,
                     chunk->element_size_bytes);
    return su_TRUE;
}

const void* su_mem_chunk_get(struct su_MemChunk* chunk, su_U64 idx) {
    if (idx < chunk->element_count) {
        su_LOG_ERRORF_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_CORE_MEMORY,
            "Could not get data from chunk, idx is %lu and there are %lu elements",
            chunk->element_count, idx);
    }
    return (const void*)((char*)chunk->data + (idx * chunk->element_size_bytes));
}

void* su_mem_chunk_get_ptr(struct su_MemChunk* chunk, su_U64 idx) {
    if (idx < chunk->element_count) {
        su_LOG_ERRORF_M(
            su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_CORE_MEMORY,
            "Could not get data from chunk, idx is %lu and there are %lu elements",
            chunk->element_count, idx);
    }
    return (void*)((char*)chunk->data + (idx * chunk->element_size_bytes));
}

void su_mem_print_info(void) {
    su_U64 capacity_total = 0, size_total = 0;
    for (su_U64 i = 0; i < su_MEM_CONTEXT_COUNT; ++i) {
        capacity_total += su__mem_manager[i].capacity;
        size_total += su__mem_manager[i].size_now;
    }
    if (su__mem_manager_cfg.is_arena_based) {
        printf("Allocated %lu memory and used %lu", capacity_total, size_total);
        return;
    }

    printf("Allocated %lu bytes of memory. Could not calculate memory used: Not using "
           "pool based allocator",
           capacity_total);
}

enum su_MemContext su_mem_chunk_get_ctx(const struct su_MemChunk* chunk) {
    return chunk->ctx;
}

su_U64 su_mem_chunk_get_capacity(const struct su_MemChunk* chunk) {
    return chunk->element_size_bytes * chunk->element_count;
}

su_U64 su_mem_chunk_get_element_size(const struct su_MemChunk* chunk) {
    return chunk->element_size_bytes;
}

su_U64 su_mem_chunk_get_element_count(const struct su_MemChunk* chunk) {
    return chunk->element_count;
}

void* su_mem_pool_alloc(su_MemPool* pool, const su_U64 size) {
    return ArenaPush(&pool->arena, size);
}

su_Bool su_mem_safe_copy(void* dest_ptr, su_U64 dest_capacity, su_U64 dest_offset,
                         const void* src_ptr, su_U64 src_size, su_U64 src_offset,
                         su_U64 copy_length) {
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
        su_LOG_WARNF_M(su_LOG_TYPE_USER, su_LOG_WARN_SEVERITY_LOW,
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

/* === Internal Implementation === */

// This function expects pool and mem_out to be initialized,
// it is not it's responsability to check it.
su_Bool su__mem_alloc_arena(void* pool, const su_U64 capacity, const su_U64 size, void** mem_out) {
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

su_Bool su__mem_malloc(su_U64* capacity_out, const su_U64 size, void** mem_out) {
    if (!size) {
        su_LOG_ERROR_M(
            su_LOG_TYPE_USER,
            su_LOG_ERROR_SEVERITY_HIGH,
            su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Could not allocate memory: asking for 0 bytes");
    }
    *mem_out = malloc(size);
    if (!*mem_out) {
        return su_FALSE;
    }
    *capacity_out += size;
    return su_TRUE;
}
