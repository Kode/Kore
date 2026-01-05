#include <kore3/input/mouse.h>

void kore_internal_mouse_lock(int window) {}

void kore_internal_mouse_unlock(void) {}

bool kore_mouse_can_lock(void) {
	return false;
}

void kore_mouse_show() {}

void kore_mouse_hide() {}

void kore_mouse_set_position(int window, int x, int y) {}

void kore_mouse_get_position(int window, int *x, int *y) {}
