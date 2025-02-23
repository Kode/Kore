#ifndef KORE_INPUT_ROTATION_HEADER
#define KORE_INPUT_ROTATION_HEADER

#include <kinc/global.h>

/*! \file rotation.h
    \brief Provides support for rotation-sensors.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Sets the rotation-callback which is called when the device is rotated. Act quickly when this is called for a desktop-system.
/// </summary>
/// <param name="value">The callback</param>
KORE_FUNC void kore_rotation_set_callback(void (*value)(float /*x*/, float /*y*/, float /*z*/));

void kore_internal_on_rotation(float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif
