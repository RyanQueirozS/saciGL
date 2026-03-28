#include "saci_platform/scripting/scripting.h"
#include "saci_util/types.h"

#include <stdlib.h>

SACI_API void* psaci_get_scripting_value_data(const struct PSaciScriptingValue* val)
{
    switch (val->type) {
    case SACI_TYPE_DOUBLE:
        return (void*)&val->data.f64;
    case SACI_TYPE_BOOL:
        return (void*)&val->data.boolean;
    default:
        return NULL;
    }
}
