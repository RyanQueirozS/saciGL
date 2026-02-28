/* Dylilo Header
 */
#ifndef DYLILO_H_
#define DYLILO_H_

/* === INCLUDES === */

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif //_WIN32

/* === DYLILO HELPER MACROS === */

#ifndef DYLILO_DEF
#  ifdef _WIN32
#    ifdef BUILD_DYLILO
#      define DYLILO_DEF __declspec(dllexport)
#    else
#      define DYLILO_DEF __declspec(dllimport)
#    endif
#  else
#    define DYLILO_DEF
#  endif // _WIN32
#endif   // DYLILO_DEF

#ifndef DYLILO_INL
#  define DYLILO_INL inline DYLILO_DEF
#endif // DYLILO_INL

#ifndef DYLILO_WARN_m
#  define DYLILO_WARN_m(msg) \
      (void)fprintf(stderr, "DYLILO WARN: %s\n", msg)
#endif // DYLILO_WARN_m

#ifndef DYLILO_ERROR_m
#  define DYLILO_ERROR_m(msg) \
      (void)fprintf(stderr, "DYLILO ERROR: %s\n", msg)
#endif // DYLILO_WARN_m

/* Return values */
#define DYLILO_STATUS_INVALID_PARAM (-1)
#define DYLILO_STATUS_SUCCESS (1)

#ifdef _WIN32
#  define DYLILO_FLAGS_DEFAULT 0
#else
#  define DYLILO_FLAGS_DEFAULT RTLD_LAZY
#endif

/* === DEFINITIONS === */

typedef void* DyliloHandle;

DYLILO_DEF int dylilo_get_status(void);

DYLILO_DEF DyliloHandle dylilo_load_lib(char* file_name, int flags);

/* === IMPLEMENTATIONS === */

#ifdef DYLILO_IMPL

static int dylilo__last_status = 0;

static void dylilo__set_status(int status) {
    dylilo__last_status = status;
}

DYLILO_DEF int dylilo_get_status(void) {
    return dylilo__last_status;
}

DYLILO_DEF DyliloHandle dylilo_load_lib(char* file_name, int flags) {
    if (!file_name) {
        DYLILO_ERROR_m("File name is NULL");
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return NULL;
    }
#  ifdef _WIN32
    if (flags != 0) {
        DYLILO_WARN_m("Passing flags when loading libs on a windows build");
    }
    HMODULE handle = LoadLibraryA(file_name);
    if (!handle) {
        DYLILO_ERROR_m("Failed to load library");
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return NULL;
    }
#  else
    if (flags < 1) {
        DYLILO_ERROR_m("Invalid or no flags passed when loading lib");
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return NULL;
    }
    void* handle = dlopen(file_name, flags);
    if (!handle) {
        DYLILO_ERROR_m(dlerror());
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return NULL;
    }
#  endif // _WIN32
    dylilo__set_status(DYLILO_STATUS_SUCCESS);
    return handle;
}

DYLILO_DEF int dylilo_unload_lib(DyliloHandle handle) {
    if (!handle) {
        DYLILO_ERROR_m("Handle is NULL");
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return 0;
    }
#  ifdef _WIN32
    BOOL result = FreeLibrary((HMODULE)handle);
    if (!result) {
        DYLILO_ERROR_m("Failed to unload library");
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return 0;
    }
#  else
    if (dlclose(handle) != 0) {
        DYLILO_ERROR_m(dlerror());
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return 0;
    }
#  endif // _WIN32
    dylilo__set_status(DYLILO_STATUS_SUCCESS);
    return 1;
}

DYLILO_DEF void* dylilo_get_symbol(DyliloHandle handle, const char* name) {
    if (!handle || !name) {
        DYLILO_ERROR_m("Handle or name is NULL");
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return NULL;
    }
#  ifdef _WIN32
    void* symbol = (void*)GetProcAddress((HMODULE)handle, name);
#  else
    void* symbol = dlsym(handle, name);
#  endif // _WIN32
    if (!symbol) {
        DYLILO_ERROR_m("Symbol not found");
        dylilo__set_status(DYLILO_STATUS_INVALID_PARAM);
        return NULL;
    }
    dylilo__set_status(DYLILO_STATUS_SUCCESS);
    return symbol;
}

#endif // DYLILO_IMPL

#endif // DYLILO_H_
