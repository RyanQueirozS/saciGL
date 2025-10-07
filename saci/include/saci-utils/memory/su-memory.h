#ifndef SACI_UTILS_MEMORY_SU_MEMORY_H
#define SACI_UTILS_MEMORY_SU_MEMORY_H

#include "../su-types-common.h"

enum su_MemContext {
    su_MEM_CONTEXT_RENDERER,
};

#define su_MEM_CONTEXT_COUNT 1

struct su_MemChunk;

void su_mem_init(const su_Bool use_arenas);

void su_mem_print_info(void);

struct su_MemChunk* su_mem_alloc(const enum su_MemContext ctx,
                                 const su_U64 count,
                                 const su_U64 element_type);

su_Bool su_mem_chunk_push(struct su_MemChunk* dest, const struct su_MemChunk* src);

su_Bool su_mem_chunk_push_data(struct su_MemChunk* dest, const void* src, const su_U64 src_size);

enum su_MemContext su_mem_chunk_get_ctx(const struct su_MemChunk* chunk);

su_U64 su_mem_chunk_get_capacity(const struct su_MemChunk* chunk);

su_U64 su_mem_chunk_get_used(const struct su_MemChunk* chunk);

#endif // SACI_UTILS_MEMORY_SU_MEMORY_H
