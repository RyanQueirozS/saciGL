/**
 * @file sc-rendering.h
 * @brief This header defines camera related functions and structs related to saciCORE
 */
#ifndef __SACI_CORE_SC_CAMERA_H__
#define __SACI_CORE_SC_CAMERA_H__

#include "saci-utils/su-types.h"

/**
 * @struct sc_Camera
 * @brief Represents a 3D camera in the saciCORE graphics system.
 *
 * @details
 * The `sc_Camera` struct defines the properties of a camera, including its
 * position in 3D space, the target it looks at, the up direction,
 * and projection parameters such as field of view, aspect ratio,
 * and clipping planes. This struct is used for configuring the
 * camera's view in the scene and for rendering purposes.
 */
typedef struct sc_Camera {
    saci_Vec3 position; ///< The position of the camera in world space.
    saci_Vec3 target;   ///< The point the camera is looking at.
    saci_Vec3 up;       ///< The up vector that defines the camera's orientation.

    float fov;         ///< The field of view angle (in degrees) for the camera.
    float aspectRatio; ///< The aspect ratio of the camera (width / height).
    float near;        ///< The distance to the near clipping plane.
    float far;         ///< The distance to the far clipping plane.
} sc_Camera;

/**
 * @brief Generates a default 3D camera.
 *
 * This function initializes and returns a default 3D camera with predefined
 * parameters. The camera is positioned at the origin and looks towards
 * the target point, with an upward orientation defined by the up vector.
 *
 * @details
 * - **Position:** (0.0f, 0.0f, 0.0f)
 * - **Target:** (0.0f, 0.0f, 0.0f)
 * - **Up Vector:** (0.0f, 1.0f, 0.0f)
 * - **Field of View (FOV):** 45.0f
 * - **Aspect Ratio:** 1600.0f / 900.0f
 * - **Near Plane:** 0.1f
 * - **Far Plane:** 100.0f
 *
 * @return The initialized 3D camera with default settings.
 */
sc_Camera sc_Camera_GetDefault3DCamera(void);

/**
 * @brief Generates a default 2D camera.
 *
 * This function initializes and returns a default 2D camera with predefined
 * parameters. The camera is positioned slightly behind the rendering layer to
 * ensure proper display of 2D content.
 *
 * @details
 * - **Position:** (0.0f, 0.0f, 1.0f)
 *   The Z value is set to 1.0f to position the camera "behind" the 2D rendering layer (at
 * Z=0).
 * - **Target:** (0.0f, 0.0f, 0.0f)
 * - **Up Vector:** (0.0f, 1.0f, 0.0f)
 * - **Field of View (FOV):** 45.0f
 * - **Aspect Ratio:** 1600.0f / 900.0f
 * - **Near Plane:** 0.1f
 * - **Far Plane:** 100.0f
 *
 * @return The initialized 2D camera with default settings.
 */
sc_Camera sc_Camera_GetDefault2DCamera(void);

#endif
