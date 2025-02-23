#ifndef KORE_DISPLAY_HEADER
#define KORE_DISPLAY_HEADER

#include <kore3/global.h>

#include <stdbool.h>

/*! \file display.h
    \brief Provides information for the active displays.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_display_mode {
	int x;
	int y;
	int width;
	int height;
	int pixels_per_inch;
	int frequency;
	int bits_per_pixel;
} kore_display_mode;

/// <summary>
/// Allows retrieval of display values prior to the kore_init call.
/// </summary>
KORE_FUNC void kore_display_init(void);

/// <summary>
/// Retrieves the index of the primary display.
/// </summary>
/// <returns>The index of the primary display</returns>
KORE_FUNC int kore_primary_display(void);

/// <summary>
/// Retrieves the number of displays connected to the system.
/// </summary>
/// <remarks>
/// All indices from 0 to kore_count_displays() - 1 are legal display indices.
/// </remarks>
/// <returns>The number of displays connected to the system</returns>
KORE_FUNC int kore_count_displays(void);

/// <summary>
/// Checks whether the display index points to an available display.
/// </summary>
/// <param name="display_index">Index of the display to check</param>
/// <returns>
/// Returns true if the index points to an available display,
/// false otherwise
/// </returns>
KORE_FUNC bool kore_display_available(int display_index);

/// <summary>
/// Retrieves the system name of a display.
/// </summary>
/// <param name="display_index">Index of the display to retrieve the name from</param>
/// <returns>The system name of the display</returns>
KORE_FUNC const char *kore_display_name(int display_index);

/// <summary>
/// Retrieves the current mode of a display.
/// </summary>
/// <param name="display_index">Index of the display to retrieve the mode from</param>
/// <returns>The current display mode</returns>
KORE_FUNC kore_display_mode kore_display_current_mode(int display_index);

/// <summary>
/// Retrieves the number of available modes of a display.
/// </summary>
/// <param name="display_index">Index of the display to retrieve the modes count from</param>
/// <returns>The number of available modes of the display</returns>
KORE_FUNC int kore_display_count_available_modes(int display_index);

/// <summary>
/// Retrieves a specific mode of a display.
/// </summary>
/// <param name="display_index">Index of the display to retrieve the mode from</param>
/// <param name="mode_index">Index of the mode to retrieve</param>
/// <returns>The display mode</returns>
KORE_FUNC kore_display_mode kore_display_available_mode(int display_index, int mode_index);

#ifdef __cplusplus
}
#endif

#endif
