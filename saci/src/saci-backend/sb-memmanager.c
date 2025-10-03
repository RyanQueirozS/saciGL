#include "./sb-memmanager.h"

#include "saci-utils/su-log.h"
#include "saci-utils/su-general.h"

#include <arena/arena.h>
#include <saci-utils/su-types.h>
#include <stdio.h>
#include <string.h>

/* === Internal === */

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
