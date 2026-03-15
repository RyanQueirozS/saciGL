#ifndef SACI_UTILS_MEMORY_SU_MEMORY_H
#define SACI_UTILS_MEMORY_SU_MEMORY_H

#include "./types.h"
#include "./defines.h"

enum SaciMemContext {
    SACI_MEM_CONTEXT_CONFIG,
    SACI_MEM_CONTEXT_RENDERER,
    SACI_MEM_CONTEXT_LUA,
    SACI_MEM_CONTEXT_GFX,
};

typedef struct SaciMemChunk SaciMemChunk;

typedef struct SaciMemPool SaciMemPool;

SACI_API void saci_mem_init(const SaciBool use_arenas);

SACI_API void saci_mem_print_info(void);

/* === Chunk === */

SACI_API SaciMemChunk* saci_mem_chunk_alloc_size(const enum SaciMemContext ctx,
                                                 const SaciU64 size);

SACI_API SaciMemChunk* saci_mem_chunk_alloc(const enum SaciMemContext ctx,
                                            const SaciU64 count,
                                            const SaciU64 element_type);

SACI_API SaciMemChunk* saci_mem_chunk_realloc(SaciMemChunk* old, SaciU64 new_size);

SACI_API SaciMemChunk* saci_mem_chunk_from_data(void* data_addr);

SACI_API SaciMemChunk* saci_mem_chunk_strdup(const enum SaciMemContext ctx,
                                             const char* str,
                                             const SaciU64 max_size);

SACI_API SaciMemPool* saci_mem_create_pool(const enum SaciMemContext ctx,
                                           const SaciU64 size);

SACI_API SaciBool saci_mem_chunk_set(struct SaciMemChunk* chunk, SaciU64 idx, const void* data, SaciU64 data_size);

SACI_API const void* saci_mem_chunk_get(struct SaciMemChunk* chunk, SaciU64 idx, SaciU64 data_size);

SACI_API void* saci_mem_chunk_get_ptr(struct SaciMemChunk* chunk, SaciU64 idx);

SACI_API void* saci_mem_chunk_get_ptr_offset(struct SaciMemChunk* chunk, SaciU64 offset);

SACI_API SaciBool saci_mem_chunk_get_ctx(const struct SaciMemChunk* chunk, enum SaciMemContext* ctx_out);

SACI_API SaciBool saci_mem_chunk_get_capacity(const struct SaciMemChunk* chunk, SaciU64* data_out);

SACI_API SaciBool saci_mem_chunk_get_element_size(const struct SaciMemChunk* chunk, SaciU64* data_out);

SACI_API SaciBool saci_mem_chunk_get_element_count(const struct SaciMemChunk* chunk, SaciU64* data_out);

SACI_API SaciBool saci_mem_chunk_free(struct SaciMemChunk* chunk);

/* === Pool === */

SACI_API void* saci_mem_pool_alloc(SaciMemPool* pool, const SaciU64 size);

SACI_API SaciBool saci_mem_safe_copy(void* dest_ptr, SaciU64 dest_capacity, SaciU64 dest_offset,
                                     const void* src_ptr, SaciU64 src_size, SaciU64 src_offset,
                                     SaciU64 copy_length);

SACI_API char* saci_mem_pool_cpy_str(const char* src, SaciMemPool* mem);

SACI_API SaciBool saci_mem_pool_free(struct SaciMemPool* pool);

#endif // SACI_UTILS_MEMORY_SU_MEMORY_H
