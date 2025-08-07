#if defined(__APPLE__) || defined(__linux__)
#  define _GNU_SOURCE
#endif
#include "saci-backend/sb-config-manager.h"

#include "saci-utils/su-general.h"
#include <saci-utils/su-debug.h>

#include <string.h>

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#  include <dlfcn.h>
#  include <limits.h>
#  include <stdlib.h>
#endif

#define DYLILO_IMPL
#include "dylilo/dylilo.h"

/* === Helper === */

SA_INTERNAL_CONST char* sb__get_lib_path(void);

/* === Header impl === */

SA_INTERNAL struct sb_ConfigManager sb_cfg_manager = sb_CFG_MANAGER_DEFAULT;

void sb_cfg_manager_load_default(void) {
    sb_cfg_manager = sb_CFG_MANAGER_DEFAULT;
    sb_cfg_manager.saci_lib_path = sb__get_lib_path();
    printf("%s\n", sb_cfg_manager.saci_lib_path);
}

void sb_cfg_manager_set(const struct sb_ConfigManager cfg_manager) {
    sb_cfg_manager = cfg_manager;
}

void sb_cfg_manager_fetch(const char* path) {}

void sb_cfg_manager_load_dependencies(void) {
    switch (sb_cfg_manager.sc_renderer_api) {
    case sb_RENDERER_API_OPENGL:
        dylilo_load_lib("", DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid renderer api");
        exit(1);
    }

    switch (sb_cfg_manager.sc_render_api_loader) {
    case sb_RENDER_API_LOADER_GLAD:
        break;
    default:
        exit(1);
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid render api loader");
    }

    switch (sb_cfg_manager.sc_window_api) {
    case sb_WINDOW_API_GLFW:
        dylilo_load_lib("", DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid windowing api");
        exit(1);
    }
}

enum sb_RendererApi sb_cfg_manager_get_renderer_api(void) {
    return sb_cfg_manager.sc_renderer_api;
}

/* === Helper === */
SA_INTERNAL_CONST char* sb__get_lib_path(void) {
    static char sb__lib_path[1024] = {0};

#if defined(_WIN32)
    HMODULE hModule = NULL;
    if (GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)&sb__get_lib_path, &hModule)) {
        GetModuleFileNameA(hModule, sb__lib_path, sizeof(sb__lib_path));
    }

#elif defined(__APPLE__) || defined(__linux__)
    Dl_info dl_info;
    if (dladdr((void*)sb__get_lib_path, &dl_info)) {
        char resolved[PATH_MAX];
        if (realpath(dl_info.dli_fname, resolved)) {
            strncpy(sb__lib_path, resolved, sizeof(sb__lib_path) - 1);
            sb__lib_path[sizeof(sb__lib_path) - 1] = '\0';
        } else {
            strncpy(sb__lib_path, dl_info.dli_fname, sizeof(sb__lib_path) - 1);
            sb__lib_path[sizeof(sb__lib_path) - 1] = '\0';
        }
    }
#endif

    return sb__lib_path[0] ? sb__lib_path : NULL;
}
