#include <kore3/display.h>
//#include <kore3/graphics4/graphics.h>
#include <kore3/window.h>

#include <string.h>

int kore_internal_window_width = 0;
int kore_internal_window_height = 0;
kore_window_mode kore_internal_window_mode = KORE_WINDOW_MODE_WINDOW;

int kore_count_windows(void) {
	return 1;
}

int kore_window_x(int window_index) {
	return 0;
}

int kore_window_y(int window_index) {
	return 0;
}

int kore_window_width(int window_index) {
	return kore_internal_window_width;
}

int kore_window_height(int window_index) {
	return kore_internal_window_height;
}

void kore_window_resize(int window_index, int width, int height) {}

void kore_window_move(int window_index, int x, int y) {}

void kore_window_change_framebuffer(int window_index, kore_framebuffer_parameters *frame) {
	//**kinc_g4_changeFramebuffer(0, frame);
}

void kore_window_change_features(int window_index, int features) {}

// In HTML5 fullscreen is activable only from user input.
void kore_window_change_mode(int window_index, kore_window_mode mode) {
	if (mode == KORE_WINDOW_MODE_FULLSCREEN || mode == KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
		if (kore_internal_window_mode == KORE_WINDOW_MODE_FULLSCREEN || kore_internal_window_mode == KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
			kore_internal_window_mode = mode;
			return;
		}
		// TODO: call js Fullscreen API
		kore_internal_window_mode = mode;
	}
	else {
		if (mode == kore_internal_window_mode) {
			return;
		}
		// TODO: call js Fullscreen API
		kore_internal_window_mode = mode;
	}
}

void kore_window_destroy(int window_index) {}

void kore_window_show(int window_index) {}

void kore_window_hide(int window_index) {}

// TODO: change browser title.
void kore_window_set_title(int window_index, const char *title) {}

int kore_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	return 0;
}

void kore_window_set_resize_callback(int window_index, void (*callback)(int x, int y, void *data), void *data) {}

void kore_window_set_ppi_changed_callback(int window_index, void (*callback)(int ppi, void *data), void *data) {}

void kore_window_set_close_callback(int window, bool (*callback)(void *), void *data) {}

kore_window_mode kore_window_get_mode(int window_index) {
	return kore_internal_window_mode;
}
