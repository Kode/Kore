#include <kore3/input/gamepad.h>

void kore_gamepad_set_count(int count) {}

const char *kore_gamepad_vendor(int gamepad) {
	return "None";
}

const char *kore_gamepad_product_name(int gamepad) {
	return "Gamepad";
}

bool kore_gamepad_connected(int gamepad) {
	return false;
}

void kore_gamepad_rumble(int gamepad, float left, float right) {}
