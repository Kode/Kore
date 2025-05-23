#include "kore3/input/keyboard.h"
#include "kore3/log.h"
#include <kore3/graphics4/graphics.h>
#include <kore3/input/mouse.h>
#include <kore3/system.h>

#include <assert.h>
#include <stdlib.h>

static void update(void *data) {
	kore_g4_begin(0);
	kore_g4_clear(KORE_G4_CLEAR_COLOR, 0, 0.0f, 0);
	kore_g4_end(0);
	kore_g4_swap_buffers();
}

static void mouse_enter_window(int window, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "mouse_enter_window -- window: %i", window);
}
static void mouse_leave_window(int window, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "mouse_leave_window -- window: %i", window);
}
static void mouse_press(int window, int button, int x, int y, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "mouse_press -- window: %i, button: %i, x: %i, y: %i", window, button, x, y);
}
static void mouse_release(int window, int button, int x, int y, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "mouse_release -- window: %i, button: %i, x: %i, y: %i", window, button, x, y);
}
static void mouse_move(int window, int x, int y, int mov_x, int mov_y, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "mouse_move -- window: %i, x: %i, y: %i, movement_x: %i, movement_y: %i", window, x, y, mov_x, mov_y);
}
static void mouse_scroll(int window, int delta, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "mouse_scroll -- window: %i, delta: %i", window, delta);
}
static void key_down(int key, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "key_down -- key: %i", key);
}
static void key_up(int key, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "key_up -- key: %i", key);
}
static void key_press(unsigned character, void *data) {
	char str[5] = {0};
	wctomb(str, character);
	kore_log(KORE_LOG_LEVEL_INFO, "key_press -- char: %s", str);
}

int kickstart(int argc, char **argv) {
	kore_init("Shader", 1024, 768, NULL, NULL);
	kore_set_update_callback(update, NULL);

	kore_mouse_set_enter_window_callback(mouse_enter_window, NULL);
	kore_mouse_set_leave_window_callback(mouse_leave_window, NULL);
	kore_mouse_set_press_callback(mouse_press, NULL);
	kore_mouse_set_release_callback(mouse_release, NULL);
	kore_mouse_set_move_callback(mouse_move, NULL);
	kore_mouse_set_scroll_callback(mouse_scroll, NULL);

	kore_keyboard_set_key_down_callback(key_down, NULL);
	kore_keyboard_set_key_up_callback(key_up, NULL);
	kore_keyboard_set_key_press_callback(key_press, NULL);

	kore_start();

	return 0;
}
