#ifndef SACI_BACKEND_WINDOWING_SB_WINDOWING_COMMON_H
#define SACI_BACKEND_WINDOWING_SB_WINDOWING_COMMON_H

#include "saci_util/types.h"

union sb_Window;

typedef void (*PSaciWindowMousePosHandler)(union sb_Window*, int, int);
typedef void (*PSaciWindowSizeHandler)(union sb_Window*, int, int);
typedef void (*PSaciWindowPosHandler)(union sb_Window*, int, int);

union PSaciWindow {
    struct {
        void* window;
        const char* title;
        int width, height;
    } glfw;
    struct {
        SaciU64 context;
        const char* selector;
        int width, height;
        int target_width, target_height; // For canvas vs display size
        PSaciWindowMousePosHandler mouse_pos_handler;
        PSaciWindowSizeHandler size_handler;
        SaciBool context_lost;
    } emscripten;
};

union PSaciWindowOpts {
    struct {
        void* monitor;
        void* window_share;
    } glfw;
    struct {
        int major_version; // WebGL major version (1 or 2)
        int minor_version; // WebGL minor version
        int alpha;
        int depth;
        int stencil;
        int antialias;
        int prefer_low_power_to_high_performance;
        int fail_if_major_performance_caveat;
        const char* power_preference; // "default", "low-power", "high-performance"
    } emscripten;
};

#endif // SACI_BACKEND_WINDOWING_SB_WINDOWING_COMMON_H
