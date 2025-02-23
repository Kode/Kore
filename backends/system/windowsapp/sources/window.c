#include <kore3/display.h>
#include <kore3/graphics4/graphics.h>
#include <kore3/window.h>

int kore_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	return 0;
}

void kore_window_destroy(int window_index) {}

int kore_count_windows(void) {
	return 1;
}

void kore_window_resize(int window_index, int width, int height) {}

void kore_window_move(int window_index, int x, int y) {}

void kore_window_change_mode(int window_index, kore_window_mode mode) {}

void kore_window_change_features(int window_index, int features) {}

void kore_internal_change_framebuffer(int window, kore_framebuffer_parameters *frame);

void kore_window_change_framebuffer(int window_index, kore_framebuffer_parameters *frame) {
	kore_internal_change_framebuffer(0, frame);
}

int kore_window_x(int window_index) {
	return 0;
}

int kore_window_y(int window_index) {
	return 0;
}

int kore_window_display(int window_index) {
	return 0;
}

kore_window_mode kore_window_get_mode(int window_index) {
	return KORE_WINDOW_MODE_WINDOW;
}

void kore_window_show(int window_index) {}

void kore_window_hide(int window_index) {}

void kore_window_set_title(int window_index, const char *title) {}

void kore_window_set_resize_callback(int window_index, void (*callback)(int x, int y, void *data), void *data) {}

void kore_window_set_ppi_changed_callback(int window_index, void (*callback)(int ppi, void *data), void *data) {}

void kore_window_set_close_callback(int window, bool (*callback)(void *), void *data) {}
