#include <kore3/input/pen.h>

static void (*pen_press_callback)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)   = NULL;
static void (*pen_move_callback)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)    = NULL;
static void (*pen_release_callback)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/) = NULL;

static void (*eraser_press_callback)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)   = NULL;
static void (*eraser_move_callback)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)    = NULL;
static void (*eraser_release_callback)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/) = NULL;

void kore_pen_set_press_callback(void (*value)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)) {
	pen_press_callback = value;
}

void kore_pen_set_move_callback(void (*value)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)) {
	pen_move_callback = value;
}

void kore_pen_set_release_callback(void (*value)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)) {
	pen_release_callback = value;
}

void kore_eraser_set_press_callback(void (*value)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)) {
	eraser_press_callback = value;
}

void kore_eraser_set_move_callback(void (*value)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)) {
	eraser_move_callback = value;
}

void kore_eraser_set_release_callback(void (*value)(int /*window*/, int /*x*/, int /*y*/, float /*pressure*/)) {
	eraser_release_callback = value;
}

void kore_internal_pen_trigger_press(int window, int x, int y, float pressure) {
	if (pen_press_callback != NULL) {
		pen_press_callback(window, x, y, pressure);
	}
}

void kore_internal_pen_trigger_move(int window, int x, int y, float pressure) {
	if (pen_move_callback != NULL) {
		pen_move_callback(window, x, y, pressure);
	}
}

void kore_internal_pen_trigger_release(int window, int x, int y, float pressure) {
	if (pen_release_callback != NULL) {
		pen_release_callback(window, x, y, pressure);
	}
}

void kore_internal_eraser_trigger_press(int window, int x, int y, float pressure) {
	if (eraser_press_callback != NULL) {
		eraser_press_callback(window, x, y, pressure);
	}
}

void kore_internal_eraser_trigger_move(int window, int x, int y, float pressure) {
	if (eraser_move_callback != NULL) {
		eraser_move_callback(window, x, y, pressure);
	}
}

void kore_internal_eraser_trigger_release(int window, int x, int y, float pressure) {
	if (eraser_release_callback != NULL) {
		eraser_release_callback(window, x, y, pressure);
	}
}
