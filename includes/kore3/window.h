#ifndef KORE_WINDOW_HEADER
#define KORE_WINDOW_HEADER

#include <kore3/global.h>

#include <stdbool.h>

/*! \file window.h
    \brief Provides functionality for creating and handling windows.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_framebuffer_parameters {
	int frequency;
	bool vertical_sync;
	int color_bits;
	int depth_bits;
	int stencil_bits;
	int samples_per_pixel;
} kore_framebuffer_parameters;

typedef enum {
	KORE_WINDOW_MODE_WINDOW,
	KORE_WINDOW_MODE_FULLSCREEN,
	KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN // Only relevant for Windows
} kore_window_mode;

#define KORE_WINDOW_FEATURE_RESIZEABLE 1
#define KORE_WINDOW_FEATURE_MINIMIZABLE 2
#define KORE_WINDOW_FEATURE_MAXIMIZABLE 4
#define KORE_WINDOW_FEATURE_BORDERLESS 8
#define KORE_WINDOW_FEATURE_ON_TOP 16

typedef struct kore_window_parameters {
	const char *title;

	int x;
	int y;
	int width;
	int height;
	int display_index;

	bool visible;
	int window_features;
	kore_window_mode mode;
} kore_window_parameters;

/// <summary>
/// Creates a window.
/// </summary>
/// <returns>The id of the created window</returns>
KORE_FUNC int kore_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame);

/// <summary>
/// Closes and destroys a window.
/// </summary>
KORE_FUNC void kore_window_destroy(int window);

KORE_FUNC void kore_window_options_set_defaults(kore_window_parameters *win);

KORE_FUNC void kore_framebuffer_options_set_defaults(kore_framebuffer_parameters *frame);

/// <summary>
/// Counts all windows the program created, including the initial window created by kore_init.
/// </summary>
/// <returns>The number of windows</returns>
KORE_FUNC int kore_count_windows(void);

/// <summary>
/// Resizes a window.
/// </summary>
KORE_FUNC void kore_window_resize(int window, int width, int height);

/// <summary>
/// Moves a window.
/// </summary>
KORE_FUNC void kore_window_move(int window, int x, int y);

/// <summary>
/// Switches between window and fullscreen-modes.
/// </summary>
KORE_FUNC void kore_window_change_mode(int window, kore_window_mode mode);

/// <summary>
/// Applies an or-ed combination of KINC_WINDOW_FEATURE values.
/// </summary>
KORE_FUNC void kore_window_change_features(int window, int features);

/// <summary>
/// Changes the framebuffer-options of a window.
/// </summary>
KORE_FUNC void kore_window_change_framebuffer(int window, kore_framebuffer_parameters *frame);

/// <summary>
/// Returns the x position of a window.
/// </summary>
/// <returns>The x-position</returns>
KORE_FUNC int kore_window_x(int window);

/// <summary>
/// Returns the y position of a window.
/// </summary>
/// <returns>The y-position</returns>
KORE_FUNC int kore_window_y(int window);

/// <summary>
/// Returns the width of a window.
/// </summary>
/// <returns>The window-width</returns>
KORE_FUNC int kore_window_width(int window);

/// <summary>
/// Returns the height of a window.
/// </summary>
/// <returns>The window-height</returns>
KORE_FUNC int kore_window_height(int window);

/// <summary>
/// Returns the id of the display the window currently is on.
/// </summary>
/// <returns>The display-id</returns>
KORE_FUNC int kore_window_display(int window);

/// <summary>
/// Returns the current window-mode.
/// </summary>
/// <returns>The window-mode</returns>
KORE_FUNC kore_window_mode kore_window_get_mode(int window);

/// <summary>
/// Makes the window visible.
/// </summary>
KORE_FUNC void kore_window_show(int window);

/// <summary>
/// Hides a window.
/// </summary>
KORE_FUNC void kore_window_hide(int window);

/// <summary>
/// Sets the title of a window.
/// </summary>
KORE_FUNC void kore_window_set_title(int window, const char *title);

/// <summary>
/// Sets a resize callback that's called whenever the window is resized.
/// </summary>
KORE_FUNC void kore_window_set_resize_callback(int window, void (*callback)(int x, int y, void *data), void *data);

/// <summary>
/// Sets a PPI callback that's called whenever the window moves to a display that uses a different PPI-setting.
/// </summary>
KORE_FUNC void kore_window_set_ppi_changed_callback(int window, void (*callback)(int ppi, void *data), void *data);

/// <summary>
/// Sets a close callback that's called when the window is about to close.
/// Returning false from the callback tries to stop the window from closing.
/// </summary>
KORE_FUNC void kore_window_set_close_callback(int window, bool (*callback)(void *data), void *data);

/// <summary>
/// Returns whether the window is vsynced or not.
/// </summary>
/// <returns>Whether the window is vsynced or not</returns>
KORE_FUNC bool kore_window_vsynced(int window);

void kore_internal_call_resize_callback(int window, int width, int height);
void kore_internal_call_ppi_changed_callback(int window, int ppi);
bool kore_internal_call_close_callback(int window);

#ifdef __cplusplus
}
#endif

#endif
