#ifndef SACI_CORE_CONFIG_H
#define SACI_CORE_CONFIG_H

#include <lua5.4/lua.h>

#include "saci-utils/su-general.h"
#include "saci-utils/su-types.h"

typedef lua_State sc_configState;

SA_API sc_configState* sc_Config_Load(const char* file_path);

SA_API su_bool sc_Config_Load_Table(sc_configState* state, const char* table_name);

SA_API su_s8 sc_Config_Get_Bool(sc_configState* state, const char* b_name);

SA_API su_u8 sc_Config_Get_Uint8(sc_configState* state, const char* i_name);

SA_API su_u32 sc_Config_Get_Uint32(sc_configState* state, const char* i_name);

SA_API su_u64 sc_Config_Get_Uint64(sc_configState* state, const char* i_name);

SA_API const char* sc_Config_Get_Str(sc_configState* state, const char* s_name);

SA_API void sc_Config_Close(sc_configState* state);

#endif // SACI_CORE_CONFIG_H
