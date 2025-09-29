#include "./sb-memmanager.h"

#include "saci-utils/su-log.h"
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
        su_LOG_ERROR_M(su_LOG_TYPE_DEV, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_CORE_MEMORY_MANAGER, "Trying to reinitialize memory allocator");
        return;
    }
    sb__mem_manager_cfg.is_arena_based = use_arenas;
}

su_Bool sb_mem_alloc(const enum sb_MemContext ctx,
                     const su_U64 count,
                     const su_U64 element_size,
                     void** mem_out) {
    if (element_size != 0 && count > UINT64_MAX / element_size) {
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                        "Overflow in allocation size");
    }
    su_U64 total_size = count * element_size;
    if (sb__mem_manager_cfg.is_arena_based) {
        if (sb__mem_manager[ctx].capacity < total_size) {
            su_LOG_ERRORF_M(
                su_LOG_TYPE_USER,
                su_LOG_ERROR_SEVERITY_CRASH,
                su_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                "Could not allocate memory: capacity is %lu and asking for %lu",
                sb__mem_manager[ctx].capacity, total_size);
        }
        *mem_out = ArenaPush(sb__mem_manager[ctx].arena, total_size);
        return su_TRUE;
    }
    *mem_out = su_MALLOC_M(total_size);
    su_LOG_INFOF_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_CORE_MEMORY_MANAGER, "Allocating %lu bytes", total_size);
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
