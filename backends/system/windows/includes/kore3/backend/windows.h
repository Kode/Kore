#ifndef KORE_WINDOWS_HEADER
#define KORE_WINDOWS_HEADER

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HMONITOR__;
struct HWND__;

int            kore_windows_get_display_for_monitor(struct HMONITOR__ *monitor);
bool           kore_windows_set_display_mode(int display_index, int width, int height, int bpp, int frequency);
void           kore_windows_restore_display(int display_index);
void           kore_windows_restore_displays();
void           kore_windows_hide_windows();
void           kore_windows_destroy_windows();
struct HWND__ *kore_windows_window_handle(int window_index);
int            kore_windows_window_index_from_hwnd(struct HWND__ *handle);
int            kore_windows_manual_width(int window);
int            kore_windows_manual_height(int window);

#ifdef __cplusplus
}
#endif

#endif
