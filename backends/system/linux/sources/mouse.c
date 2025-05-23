#include <kore3/backend/funcs.h>

#include <kore3/input/mouse.h>

void kore_internal_mouse_lock(int window) {
	procs.mouse_lock(window);
}

void kore_internal_mouse_unlock() {
	procs.mouse_unlock();
}

bool kore_mouse_can_lock(void) {
	return true;
}

bool _mouseHidden = false;

void kore_mouse_show() {
	procs.mouse_show();
}

void kore_mouse_hide() {
	procs.mouse_hide();
}

void kore_mouse_set_cursor(int cursorIndex) {
	procs.mouse_set_cursor(cursorIndex);
}

void kore_mouse_set_position(int window, int x, int y) {
	procs.mouse_set_position(window, x, y);
}

void kore_mouse_get_position(int window, int *x, int *y) {
	procs.mouse_get_position(window, x, y);
}
