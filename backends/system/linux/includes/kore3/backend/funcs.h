#pragma once

#include <kore3/display.h>
#include <kore3/global.h>
#include <kore3/window.h>
#ifdef KINC_EGL
#define EGL_NO_PLATFORM_SPECIFIC_TYPES
#include <EGL/egl.h>
#endif
#ifdef KINC_VULKAN
#include <vulkan/vulkan.h>
#endif

struct linux_procs {
	bool (*handle_messages)(void);
	void (*shutdown)(void);

	void (*display_init)(void);
	kore_display_mode (*display_available_mode)(int display, int mode);
	int (*display_count_available_modes)(int display);
	bool (*display_available)(int display_index);
	const char *(*display_name)(int display_index);
	kore_display_mode (*display_current_mode)(int display_index);
	int (*display_primary)(void);
	int (*count_displays)(void);

	int (*window_create)(kore_window_parameters *window_options, kore_framebuffer_parameters *framebuffer_options);
	void (*window_destroy)(int window_index);
	int (*window_display)(int window_index);
	void (*window_show)(int window_index);
	void (*window_hide)(int window_index);
	void (*window_set_title)(int window_index, const char *title);
	void (*window_change_mode)(int window_index, kore_window_mode mode);
	kore_window_mode (*window_get_mode)(int window_index);
	void (*window_move)(int window_index, int x, int y);
	void (*window_resize)(int window_index, int width, int height);
	int (*window_x)(int window_index);
	int (*window_y)(int window_index);
	int (*window_width)(int window_index);
	int (*window_height)(int window_index);
	int (*count_windows)(void);

	bool (*mouse_can_lock)(void);
	bool (*mouse_is_locked)(void);
	void (*mouse_lock)(int window);
	void (*mouse_unlock)(void);
	void (*mouse_show)(void);
	void (*mouse_hide)(void);
	void (*mouse_set_position)(int window, int x, int y);
	void (*mouse_get_position)(int window, int *x, int *y);
	void (*mouse_set_cursor)(int cursor);

	void (*copy_to_clipboard)(const char *text);
#ifdef KINC_EGL
	EGLDisplay (*egl_get_display)(void);
	EGLNativeWindowType (*egl_get_native_window)(EGLDisplay display, EGLConfig config, int window_index);
#endif
#ifdef KINC_VULKAN
	void (*vulkan_get_instance_extensions)(const char **extensions, int *count, int max);
	VkResult (*vulkan_create_surface)(VkInstance instance, int window_index, VkSurfaceKHR *surface);
	VkBool32 (*vulkan_get_physical_device_presentation_support)(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
#endif
};

extern struct linux_procs procs;

void kinc_linux_init_procs();
