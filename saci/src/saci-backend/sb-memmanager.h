#ifndef SACI_BACKEND_SB_MEMMANAGER_H
#define SACI_BACKEND_SB_MEMMANAGER_H

#include "saci-utils/su-types.h"

enum sb_MemContext {
    sb_MEM_CONTEXT_RENDERER,
};

struct sb_MemChunk;

#define sb_MEM_CONTEXT_COUNT 1

void sb_mem_init(const su_Bool use_arenas);

struct sb_MemChunk* sb_mem_alloc(const enum sb_MemContext ctx,
                                 const su_U64 count,
                                 const su_U64 element_type);

void sb_mem_print_info(void);

su_Bool sb_mem_chunk_alloc(struct sb_MemChunk* dest, const struct sb_MemChunk* src);

su_Bool sb_mem_chunk_alloc_data(struct sb_MemChunk* dest, const void* src, const su_U64 src_size);

#endif
