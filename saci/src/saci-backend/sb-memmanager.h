#ifndef SACI_BACKEND_SB_MEMMANAGER_H
#define SACI_BACKEND_SB_MEMMANAGER_H

#include "saci-utils/su-types.h"

enum sb_MemContext {
    sb_MEM_CONTEXT_RENDERER,
};

#define sb_MEM_CONTEXT_COUNT 1

void sb_mem_init(const su_Bool use_arenas);

su_Bool sb_mem_alloc(const enum sb_MemContext ctx,
                     const su_U64 count,
                     const su_U64 element_type,
                     void** mem_out);

void sb_mem_print_info(void);

#endif
