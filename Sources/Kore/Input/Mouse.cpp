#include "Mouse.h"

#include <kinc/input/mouse.h>

using namespace Kore;

namespace {
	Mouse mouse;
	bool initialized = false;

	void move(int window, int x, int y, int movementX, int movementY, void *data) {
		if (mouse.Move != nullptr) {
			mouse.Move(window, x, y, movementX, movementY);
		}
	}

	void press(int window, int button, int x, int y, void *data) {
		if (mouse.Press != nullptr) {
			mouse.Press(window, button, x, y);
		}
	}

	void release(int window, int button, int x, int y, void *data) {
		if (mouse.Release != nullptr) {
			mouse.Release(window, button, x, y);
		}
	}

	void scroll(int window, int delta, void *data) {
		if (mouse.Scroll != nullptr) {
			mouse.Scroll(window, delta);
		}
	}

	void leave(int window, void *data) {
		if (mouse.Leave != nullptr) {
			mouse.Leave(window);
		}
	}
}

Mouse *Mouse::the() {
	if (!initialized) {
		kinc_mouse_set_move_callback(move, nullptr);
		kinc_mouse_set_press_callback(press, nullptr);
		kinc_mouse_set_release_callback(release, nullptr);
		kinc_mouse_set_scroll_callback(scroll, nullptr);
		kinc_mouse_set_leave_window_callback(leave, nullptr);
		initialized = true;
	}
	return &mouse;
}

Mouse::Mouse() : Move(nullptr), Press(nullptr), Release(nullptr), Scroll(nullptr), Leave(nullptr) {}

bool Mouse::canLock() {
	return kinc_mouse_can_lock();
}

bool Mouse::isLocked() {
	return kinc_mouse_is_locked();
}

void Mouse::lock(int window) {
	kinc_mouse_lock(window);
}

void Mouse::unlock() {
	kinc_mouse_unlock();
}

void Mouse::show(bool truth) {
	if (truth) {
		kinc_mouse_show();
	}
	else {
		kinc_mouse_hide();
	}
}

void Mouse::setCursor(int cursor) {
	kinc_mouse_set_cursor(cursor);
}

void Mouse::setPosition(int window, int x, int y) {
	kinc_mouse_set_position(window, x, y);
}

void Mouse::getPosition(int window, int &x, int &y) {
	kinc_mouse_get_position(window, &x, &y);
}
