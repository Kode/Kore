#import <UIKit/UIKit.h>

#include <kore3/display.h>
#include <kore3/log.h>

void kore_display_init(void) {}

kore_display_mode kore_display_available_mode(int display, int mode) {
	kore_display_mode dm;
	dm.width = kore_window_width(0);
	dm.height = kore_window_height(0);
	dm.frequency = 60;
	dm.bits_per_pixel = 32;
	return dm;
}

int kore_display_count_available_modes(int display) {
	return 1;
}

bool kore_display_available(int display) {
	return true;
}

const char *kore_display_name(int display) {
	return "Display";
}

kore_display_mode kore_display_current_mode(int display) {
	kore_display_mode dm;
	dm.width = kore_window_width(0);
	dm.height = kore_window_height(0);
	dm.frequency = (int)[[UIScreen mainScreen] maximumFramesPerSecond];
	dm.bits_per_pixel = 32;
	return dm;
}

int kore_count_displays(void) {
	return 1;
}

int kore_primary_display(void) {
	return 0;
}
