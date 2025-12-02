#ifndef SACI_UTILS_MEMORY_SU_MEMORY_H
#define SACI_UTILS_MEMORY_SU_MEMORY_H

#include "../su-types-common.h"

enum su_MemContext {
    su_MEM_CONTEXT_CONFIG,
    su_MEM_CONTEXT_RENDERER,
    su_MEM_CONTEXT_GFX,
};

typedef struct su_MemChunk su_MemChunk;

typedef struct su_MemPool su_MemPool;

void su_mem_init(const su_Bool use_arenas);

void su_mem_print_info(void);

su_MemChunk* su_mem_alloc_chunk_size(const enum su_MemContext ctx,
                                     const su_U64 size);

su_MemChunk* su_mem_alloc_chunk(const enum su_MemContext ctx,
                                const su_U64 count,
                                const su_U64 element_type);

su_MemPool* su_mem_create_pool(const enum su_MemContext ctx,
                               const su_U64 size);

su_Bool su_mem_chunk_set(struct su_MemChunk* chunk, su_U64 idx, void* data, su_U64 data_size);

const void* su_mem_chunk_get(struct su_MemChunk* chunk, su_U64 idx, su_U64 data_size);

void* su_mem_chunk_get_ptr(struct su_MemChunk* chunk, su_U64 idx);

su_Bool su_mem_chunk_get_ctx(const struct su_MemChunk* chunk, enum su_MemContext* ctx_out);

su_Bool su_mem_chunk_get_capacity(const struct su_MemChunk* chunk, su_U64* data_out);

su_Bool su_mem_chunk_get_element_size(const struct su_MemChunk* chunk, su_U64* data_out);

su_Bool su_mem_chunk_get_element_count(const struct su_MemChunk* chunk, su_U64* data_out);

su_Bool su_mem_chunk_free(struct su_MemChunk* chunk);

void* su_mem_pool_alloc(su_MemPool* pool, const su_U64 size);

su_Bool su_mem_safe_copy(void* dest_ptr, su_U64 dest_capacity, su_U64 dest_offset,
                         const void* src_ptr, su_U64 src_size, su_U64 src_offset,
                         su_U64 copy_length);

char* su_mem_pool_cpy_str(const char* src, su_MemPool* mem);

su_Bool su_mem_pool_free(struct su_MemPool* pool);

#endif // SACI_UTILS_MEMORY_SU_MEMORY_H
