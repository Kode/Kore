#include <kore3/input/gamepad.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/system.h>
#include <kore3/video.h>

static int mouseX, mouseY;
static bool keyboardShown = false;

void Kinc_Mouse_GetPosition(int window, int *x, int *y) {
	*x = mouseX;
	*y = mouseY;
}

void kore_keyboard_show(void) {
	keyboardShown = true;
}

void kore_keyboard_hide(void) {
	keyboardShown = false;
}

bool kore_keyboard_active(void) {
	return keyboardShown;
}

void kore_vibrate(int ms) {}

const char *kore_system_id(void) {
	return "macOS";
}

static const char *videoFormats[] = {"ogv", NULL};

const char **kore_video_formats(void) {
	return videoFormats;
}

void kore_set_keep_screen_on(bool on) {}

#include <mach/mach_time.h>

double kore_frequency(void) {
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	return (double)info.denom / (double)info.numer / 1e-9;
}

kore_ticks kore_timestamp(void) {
	return mach_absolute_time();
}

void kore_login(void) {}

void kore_unlock_achievement(int id) {}

void kore_gamepad_set_count(int count) {}

bool kore_gamepad_connected(int num) {
	return true;
}

void kore_gamepad_rumble(int gamepad, float left, float right) {}
