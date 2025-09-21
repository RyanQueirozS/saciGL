#include "./sb-memmanager.h"

#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"

#include <arena/arena.h>
#include <stdio.h>

struct {
    su_U64 size_now;
    su_U64 capacity;

    void* arena; // Will either be a continuously reallocated chunk or an
                 // arena depending on build type
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
        su_FORCE_CRASH("Trying to reinitialize memory allocator",
                       su_CRASH_INIT_FAILURE);
        return;
    }
    sb__mem_manager_cfg.is_arena_based = use_arenas;
}

su_Bool sb_mem_alloc(const enum sb_MemContext ctx,
                     const su_U64 count,
                     const su_U64 element_size,
                     void** mem_out) {
    if (element_size != 0 && count > UINT64_MAX / element_size) {
        su_LOG_ERRORF_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_MEMORY,
                              "Overflow in allocation size");
        return su_FALSE;
    }
    su_U64 total_size = count * element_size;
    if (sb__mem_manager_cfg.is_arena_based) {
        if (sb__mem_manager[ctx].capacity < total_size) {
            su_LOG_ERRORF_PRINT_M(
                su_LOG_SEVERITY_MEDIUM,
                su_LOG_CONTEXT_MEMORY,
                "Could not allocate memory: capacity is %lu and asking for %lu",
                sb__mem_manager[ctx].capacity, total_size);
            return false;
        }
        *mem_out = ArenaPush(sb__mem_manager[ctx].arena, total_size);
        return su_TRUE;
    }
    *mem_out = su_MALLOC_M(total_size);
    su_LOG_DEBUGF_PRINT_M(su_LOG_DEBUG_TYPE_MEMORY, su_LOG_CONTEXT_MEMORY, "Allocating %lu bytes", total_size);
    if (!*mem_out) {
        return su_FALSE;
    }
    sb__mem_manager[ctx].capacity += total_size;

    return su_TRUE;
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
           "arena based allocator",
           capacity_total);
}
