#include <kore3/display.h>
#include <kore3/window.h>

static void (*resizeCallback)(int x, int y, void *data) = NULL;
static void *resizeCallbackData = NULL;

int kore_window_x(int window) {
	return 0;
}

int kore_window_y(int window) {
	return 0;
}

int kore_count_windows(void) {
	return 1;
}

void kore_window_resize(int window, int width, int height) {}

void kore_window_move(int window, int x, int y) {}

void kore_internal_change_framebuffer(int window, struct kore_framebuffer_parameters *frame);

void kore_window_change_framebuffer(int window, struct kore_framebuffer_parameters *frame) {
	kore_internal_change_framebuffer(0, frame);
}

void kore_internal_change_framebuffer(int window, struct kore_framebuffer_parameters *frame) {}

void kore_window_change_features(int window, int features) {}

void kore_window_change_mode(int window, kore_window_mode mode) {}

void kore_window_destroy(int window) {}

void kore_window_show(int window) {}

void kore_window_hide(int window) {}

void kore_window_set_title(int window, const char *title) {}

int kore_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	return 0;
}

void kore_window_set_resize_callback(int window, void (*callback)(int x, int y, void *data), void *data) {
	resizeCallback = callback;
	resizeCallbackData = data;
}

void kore_internal_call_resize_callback(int window, int width, int height) {
	if (resizeCallback != NULL) {
		resizeCallback(width, height, resizeCallbackData);
	}
}

void kore_window_set_ppi_changed_callback(int window, void (*callback)(int ppi, void *data), void *data) {}

void kore_window_set_close_callback(int window, bool (*callback)(void *), void *data) {}

kore_window_mode kore_window_get_mode(int window) {
	return KORE_WINDOW_MODE_FULLSCREEN;
}

int kore_window_display(int window) {
	return 0;
}
