#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include "saci-core/sc-gl.h"
#include "saci-core/sc-event.h"
#include "saci-testing.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

static void __Saci_Init(sc_window_t** window_out, struct sc_renderer_c** renderer_out,
                        struct sc_camera_c* camera_out);

static double __Math_GetFPS(sa_u64_t frame_count, time_t begin, time_t end);

/* === Helper functions === */

static void __Test_Performance(void);

/* === Helper functions === */

void Test_Integration_Model_Loading(void) {
    __Test_Performance();
}

/* === Helper functions === */

static void __Test_Performance(void) {
    sc_window_t* window;
    struct sc_renderer_c* renderer;
    struct sc_camera_c camera;
    __Saci_Init(&window, &renderer, &camera);

    glfwSwapInterval(0);
    {
        printf("TEST WILL RUN FOR 10 SECONDS TO TAKE AVERAGE FPS\n");
        time_t begin = time(NULL);
        sa_u64_t frames = 0;
        while ((begin + 10) > time(NULL)) {
            sc_Renderer_Begin(renderer);
            sc_Window_Clear_Color(sa_Color_From_U8(255, 0, 0, 1));
            sc_Renderer_End(renderer, &camera);

            sc_Window_Swap_Buffer(window);
            sc_Event_Poll();
            frames++;
        }
        time_t end = time(NULL);
        double elapsed = difftime(end, begin);
        double fps = frames / elapsed;
        printf("%f\n", fps);
    }
}

static void __Saci_Init(sc_window_t** window_out, struct sc_renderer_c** renderer_out,
                        struct sc_camera_c* camera_out) {
    sa_Math_Init();
    assert(sc_GLFW_Init());
    *window_out = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window_out);
    sc_Window_Make_Context(*window_out);
    assert(sc_GLAD_Init());

    *renderer_out = sc_Renderer_Create_Default();
    assert(*renderer_out);

    *camera_out = sc_Camera_Get_Default();
    (*camera_out).m_aspect_ratio = 1600.0f / 900.0f; // Default is already 16/9 but just in case it changes

    sc_Renderer_Enable_Z_Buffer();
    sc_Renderer_Set_Projection_Mode(sa_RENDERER_PROJECTION_MODE_PERSPECTIVE);
}

static double __Math_GetFPS(sa_u64_t frame_count, time_t begin, time_t end) {
    double elapsed_time = difftime(end, begin);
    if (elapsed_time == 0) return 0;
    return frame_count / elapsed_time;
}
