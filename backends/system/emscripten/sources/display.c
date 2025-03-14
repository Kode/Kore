#include <kore3/display.h>

void kore_display_init(void) {}

int kore_primary_display(void) {
	return 0;
}

int kore_count_displays(void) {
	return 1;
}

bool kore_display_available(int display_index) {
	return false;
}

const char *kore_display_name(int display_index) {
	return "Browser";
}

kore_display_mode kore_display_current_mode(int display_index) {
	kore_display_mode mode;
	mode.x               = 0;
	mode.y               = 0;
	mode.width           = 800;
	mode.height          = 600;
	mode.pixels_per_inch = 96;
	mode.frequency       = 60;
	mode.bits_per_pixel  = 32;
	return mode;
}

int kore_display_count_available_modes(int display_index) {
	return 1;
}

kore_display_mode kore_display_available_mode(int display_index, int mode_index) {
	kore_display_mode mode;
	mode.x               = 0;
	mode.y               = 0;
	mode.width           = 800;
	mode.height          = 600;
	mode.pixels_per_inch = 96;
	mode.frequency       = 60;
	mode.bits_per_pixel  = 32;
	return mode;
}
