#ifndef SACI_BACKEND_WINDOWING_SB_LOOPER_H
#define SACI_BACKEND_WINDOWING_SB_LOOPER_H

#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"

typedef struct {
    double frame_delta;
    float fps;
    // TODO more usefull fields to the user
} sb_LoopFrameData;

typedef struct {
    float desired_fps;
} sb_MainLoopOpts;

typedef void (*sb_MainLoopFunc)(sb_LoopFrameData);

SA_API void sb_looper_set_main_loop(sb_MainLoopFunc loop_func, sb_MainLoopOpts loop_opts);
SA_API void sb_looper_run(void);
SA_API void sb_looper_stop(void);
SA_API su_Bool sb_looper_is_running(void);

#endif // SACI_BACKEND_WINDOWING_SB_LOOPER_H
