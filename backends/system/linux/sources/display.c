#include <kore3/backend/funcs.h>

#include <kore3/display.h>

void kore_display_init() {
	static bool display_initialized = false;
	if (display_initialized) {
		return;
	}

	kinc_linux_init_procs();
	procs.display_init();
	display_initialized = true;
}

kore_display_mode kore_display_available_mode(int display, int mode) {
	return procs.display_available_mode(display, mode);
}

int kore_display_count_available_modes(int display) {
	return procs.display_count_available_modes(display);
}

bool kore_display_available(int display) {
	return procs.display_available(display);
}

const char *kore_display_name(int display) {
	return procs.display_name(display);
}

kore_display_mode kore_display_current_mode(int display) {
	return procs.display_current_mode(display);
}

int kore_primary_display(void) {
	return procs.display_primary();
}

int kore_count_displays(void) {
	return procs.count_displays();
}
