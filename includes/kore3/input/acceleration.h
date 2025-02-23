#ifndef KORE_INPUT_ACCELERATION_HEADER
#define KORE_INPUT_ACCELERATION_HEADER

#include <kore3/global.h>

/*! \file acceleration.h
    \brief Provides data provided by acceleration-sensors.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Sets the acceleration-callback which is called with measured acceleration-data in three dimensions.
/// </summary>
/// <param name="value">The callback</param>
KORE_FUNC void kore_acceleration_set_callback(void (*value)(float /*x*/, float /*y*/, float /*z*/));

void kore_internal_on_acceleration(float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif
