#include "saci_platform/config/internal/prealloc.h"

void psaci_cfg_preallocate_array_component(PSaciLuaState* lua, SaciMemPool* pool, struct PSaciCfgPreallocArrayEntity prealloc_entity)
{
    *prealloc_entity.length_ptr = psaci_lua_get_array_length(lua, prealloc_entity.path_to_value);
    *prealloc_entity.data_ptr = saci_mem_pool_alloc(pool, prealloc_entity.STRUCT_SIZE * (*prealloc_entity.length_ptr));
}
