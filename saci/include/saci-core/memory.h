#ifndef SACI_UTILS_MEMORY_SU_MEMORY_H
#define SACI_UTILS_MEMORY_SU_MEMORY_H

#include "./types.h"

enum SaciMemContext {
    SACI_MEM_CONTEXT_CONFIG,
    SACI_MEM_CONTEXT_RENDERER,
    SACI_MEM_CONTEXT_GFX,
};

typedef struct SaciMemChunk SaciMemChunk;

typedef struct SaciMemPool SaciMemPool;

void saci_mem_init(const SaciBool use_arenas);

void saci_mem_print_info(void);

SaciMemChunk* saci_mem_alloc_chunk_size(const enum SaciMemContext ctx,
                                        const SaciU64 size);

SaciMemChunk* saci_mem_alloc_chunk(const enum SaciMemContext ctx,
                                   const SaciU64 count,
                                   const SaciU64 element_type);

SaciMemPool* saci_mem_create_pool(const enum SaciMemContext ctx,
                                  const SaciU64 size);

SaciBool saci_mem_chunk_set(struct SaciMemChunk* chunk, SaciU64 idx, void* data, SaciU64 data_size);

const void* saci_mem_chunk_get(struct SaciMemChunk* chunk, SaciU64 idx, SaciU64 data_size);

void* saci_mem_chunk_get_ptr(struct SaciMemChunk* chunk, SaciU64 idx);

SaciBool saci_mem_chunk_get_ctx(const struct SaciMemChunk* chunk, enum SaciMemContext* ctx_out);

SaciBool saci_mem_chunk_get_capacity(const struct SaciMemChunk* chunk, SaciU64* data_out);

SaciBool saci_mem_chunk_get_element_size(const struct SaciMemChunk* chunk, SaciU64* data_out);

SaciBool saci_mem_chunk_get_element_count(const struct SaciMemChunk* chunk, SaciU64* data_out);

SaciBool saci_mem_chunk_free(struct SaciMemChunk* chunk);

void* saci_mem_pool_alloc(SaciMemPool* pool, const SaciU64 size);

SaciBool saci_mem_safe_copy(void* dest_ptr, SaciU64 dest_capacity, SaciU64 dest_offset,
                            const void* src_ptr, SaciU64 src_size, SaciU64 src_offset,
                            SaciU64 copy_length);

char* saci_mem_pool_cpy_str(const char* src, SaciMemPool* mem);

SaciBool saci_mem_pool_free(struct SaciMemPool* pool);

#endif // SACI_UTILS_MEMORY_SU_MEMORY_H
