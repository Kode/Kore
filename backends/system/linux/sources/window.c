#include <kore3/backend/funcs.h>

#include <kore3/display.h>
// #include <kore3/graphics4/graphics.h>
#include <kore3/window.h>

#include <string.h>

#ifdef KORE_EGL
EGLDisplay kore_egl_get_display() {
	return procs.egl_get_display();
}
EGLNativeWindowType kore_egl_get_native_window(EGLDisplay display, EGLConfig config, int window_index) {
	return procs.egl_get_native_window(display, config, window_index);
}
#endif

#ifdef KORE_VULKAN
void kore_vulkan_get_instance_extensions(const char **extensions, int *count, int max) {
	procs.vulkan_get_instance_extensions(extensions, count, max);
}

VkBool32 kore_vulkan_get_physical_device_presentation_support(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
	return procs.vulkan_get_physical_device_presentation_support(physicalDevice, queueFamilyIndex);
}
VkResult kore_vulkan_create_surface(VkInstance instance, int window_index, VkSurfaceKHR *surface) {
	return procs.vulkan_create_surface(instance, window_index, surface);
}
#endif

int kore_count_windows(void) {
	return procs.count_windows();
}

int kore_window_x(int window_index) {
	return procs.window_x(window_index);
}

int kore_window_y(int window_index) {
	return procs.window_y(window_index);
}

int kore_window_width(int window_index) {
	return procs.window_width(window_index);
}

int kore_window_height(int window_index) {
	return procs.window_height(window_index);
}

void kore_window_resize(int window_index, int width, int height) {
	procs.window_resize(window_index, width, height);
}

void kore_window_move(int window_index, int x, int y) {
	procs.window_move(window_index, x, y);
}

void kore_window_change_framebuffer(int window_index, kore_framebuffer_parameters *frame) {}

void kore_window_change_features(int window_index, int features) {}

void kore_window_change_mode(int window_index, kore_window_mode mode) {
	procs.window_change_mode(window_index, mode);
}

int kore_window_display(int window_index) {
	return procs.window_display(window_index);
}

void kore_window_destroy(int window_index) {
	// kore_g4_internal_destroy_window(window_index);
	procs.window_destroy(window_index);
}

void kore_window_show(int window_index) {
	procs.window_show(window_index);
}

void kore_window_hide(int window_index) {
	procs.window_hide(window_index);
}

void kore_window_set_title(int window_index, const char *title) {
	procs.window_set_title(window_index, title);
}

int kore_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	int index = procs.window_create(win, frame);
	// kore_g4_internal_init_window(index, frame->depth_bits, frame->stencil_bits, frame->vertical_sync);
	return index;
}

static struct {
	void (*resize_callback)(int width, int height, void *data);
	void *resize_data;
	void (*ppi_callback)(int ppi, void *data);
	void *ppi_data;
	bool (*close_callback)(void *data);
	void *close_data;
} kore_internal_window_callbacks[16];

void kore_window_set_resize_callback(int window_index, void (*callback)(int width, int height, void *data), void *data) {
	kore_internal_window_callbacks[window_index].resize_callback = callback;
	kore_internal_window_callbacks[window_index].resize_data     = data;
}

void kore_internal_call_resize_callback(int window_index, int width, int height) {
	if (kore_internal_window_callbacks[window_index].resize_callback != NULL) {
		kore_internal_window_callbacks[window_index].resize_callback(width, height, kore_internal_window_callbacks[window_index].resize_data);
	}
}

void kore_window_set_ppi_changed_callback(int window_index, void (*callback)(int ppi, void *data), void *data) {
	kore_internal_window_callbacks[window_index].ppi_callback = callback;
	kore_internal_window_callbacks[window_index].ppi_data     = data;
}

void kore_internal_call_ppi_changed_callback(int window_index, int ppi) {
	if (kore_internal_window_callbacks[window_index].ppi_callback != NULL) {
		kore_internal_window_callbacks[window_index].ppi_callback(ppi, kore_internal_window_callbacks[window_index].resize_data);
	}
}

void kore_window_set_close_callback(int window_index, bool (*callback)(void *data), void *data) {
	kore_internal_window_callbacks[window_index].close_callback = callback;
	kore_internal_window_callbacks[window_index].close_data     = data;
}

bool kore_internal_call_close_callback(int window_index) {
	if (kore_internal_window_callbacks[window_index].close_callback != NULL) {
		return kore_internal_window_callbacks[window_index].close_callback(kore_internal_window_callbacks[window_index].close_data);
	}
	else {
		return true;
	}
}

kore_window_mode kore_window_get_mode(int window_index) {
	return procs.window_get_mode(window_index);
}
