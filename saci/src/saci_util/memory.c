#include "saci_util/memory.h"

#include "saci_util/internal/log.h"

#include "saci_util/internal/general.h"
#include "saci_util/safe_string.h"
#include <saci_util/log.h>
#include <saci_util/types.h>
#include <stdlib.h>

#define ARENA_ASSERT(x) SACI_LOG_ASSERT_M(x, SACI_LOG_CONTEXT_CORE_MEMORY, "Error in arena function")
#define ARENA_FREE(x) free(x)
#define ARENA_MALLOC(x) malloc(x)
#define ARENA_ALLOCATOR_IMPL
#define ARENA_DEBUG
#include <arena/arena.h>
#include <stdio.h>
#include <string.h>

#define SACI_MEM_KB (1024ULL)
#define SACI_MEM_MB (1024ULL * SACI_MEM_KB)
#define SACI_MEM_GB (1024ULL * SACI_MEM_MB)
#define SACI_MEM_TB (1024ULL * SACI_MEM_GB)

#define SACI_MEM_CONTEXT_COUNT 3

#define SACI_MEM_FREED_POOL_PTR_ARRAY_COUNT 1024
#define SACI_MEM_FREED_CHUNK_PTR_ARRAY_COUNT 1024

#define SACI_CHUNK_END(chunk) ((char*)(chunk)->data + (chunk)->size)
#define SACI_CHUNK_START(chunk) ((char*)(chunk)->data)

/* === Internal === */

SACI_STATIC SaciBool saci__mem_alloc_arena(void* pool, const SaciU64 capacity, const SaciU64 size, void** mem_out);

SACI_STATIC SaciBool saci__mem_malloc(SaciU64* capacity_out, const SaciU64 size, void** mem_out);

SACI_STATIC SaciBool saci__mem_pool_alloc(void* pool, SaciU64* capacity_inout, const SaciU64 total_size, void** mem_out);

SACI_STATIC struct SaciMemChunk* saci__mem_chunk_find_suitable(const enum SaciMemContext ctx,
                                                               const SaciU64 size,
                                                               struct SaciMemChunk** chunk_ptr_array,
                                                               const SaciU64 chunk_ptr_array_len);

SACI_STATIC SaciBool saci__mem_chunk_is_empty(struct SaciMemChunk** chunk_ptr_array, const SaciU64 chunk_ptr_array_len);

SACI_STATIC SaciBool saci__mem_chunk_check_empty(struct SaciMemChunk** chunk_ptr_array, const SaciU64 chunk_ptr_array_len, SaciU64* idx_out);

SACI_STATIC int saci__mem_chunk_compare_addresses(const void* a, const void* b);

SACI_STATIC SaciU64 saci__mem_new_chunk_id(void);

SACI_STATIC void saci__mem_chunk_join_all(struct SaciMemChunk** mem_chunk_ptr_array, const SaciU64 array_len);

SACI_STATIC SaciBool saci__mem_safe_copy_validate(
    void* dest_ptr, SaciU64 dest_capacity, SaciU64 dest_offset,
    const void* src_ptr, SaciU64 src_size, SaciU64 src_offset,
    SaciU64 copy_length);

/* === Header impl === */

struct SaciMemPool {
    SaciU64 id; // Static. Is incremented per "new" mempool, allows reusing preexisting chunks.

    SaciU64 is_freed; // When freed the user might still have access, this flag denies the use from the functions in the su-memory.h

    Arena arena;
};

struct SaciMemChunk {
    SaciU64 id; // Static. Is incremented per "new" memchunk

    SaciU64 is_freed; // When freed the user might still have access, this flag denies the use from the functions in the su-memory.h

    enum SaciMemContext ctx;

    SaciU64 element_count;      // Can be 0
    SaciU64 element_size_bytes; // Can be 0
    SaciU64 size;               // Won't be 0

    void* data;
};

SACI_STATIC struct {
    struct {
        SaciU64 size_now;
        SaciU64 capacity;

        void* pool; // Will either be a continuously reallocated chunk or an
                    // pool depending on build type
    } context_array[SACI_MEM_CONTEXT_COUNT];

    SaciU64 mem_pool_id;  // Only increments
    SaciU64 mem_chunk_id; // Only increments
    struct SaciMemPool* freed_pool_ptr_array[SACI_MEM_FREED_POOL_PTR_ARRAY_COUNT];
    struct SaciMemChunk* freed_chunk_ptr_array[SACI_MEM_FREED_CHUNK_PTR_ARRAY_COUNT];
} saci_g_mem_manager = {0};

SACI_STATIC struct {
    SaciBool is_arena_based;
    SaciBool is_initialized;
    SaciU64 default_sizes[SACI_MEM_CONTEXT_COUNT];
} saci_g_mem_manager_cfg = {
    .is_arena_based = SACI_FALSE,
    .is_initialized = SACI_FALSE,
    .default_sizes = {
        2 * SACI_MEM_KB,
        1 * SACI_MEM_GB,
        2 * SACI_MEM_KB,
    },
};

void saci_mem_init(const SaciBool use_arenas)
{
    if (saci_g_mem_manager_cfg.is_initialized) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_DEV, SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Trying to reinitialize memory allocator");
        return;
    }
    saci_g_mem_manager_cfg.is_arena_based = use_arenas;
    if (use_arenas) {
        for (SaciU64 i = 0; i < SACI_MEM_CONTEXT_COUNT; ++i) {
            ArenaInit(saci_g_mem_manager.context_array[i].pool, saci_g_mem_manager_cfg.default_sizes[i]);
        }
    }
}

struct SaciMemChunk* saci_mem_chunk_alloc_size(const enum SaciMemContext ctx,
                                               const SaciU64 size)
{
    if (size == 0) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER,
                         SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                         "Trying to allocate memory passing size 0");
    }
    const SaciU64 total_size = size + sizeof(struct SaciMemChunk);
    struct SaciMemChunk* suitable_chunk = saci__mem_chunk_find_suitable(
        ctx, total_size, saci_g_mem_manager.freed_chunk_ptr_array,
        SACI_MEM_FREED_CHUNK_PTR_ARRAY_COUNT);
    if (suitable_chunk) {
        suitable_chunk->element_count = 0; // zero the 'unused element data'
        suitable_chunk->element_size_bytes = 0;
        return suitable_chunk;
    }
    void* memctx = NULL;
    saci__mem_pool_alloc(saci_g_mem_manager.context_array[ctx].pool, &saci_g_mem_manager.context_array[ctx].capacity,
                         total_size, &memctx);
    if (!memctx) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Memory context could not be created");
        return NULL;
    }

    // Allocates the memory for the chunk
    struct SaciMemChunk* chunk = (struct SaciMemChunk*)(memctx);
    *chunk = (struct SaciMemChunk){
        .id = saci__mem_new_chunk_id(),
        .size = size, // Will set size, but make 'element data' be zeroed
        .ctx = ctx,
        .element_size_bytes = 0,
        .element_count = 0,
        .data = (void*)((char*)memctx + sizeof(struct SaciMemChunk)), // Data goes after the chunk structure
    };

    return chunk;
}

struct SaciMemChunk* saci_mem_chunk_alloc(const enum SaciMemContext ctx,
                                          const SaciU64 count,
                                          const SaciU64 element_size)
{
    void* memctx = NULL;
    if (element_size != 0 && count > UINT64_MAX / element_size) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Overflow in allocation size");
        return NULL;
    }
    const SaciU64 size = count * element_size;
    const SaciU64 total_size = size + sizeof(struct SaciMemChunk);

    struct SaciMemChunk* suitable_chunk = saci__mem_chunk_find_suitable(
        ctx, total_size, saci_g_mem_manager.freed_chunk_ptr_array, SACI_MEM_FREED_CHUNK_PTR_ARRAY_COUNT);
    if (suitable_chunk) {
        suitable_chunk->element_size_bytes = element_size;
        suitable_chunk->element_count = count;
        suitable_chunk->is_freed = SACI_FALSE;
        return suitable_chunk;
    }

    saci__mem_pool_alloc(saci_g_mem_manager.context_array[ctx].pool, &saci_g_mem_manager.context_array[ctx].capacity,
                         total_size, &memctx);
    if (!memctx) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Memory context could not be created");
        return NULL;
    }

    // Allocates the memory for the chunk
    struct SaciMemChunk* chunk = (struct SaciMemChunk*)(memctx);
    *chunk = (struct SaciMemChunk){
        .id = saci__mem_new_chunk_id(),
        .is_freed = SACI_FALSE,
        .ctx = ctx,
        .element_count = count,
        .element_size_bytes = element_size,
        .size = size,

        // Data goes after the chunk structure
        .data = (void*)((char*)memctx + sizeof(struct SaciMemChunk)),
    };
    return chunk;
}

SACI_API SaciMemChunk* saci_mem_chunk_strdup(const enum SaciMemContext ctx,
                                             const char* str,
                                             const SaciU64 max_size)
{
    SaciMemChunk* strchunk = saci_mem_chunk_alloc(ctx, saci_safe_str_len(str, max_size), sizeof(char));

    saci_mem_chunk_set(strchunk, 0, str, saci_safe_str_len(str, max_size) * sizeof(char));

    return strchunk;
}

SaciMemPool* saci_mem_create_pool(const enum SaciMemContext ctx,
                                  const SaciU64 size)
{
    void* memctx = NULL;
    if (size <= 0) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Overflow in allocation size");
        return NULL;
    }
    SaciU64 total_size = sizeof(SaciMemPool) + size;

    // In this case will generate a partion out of the mem_manager's memory pool.
    if (!saci__mem_pool_alloc(saci_g_mem_manager.context_array[ctx].pool,
                              &saci_g_mem_manager.context_array[ctx].capacity,
                              total_size, &memctx)) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Memory context could not be created");
        return NULL;
    }

    SaciMemPool* pool = (SaciMemPool*)memctx;
    ArenaInitCtx(&pool->arena, (void*)((char*)memctx + sizeof(SaciMemPool)), size);

    return pool;
}

SaciBool saci_mem_chunk_set(struct SaciMemChunk* chunk, SaciU64 idx, const void* data, SaciU64 data_size)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in set func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in set func");
    if (chunk->element_count < idx && chunk->element_count != 0) {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
            SACI_LOG_CONTEXT_CORE_MEMORY,
            "Could not set data in chunk, idx is " SACI_FMTU64 " and there are " SACI_FMTU64 " elements",
            chunk->element_count, idx);
    }
    if (chunk->element_size_bytes != data_size && chunk->element_size_bytes != 0) {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
            SACI_LOG_CONTEXT_CORE_MEMORY,
            "Could not set data in chunk, data (" SACI_FMTU64 ") is bigger than element "
            "size (" SACI_FMTU64 ") in chunk",
            chunk->element_size_bytes, idx);
    }
    const SaciU64 dest_capacity = chunk->element_size_bytes * chunk->element_count;
    const SaciU64 dest_offset = chunk->element_size_bytes * idx;
    // We use the chunk->element_size_bytes as the copy length to avoid overflow
    saci_mem_safe_copy(chunk->data, dest_capacity, dest_offset, data, 0, data_size,
                       chunk->element_size_bytes ? chunk->element_size_bytes : data_size);
    return SACI_TRUE;
}

const void* saci_mem_chunk_get(struct SaciMemChunk* chunk, SaciU64 idx, SaciU64 data_size)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in get func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in get func");

    if (data_size < chunk->element_size_bytes && chunk->element_size_bytes) // has to have non-zero element_size
    {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY,
            "Could not get data from chunk, data_size expected (" SACI_FMTU64 ") is not "
            "equal to the one recieved (" SACI_FMTU64 ")",
            chunk->element_count, idx);
    }
    if (idx < chunk->element_count && chunk->element_count) // has to have non-zero element_count
    {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY,
            "Could not get data from chunk, idx is " SACI_FMTU64 " and there are " SACI_FMTU64 " elements",
            chunk->element_count, idx);
    }
    return (const void*)((char*)chunk->data + (idx * chunk->element_size_bytes));
}

void* saci_mem_chunk_get_ptr(struct SaciMemChunk* chunk, SaciU64 idx)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in get ptr func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in get ptr func");
    if (idx >= chunk->element_count && chunk->element_count) {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY,
            "Could not get data from chunk, idx is " SACI_FMTU64 " and there are " SACI_FMTU64 " elements",
            chunk->element_count, idx);
    }
    return (void*)((char*)chunk->data + (idx * chunk->element_size_bytes));
}

void* saci_mem_chunk_get_ptr_offset(struct SaciMemChunk* chunk, SaciU64 offset)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in get ptr func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in get ptr func");
    if (chunk->element_count) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY,
            "Forbidden use of element based chunk on a offset operation");
    }
    return (void*)((char*)(chunk->data) + (offset));
}

void saci_mem_print_info(void)
{
    SaciU64 capacity_total = 0, size_total = 0;
    for (SaciU64 i = 0; i < SACI_MEM_CONTEXT_COUNT; ++i) {
        capacity_total += saci_g_mem_manager.context_array[i].capacity;
        size_total += saci_g_mem_manager.context_array[i].size_now;
    }
    if (saci_g_mem_manager_cfg.is_arena_based) {
        printf("Allocated " SACI_FMTU64 " memory and used " SACI_FMTU64, capacity_total, size_total);
        return;
    }

    printf("Allocated " SACI_FMTU64 " bytes of memory. Could not calculate memory used: Not using "
           "pool based allocator",
           capacity_total);
}

SaciBool saci_mem_chunk_get_ctx(const struct SaciMemChunk* chunk, enum SaciMemContext* ctx_out)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in get ctx func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in get ctx func");

    *ctx_out = chunk->ctx;
    return SACI_TRUE;
}

SaciBool saci_mem_chunk_get_capacity(const struct SaciMemChunk* chunk, SaciU64* data_out)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in get capacity func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in get capacity func");
    *data_out = chunk->size;
    return SACI_TRUE;
}

SaciBool saci_mem_chunk_get_element_size(const struct SaciMemChunk* chunk, SaciU64* data_out)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in get element size func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in get element size func");
    *data_out = chunk->element_size_bytes;
    return SACI_TRUE;
}

SaciBool saci_mem_chunk_get_element_count(const struct SaciMemChunk* chunk, SaciU64* data_out)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using empty memory chunk in get element count func");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Using freed memory chunk in get element count func");
    *data_out = chunk->element_count;
    return SACI_TRUE;
}

SaciBool saci_mem_chunk_free(struct SaciMemChunk* chunk)
{
    SACI_LOG_DUMMY_CHECK_M(chunk, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Trying to free a chunk that is NULL");
    SACI_LOG_DUMMY_CHECK_M(!chunk->is_freed, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                           "Trying to free a already freed memory chunk");
    SaciU64 suitable_idx = 0;
    saci__mem_chunk_check_empty(saci_g_mem_manager.freed_chunk_ptr_array, SACI_MEM_FREED_CHUNK_PTR_ARRAY_COUNT, &suitable_idx);

    if (!suitable_idx) { // Cannot free if there is no freed chunk space
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                         "Trying to free caused an overflow in freed chunk's, "
                         "please handle memory more carefully");
        return SACI_FALSE;
    }
    chunk->is_freed = SACI_TRUE;
    saci_g_mem_manager.freed_chunk_ptr_array[suitable_idx] = chunk;
    saci__mem_chunk_join_all(saci_g_mem_manager.freed_chunk_ptr_array, SACI_MEM_FREED_CHUNK_PTR_ARRAY_COUNT);

    return SACI_TRUE;
}

void* saci_mem_pool_alloc(SaciMemPool* pool, const SaciU64 size)
{
    return ArenaPush(&pool->arena, size);
}

SaciBool saci_mem_safe_copy(
    void* dest_ptr, SaciU64 dest_capacity, SaciU64 dest_offset,
    const void* src_ptr, SaciU64 src_size, SaciU64 src_offset,
    SaciU64 copy_length)
{
    if (!saci__mem_safe_copy_validate(dest_ptr, dest_capacity, dest_offset,
                                      src_ptr, src_size, src_offset,
                                      copy_length)) {
        return SACI_FALSE;
    }
    uint8_t* dest_start = (uint8_t*)dest_ptr + dest_offset;
    const uint8_t* src_start = (const uint8_t*)src_ptr + src_offset;

    SaciBool does_overlap =
        (src_start < dest_start && src_start + copy_length > dest_start) ||
        (dest_start < src_start && dest_start + copy_length > src_start);
    if (does_overlap) {
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_HIGH,
                          SACI_LOG_CONTEXT_CORE_MEMORY,
                          "Overlapping memory regions in safe memcpy (src: %p, dest: %p, length: " SACI_FMTU64 ")",
                          (void*)src_start, (void*)dest_start, copy_length);
        return SACI_FALSE;
    }

    memcpy(dest_start, src_start, copy_length);

    SACI_LOG_ASSERTF_M(*src_start == *dest_start,
                       SACI_LOG_CONTEXT_CORE_MEMORY,
                       "First byte verification in safe memcpy");
    if (copy_length > 1) {
        SACI_LOG_ASSERTF_M(*(src_start + copy_length - 1) == *(dest_start + copy_length - 1),
                           SACI_LOG_CONTEXT_CORE_MEMORY,
                           "Last byte verification in safe memcpy");
    }

    SACI_LOG_INFOF_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_CORE_MEMORY,
                     "Safe memcpy completed successfully (dest: %p+" SACI_FMTU64 ", src: %p+" SACI_FMTU64 ", length: " SACI_FMTU64 ")",
                     dest_ptr, dest_offset, src_ptr, src_offset, copy_length);

    return SACI_TRUE;
}

char* saci_mem_pool_cpy_str(const char* src, SaciMemPool* mem)
{
    if (!src) {
        return NULL;
    }
    SaciU64 len = strlen(src) + 1;
    char* dst = (char*)saci_mem_pool_alloc(mem, len);
    if (dst) {
        saci_mem_safe_copy(dst, len, 0, src, len, 0, len);
    }
    return dst;
}

SaciBool saci_mem_pool_free(struct SaciMemPool* pool)
{
    if (!pool) {
        return SACI_FALSE;
    };

    // TODO need to send to free array and coalesce
    return SACI_TRUE;
}

/* === Internal Implementation === */

// This function expects pool and mem_out to be initialized,
// it is not it's responsability to check it.
SaciBool saci__mem_alloc_arena(void* pool, const SaciU64 capacity, const SaciU64 size, void** mem_out)
{
    if (capacity < size) {
        SACI_LOG_ERRORF_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_CRASH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Could not allocate memory: capacity is " SACI_FMTU64 " and asking for " SACI_FMTU64,
            capacity, size);
        return SACI_FALSE;
    }
    *mem_out = ArenaPush(pool, size);
    return (*mem_out) ? SACI_TRUE : SACI_FALSE;
}

SaciBool saci__mem_malloc(SaciU64* capacity_out, const SaciU64 size, void** mem_out)
{
    if (!size) {
        SACI_LOG_ERROR_M(
            SACI_LOG_TYPE_USER,
            SACI_LOG_ERROR_SEVERITY_HIGH,
            SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
            "Could not allocate memory: asking for 0 bytes");
    }
    *mem_out = malloc(size);
    if (!*mem_out) {
        return SACI_FALSE;
    }
    *capacity_out += size;
    return SACI_TRUE;
}

struct SaciMemChunk* saci__mem_chunk_find_suitable(const enum SaciMemContext ctx,
                                                   const SaciU64 size,
                                                   struct SaciMemChunk** chunk_ptr_array_inout,
                                                   const SaciU64 chunk_ptr_array_len)
{
    // There is no way to split a chunk into a suitable and another unsuitable
    // chunk if there is no space (empty slot) in ptr_array
    if (saci__mem_chunk_is_empty(chunk_ptr_array_inout, chunk_ptr_array_len)) {
        return NULL;
    };
    SaciU64 empty_slot_idx = 0;
    SaciBool has_empty_slot = saci__mem_chunk_check_empty(chunk_ptr_array_inout, chunk_ptr_array_len, &empty_slot_idx);
    if (!has_empty_slot) {
        return NULL;
    }

    struct SaciMemChunk* suitable_chunk = NULL;
    struct SaciMemChunk* split_chunk = NULL;

    for (SaciU64 i = 0; i < SACI_MEM_FREED_CHUNK_PTR_ARRAY_COUNT; ++i) {
        struct SaciMemChunk* chunk_ptr = chunk_ptr_array_inout[i];
        if (!chunk_ptr) {
            continue;
        }

        if (chunk_ptr->ctx != ctx) {
            continue;
        }

        const SaciU64 iter_total_size = chunk_ptr->element_size_bytes * chunk_ptr->element_count;
        // Check if can hold size + a new mem_chunk from it's data
        if (iter_total_size >= size + sizeof(struct SaciMemChunk)) {
            suitable_chunk = chunk_ptr; // set the suitable_chunk
            split_chunk = (struct SaciMemChunk*)((char*)chunk_ptr->data + size);
            *split_chunk = (struct SaciMemChunk){
                .data = (void*)((char*)split_chunk + sizeof(struct SaciMemChunk)),
                .ctx = ctx,
                .id = chunk_ptr->id,
                .size = iter_total_size - (size + sizeof(struct SaciMemChunk)), // reamining size of previous chunk
            };
            chunk_ptr_array_inout[empty_slot_idx] = split_chunk;
            chunk_ptr_array_inout[i] = NULL;
            break;
        }
    }
    suitable_chunk->size = size;
    return suitable_chunk;
}

SaciBool saci__mem_chunk_is_empty(struct SaciMemChunk** chunk_ptr_array, const SaciU64 chunk_ptr_array_len)
{
    for (SaciU64 i = 0; i < chunk_ptr_array_len; ++i) {
        if (chunk_ptr_array[i]) {
            return SACI_FALSE;
        }
    }
    return SACI_TRUE;
}

SaciBool saci__mem_chunk_check_empty(struct SaciMemChunk** chunk_ptr_array, const SaciU64 chunk_ptr_array_len, SaciU64* idx_out)
{
    for (SaciU64 i = 0; i < chunk_ptr_array_len; ++i) {
        if (chunk_ptr_array[i] == NULL) {
            *idx_out = i;
            return SACI_TRUE;
        }
    }
    *idx_out = 0;
    return SACI_FALSE;
}

SaciU64 saci__mem_new_chunk_id(void)
{
    return ++saci_g_mem_manager.mem_chunk_id;
}

int saci__mem_chunk_compare_addresses(const void* a, const void* b)
{
    const SaciMemChunk* ca = *(const SaciMemChunk**)a;
    const SaciMemChunk* cb = *(const SaciMemChunk**)b;

    if ((SaciUintPtr)ca->data < (SaciUintPtr)cb->data) {
        return -1;
    }
    if ((SaciUintPtr)ca->data > (SaciUintPtr)cb->data) {
        return 1;
    }
    return 0;
}

void saci__mem_chunk_join_all(struct SaciMemChunk** mem_chunk_ptr_array, const SaciU64 array_len)
{
    SACI_LOG_ASSERT_M(mem_chunk_ptr_array, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                      "Invalid free chunk array");
    SACI_LOG_ASSERT_M(array_len > 0, SACI_LOG_CONTEXT_CORE_MEMORY_MANAGER,
                      "Invalid array free chunk length");
    qsort((void*)mem_chunk_ptr_array, array_len, sizeof(struct SaciMemChunk*),
          saci__mem_chunk_compare_addresses);

    SaciU64 write_idx = 0;
    for (SaciU64 read_idx = 1; read_idx < array_len; ++read_idx) {
        struct SaciMemChunk* write = mem_chunk_ptr_array[write_idx];
        struct SaciMemChunk* read = mem_chunk_ptr_array[read_idx];
        if (!write) {
            mem_chunk_ptr_array[write_idx] = read;
            continue;
        }

        if (!read) {
            continue;
        }

        if (SACI_CHUNK_END(write) == SACI_CHUNK_START(read)) {
            write->size += read->size;
            continue;
        }

        ++write_idx;
        mem_chunk_ptr_array[write_idx] = read;
    }
}

SaciBool saci__mem_pool_alloc(void* pool, SaciU64* capacity_inout, const SaciU64 total_size, void** mem_out)
{
    if (pool) {
        return saci__mem_alloc_arena(
            pool,
            *capacity_inout,
            total_size, mem_out);
    }

    return saci__mem_malloc(capacity_inout, total_size, mem_out);
}

SACI_STATIC SaciBool saci__mem_safe_copy_validate(
    void* dest_ptr, SaciU64 dest_capacity, SaciU64 dest_offset,
    const void* src_ptr, SaciU64 src_size, SaciU64 src_offset,
    SaciU64 copy_length)
{
    SaciBool should_crash = SACI_FALSE;
    char* invalid_checks[7] = {0};
    if (dest_ptr == NULL) {
        invalid_checks[0] = "Destination pointer";
        should_crash = SACI_TRUE;
    }

    if (src_ptr == NULL) {
        invalid_checks[1] = "Source pointer";
        should_crash = SACI_TRUE;
    }

    if (copy_length == 0) {
        invalid_checks[2] = "Copy length";
    }

    if (dest_offset > dest_capacity) {
        invalid_checks[3] = "Destination offset exceeds capacity";
        should_crash = SACI_TRUE;
    }

    if (dest_offset + copy_length > dest_capacity) {
        invalid_checks[4] = "Copy would exceed destination capacity";
        should_crash = SACI_TRUE;
    }

    if (src_offset > src_size) {
        invalid_checks[5] = "Source offset exceeds source size";
        should_crash = SACI_TRUE;
    }

    if (src_offset + copy_length > src_size) {
        invalid_checks[6] = "Copy would exceed source bounds";
        should_crash = SACI_TRUE;
    }

    SaciBool has_errors = SACI_FALSE;
    enum SaciLogErrorSeverity severity = should_crash
                                             ? SACI_LOG_ERROR_SEVERITY_CRASH
                                             : SACI_LOG_ERROR_SEVERITY_HIGH;
    for (SaciU64 i = 0; i < SACI_ARRLEN_M(invalid_checks); ++i) {
        if (invalid_checks[i][0]) {
            SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, severity,
                              SACI_LOG_CONTEXT_CORE_MEMORY,
                              "Error while copying memory: %s",
                              invalid_checks[i]);
            has_errors = SACI_TRUE;
        }
    }

    return has_errors;
}
