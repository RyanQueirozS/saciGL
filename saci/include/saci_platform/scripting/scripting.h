#ifndef SACI_PLATFORM_SCRIPTING_SCRIPTING_H
#define SACI_PLATFORM_SCRIPTING_SCRIPTING_H

#include "saci_util/types.h"

#include "saci_util/memory.h"

struct PSaciScriptingValue {
    SaciDataType type;
    union {
        double f64;
        SaciBool boolean;
        SaciS64 s64;
        SaciMemChunk* string;
        struct PSaciScriptingTable* table;
        void* voidptr;
        void* function;
        void* thread;
        SaciS32 func_ref;
        SaciS32 thread_ref;
    } data;
};

struct PSaciScriptingField {
    struct PSaciScriptingValue value;
    const char* name;
};

struct PSaciScriptingTable {
    SaciMemChunk* chunk;
    struct PSaciScriptingField* field_array;
    SaciU64 field_count;
};

SACI_API void* psaci_get_scripting_value_data(const struct PSaciScriptingValue* val);

#endif // SACI_PLATFORM_SCRIPTING_SCRIPTING_H
