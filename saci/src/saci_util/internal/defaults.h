#ifndef SACI_UTIL_INTERNAL_DEFAULTS_H
#define SACI_UTIL_INTERNAL_DEFAULTS_H

// This is related to the SACI_PROJECT_PATH macro, this is going to be inside
// the user's C project
#define SACI_CONFIG_PATH SACI_PROJECT_PATH "/config/saci_config.lua"
#define SACI_CONFIG_PATH_HEADER SACI_PROJECT_PATH "/config/saci_config.h"

// These are related to the SACI_LIB_PATH macro, this is going to be inside the
// saci library code
#define SACI_INTERNAL_CONFIG_PATH_API SACI_LIB_PATH "/config/saci_api.lua"
#define SACI_INTERNAL_CONFIG_PATH_CORE_API SACI_LIB_PATH "/config/saci_core_api.lua"
#define SACI_INTERNAL_CONFIG_PATH_PLATFORM_API SACI_LIB_PATH "/config/saci_platform_api.lua"

#endif // SACI_UTIL_INTERNAL_DEFAULTS_H
