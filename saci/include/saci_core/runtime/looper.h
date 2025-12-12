#ifndef SACI_BACKEND_WINDOWING_SB_LOOPER_H
#define SACI_BACKEND_WINDOWING_SB_LOOPER_H

#include "saci_util/defines.h"
#include "saci_util/types.h"

/* Source file includes:
 * - saci_platform/gfx/internal/gles3.h
 */

typedef struct {
    double frame_delta;
    float fps;
    // TODO more usefull fields to the user
} CSaciLoopFrameData;

typedef struct {
    float desired_fps;
} CSaciMainLoopOpts;

typedef void (*CSaciMainLoopFunc)(CSaciLoopFrameData);

SACI_API void csaci_looper_set_main_loop(CSaciMainLoopFunc loop_func, CSaciMainLoopOpts loop_opts);
SACI_API void csaci_looper_run(void);
SACI_API void csaci_looper_stop(void);
SACI_API SaciBool csaci_looper_is_running(void);

#endif // SACI_BACKEND_WINDOWING_SB_LOOPER_H
