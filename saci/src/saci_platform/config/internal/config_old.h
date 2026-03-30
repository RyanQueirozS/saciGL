#if 0
#  include "saci_platform/config/config.h"

#  include "saci_util/memory.h"

struct PSaciCfgArrayIterablePathTable {
    const char* path;
    PSaciLuaArrayIter iter;
};

struct PSaciCfgPreallocArrayEntity {
    const char* path_to_value;
    SaciU64* length_ptr;
    void** data_ptr;
    const SaciU64 STRUCT_SIZE;
};

void psaci_cfg_preallocate_array_component(PSaciLuaState* lua, SaciMemPool* pool, struct PSaciCfgPreallocArrayEntity prealloc_entity);
#endif
