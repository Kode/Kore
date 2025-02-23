#include <kore3/input/mouse.h>

#include <kore3/backend/windows.h>
#include <kore3/system.h>
#include <kore3/window.h>

void kore_internal_mouse_lock(int window) {
	kore_mouse_hide();
	HWND handle = kore_windows_window_handle(window);
	SetCapture(handle);
	RECT rect;
	GetWindowRect(handle, &rect);
	ClipCursor(&rect);
}

void kore_internal_mouse_unlock(void) {
	kore_mouse_show();
	ReleaseCapture();
	ClipCursor(NULL);
}

bool kore_mouse_can_lock(void) {
	return true;
}

void kore_mouse_show() {
	// Work around the internal counter of ShowCursor
	while (ShowCursor(true) < 0) {
	}
}

void kore_mouse_hide() {
	// Work around the internal counter of ShowCursor
	while (ShowCursor(false) >= 0) {
	}
}

void kore_mouse_set_position(int window, int x, int y) {
	POINT point;
	point.x = x;
	point.y = y;
	ClientToScreen(kore_windows_window_handle(window), &point);
	SetCursorPos(point.x, point.y);
}

void kore_mouse_get_position(int window, int *x, int *y) {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(kore_windows_window_handle(window), &point);
	*x = point.x;
	*y = point.y;
}
