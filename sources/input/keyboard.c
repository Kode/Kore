#include <kore3/input/keyboard.h>

static void (*keyboard_key_down_callback)(int /*key_code*/, void * /*data*/)        = NULL;
static void *keyboard_key_down_callback_data                                        = NULL;
static void (*keyboard_key_up_callback)(int /*key_code*/, void * /*data*/)          = NULL;
static void *keyboard_key_up_callback_data                                          = NULL;
static void (*keyboard_key_press_callback)(unsigned /*character*/, void * /*data*/) = NULL;
static void *keyboard_key_press_callback_data                                       = NULL;

void kore_keyboard_set_key_down_callback(void (*value)(int /*key_code*/, void * /*data*/), void *data) {
	keyboard_key_down_callback      = value;
	keyboard_key_down_callback_data = data;
}

void kore_keyboard_set_key_up_callback(void (*value)(int /*key_code*/, void * /*data*/), void *data) {
	keyboard_key_up_callback      = value;
	keyboard_key_up_callback_data = data;
}

void kore_keyboard_set_key_press_callback(void (*value)(unsigned /*character*/, void * /*data*/), void *data) {
	keyboard_key_press_callback      = value;
	keyboard_key_press_callback_data = data;
}

void kore_internal_keyboard_trigger_key_down(int key_code) {
	if (keyboard_key_down_callback != NULL) {
		keyboard_key_down_callback(key_code, keyboard_key_down_callback_data);
	}
}

void kore_internal_keyboard_trigger_key_up(int key_code) {
	if (keyboard_key_up_callback != NULL) {
		keyboard_key_up_callback(key_code, keyboard_key_up_callback_data);
	}
}

void kore_internal_keyboard_trigger_key_press(unsigned character) {
	if (keyboard_key_press_callback != NULL) {
		keyboard_key_press_callback(character, keyboard_key_press_callback_data);
	}
}
