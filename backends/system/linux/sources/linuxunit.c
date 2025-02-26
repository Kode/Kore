#ifndef _GNU_SOURCE
#define _GNU_SOURCE // memfd_create and mkostemp
#endif

#include <kore3/backend/funcs.h>

#include <dlfcn.h>
#include <stdio.h>

static void load_lib(void **lib, const char *name) {
	char libname[64];
	sprintf(libname, "lib%s.so", name);
	*lib = dlopen(libname, RTLD_LAZY);
	if (*lib != NULL) {
		return;
	}
	// Ubuntu and Fedora only ship libFoo.so.major by default, so look for those.
	for (int i = 0; i < 10; i++) {
		sprintf(libname, "lib%s.so.%i", name, i);
		*lib = dlopen(libname, RTLD_LAZY);
		if (*lib != NULL) {
			return;
		}
	}
}

#ifndef KORE_NO_WAYLAND
#include "display-wayland.c"
#include "system-wayland.c"
#include "window-wayland.c"
#endif

#ifndef KORE_NO_X11
#include "display-x11.c"
#include "system-x11.c"
#include "window-x11.c"
#endif

struct linux_procs procs = {0};

void kore_linux_init_procs() {
	if (procs.window_create != NULL) {
		return;
	}
#ifndef KORE_NO_WAYLAND
	if (kore_wayland_init()) {
		procs.handle_messages = kore_wayland_handle_messages;
		procs.shutdown = kore_wayland_shutdown;

		procs.window_create = kore_wayland_window_create;
		procs.window_width = kore_wayland_window_width;
		procs.window_height = kore_wayland_window_height;
		procs.window_x = kore_wayland_window_x;
		procs.window_y = kore_wayland_window_y;
		procs.window_destroy = kore_wayland_window_destroy;
		procs.window_change_mode = kore_wayland_window_change_mode;
		procs.window_get_mode = kore_wayland_window_get_mode;
		procs.window_set_title = kore_wayland_window_set_title;
		procs.window_display = kore_wayland_window_display;
		procs.window_move = kore_wayland_window_move;
		procs.window_resize = kore_wayland_window_resize;
		procs.window_show = kore_wayland_window_show;
		procs.window_hide = kore_wayland_window_hide;
		procs.count_windows = kore_wayland_count_windows;

		procs.mouse_can_lock = kore_wl_mouse_can_lock;
		procs.mouse_lock = kore_wl_mouse_lock;
		procs.mouse_unlock = kore_wl_mouse_unlock;
		procs.mouse_show = kore_wl_mouse_show;
		procs.mouse_hide = kore_wl_mouse_hide;
		procs.mouse_set_position = kore_wl_mouse_set_position;
		procs.mouse_get_position = kore_wl_mouse_get_position;
		procs.mouse_set_cursor = kore_wl_mouse_set_cursor;

		procs.display_init = kore_wayland_display_init;
		procs.display_available = kore_wayland_display_available;
		procs.display_available_mode = kore_wayland_display_available_mode;
		procs.display_count_available_modes = kore_wayland_display_count_available_modes;
		procs.display_current_mode = kore_wayland_display_current_mode;
		procs.display_name = kore_wayland_display_name;
		procs.display_primary = kore_wayland_display_primary;
		procs.count_displays = kore_wayland_count_displays;

		procs.copy_to_clipboard = kore_wayland_copy_to_clipboard;
#ifdef KORE_EGL
		procs.egl_get_display = kore_wayland_egl_get_display;
		procs.egl_get_native_window = kore_wayland_egl_get_native_window;
#endif
#ifdef KORE_VULKAN
		procs.vulkan_create_surface = kore_wayland_vulkan_create_surface;
		procs.vulkan_get_instance_extensions = kore_wayland_vulkan_get_instance_extensions;
		procs.vulkan_get_physical_device_presentation_support = kore_wayland_vulkan_get_physical_device_presentation_support;
#endif
	}
	else
#endif
#ifndef KORE_NO_X11
	    if (kore_x11_init()) {
		procs.handle_messages = kore_x11_handle_messages;
		procs.shutdown = kore_x11_shutdown;

		procs.window_create = kore_x11_window_create;
		procs.window_width = kore_x11_window_width;
		procs.window_height = kore_x11_window_height;
		procs.window_x = kore_x11_window_x;
		procs.window_y = kore_x11_window_y;
		procs.window_destroy = kore_x11_window_destroy;
		procs.window_change_mode = kore_x11_window_change_mode;
		procs.window_get_mode = kore_x11_window_get_mode;
		procs.window_set_title = kore_x11_window_set_title;
		procs.window_display = kore_x11_window_display;
		procs.window_move = kore_x11_window_move;
		procs.window_resize = kore_x11_window_resize;
		procs.window_show = kore_x11_window_show;
		procs.window_hide = kore_x11_window_hide;
		procs.count_windows = kore_x11_count_windows;

		procs.display_init = kore_x11_display_init;
		procs.display_available = kore_x11_display_available;
		procs.display_available_mode = kore_x11_display_available_mode;
		procs.display_count_available_modes = kore_x11_display_count_available_modes;
		procs.display_current_mode = kore_x11_display_current_mode;
		procs.display_name = kore_x11_display_name;
		procs.display_primary = kore_x11_display_primary;
		procs.count_displays = kore_x11_count_displays;

		procs.mouse_can_lock = kore_x11_mouse_can_lock;
		procs.mouse_lock = kore_x11_mouse_lock;
		procs.mouse_unlock = kore_x11_mouse_unlock;
		procs.mouse_show = kore_x11_mouse_show;
		procs.mouse_hide = kore_x11_mouse_hide;
		procs.mouse_set_position = kore_x11_mouse_set_position;
		procs.mouse_get_position = kore_x11_mouse_get_position;
		procs.mouse_set_cursor = kore_x11_mouse_set_cursor;

		procs.copy_to_clipboard = kore_x11_copy_to_clipboard;
#ifdef KORE_EGL
		procs.egl_get_display = kore_x11_egl_get_display;
		procs.egl_get_native_window = kore_x11_egl_get_native_window;
#endif
#ifdef KORE_VULKAN
		procs.vulkan_create_surface = kore_x11_vulkan_create_surface;
		procs.vulkan_get_instance_extensions = kore_x11_vulkan_get_instance_extensions;
		procs.vulkan_get_physical_device_presentation_support = kore_x11_vulkan_get_physical_device_presentation_support;
#endif
	}
	else
#endif
	{
		kore_log(KORE_LOG_LEVEL_ERROR, "Neither wayland nor X11 found.");
		exit(1);
	}
}

#include "display.c"
#ifndef __FreeBSD__
#include "gamepad.c"
#endif
#include "mouse.c"
#include "sound.c"
#include "system.c"
#include "video.c"
#include "window.c"
