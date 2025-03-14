#include <kore3/input/mouse.h>

#include <kore3/window.h>

static void (*mouse_press_callback)(int /*window*/, int /*button*/, int /*x*/, int /*y*/, void * /*data*/)                      = NULL;
static void *mouse_press_callback_data                                                                                          = NULL;
static void (*mouse_release_callback)(int /*window*/, int /*button*/, int /*x*/, int /*y*/, void * /*data*/)                    = NULL;
static void *mouse_release_callback_data                                                                                        = NULL;
static void (*mouse_move_callback)(int /*window*/, int /*x*/, int /*y*/, int /*movementX*/, int /*movementY*/, void * /*data*/) = NULL;
static void *mouse_move_callback_data                                                                                           = NULL;
static void (*mouse_scroll_callback)(int /*window*/, int /*delta*/, void * /*data*/)                                            = NULL;
static void *mouse_scroll_callback_data                                                                                         = NULL;
static void (*mouse_enter_window_callback)(int /*window*/, void * /*data*/)                                                     = NULL;
static void *mouse_enter_window_callback_data                                                                                   = NULL;
static void (*mouse_leave_window_callback)(int /*window*/, void * /*data*/)                                                     = NULL;
static void *mouse_leave_window_callback_data                                                                                   = NULL;

void kore_mouse_set_press_callback(void (*value)(int /*window*/, int /*button*/, int /*x*/, int /*y*/, void * /*data*/), void *data) {
	mouse_press_callback      = value;
	mouse_press_callback_data = data;
}

void kore_mouse_set_release_callback(void (*value)(int /*window*/, int /*button*/, int /*x*/, int /*y*/, void * /*data*/), void *data) {
	mouse_release_callback      = value;
	mouse_release_callback_data = data;
}

void kore_mouse_set_move_callback(void (*value)(int /*window*/, int /*x*/, int /*y*/, int /*movement_x*/, int /*movement_y*/, void * /*data*/), void *data) {
	mouse_move_callback      = value;
	mouse_move_callback_data = data;
}

void kore_mouse_set_scroll_callback(void (*value)(int /*window*/, int /*delta*/, void * /*data*/), void *data) {
	mouse_scroll_callback      = value;
	mouse_scroll_callback_data = data;
}

void kore_mouse_set_enter_window_callback(void (*value)(int /*window*/, void * /*data*/), void *data) {
	mouse_enter_window_callback      = value;
	mouse_enter_window_callback_data = data;
}

void kore_mouse_set_leave_window_callback(void (*value)(int /*window*/, void * /*data*/), void *data) {
	mouse_leave_window_callback      = value;
	mouse_leave_window_callback_data = data;
}

void kore_internal_mouse_trigger_release(int window, int button, int x, int y) {
	if (mouse_release_callback != NULL) {
		mouse_release_callback(window, button, x, y, mouse_release_callback_data);
	}
}

void kore_internal_mouse_trigger_scroll(int window, int delta) {
	if (mouse_scroll_callback != NULL) {
		mouse_scroll_callback(window, delta, mouse_scroll_callback_data);
	}
}

void kore_internal_mouse_trigger_enter_window(int window) {
	if (mouse_enter_window_callback != NULL) {
		mouse_enter_window_callback(window, mouse_enter_window_callback_data);
	}
}

void kore_internal_mouse_trigger_leave_window(int window) {
	if (mouse_leave_window_callback != NULL) {
		mouse_leave_window_callback(window, mouse_leave_window_callback_data);
	}
}

void kore_internal_mouse_window_activated(int window) {
	if (kore_mouse_is_locked()) {
		kore_internal_mouse_lock(window);
	}
}
void kore_internal_mouse_window_deactivated(int window) {
	if (kore_mouse_is_locked()) {
		kore_internal_mouse_unlock();
	}
}

// TODO: handle state per window
static bool moved         = false;
static bool locked        = false;
static int  preLockWindow = 0;
static int  preLockX      = 0;
static int  preLockY      = 0;
static int  centerX       = 0;
static int  centerY       = 0;
static int  lastX         = 0;
static int  lastY         = 0;

void kore_internal_mouse_trigger_press(int window, int button, int x, int y) {
	lastX = x;
	lastY = y;
	if (mouse_press_callback != NULL) {
		mouse_press_callback(window, button, x, y, mouse_press_callback_data);
	}
}

void kore_internal_mouse_trigger_move(int window, int x, int y) {
	int movementX = 0;
	int movementY = 0;
	if (kore_mouse_is_locked()) {
		movementX = x - centerX;
		movementY = y - centerY;
		if (movementX != 0 || movementY != 0) {
			kore_mouse_set_position(window, centerX, centerY);
			x = centerX;
			y = centerY;
		}
	}
	else if (moved) {
		movementX = x - lastX;
		movementY = y - lastY;
	}
	moved = true;

	lastX = x;
	lastY = y;
	if (mouse_move_callback != NULL && (movementX != 0 || movementY != 0)) {
		mouse_move_callback(window, x, y, movementX, movementY, mouse_move_callback_data);
	}
}

bool kore_mouse_is_locked(void) {
	return locked;
}

void kore_mouse_lock(int window) {
	if (!kore_mouse_can_lock()) {
		return;
	}
	locked = true;
	kore_internal_mouse_lock(window);
	kore_mouse_get_position(window, &preLockX, &preLockY);
	centerX = kore_window_width(window) / 2;
	centerY = kore_window_height(window) / 2;
	kore_mouse_set_position(window, centerX, centerY);
}

void kore_mouse_unlock(void) {
	if (!kore_mouse_can_lock()) {
		return;
	}
	moved  = false;
	locked = false;
	kore_internal_mouse_unlock();
	kore_mouse_set_position(preLockWindow, preLockX, preLockY);
}
