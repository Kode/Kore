#include <kinc/display.h>
#include <kinc/log.h>

void print_mode(const char *indent, kore_display_mode_t mode) {
	kore_log(KORE_LOG_LEVEL_INFO, "%sx: %i", indent, mode.x);
	kore_log(KORE_LOG_LEVEL_INFO, "%sy: %i", indent, mode.y);
	kore_log(KORE_LOG_LEVEL_INFO, "%swidth: %i", indent, mode.width);
	kore_log(KORE_LOG_LEVEL_INFO, "%sheight: %i", indent, mode.height);
	kore_log(KORE_LOG_LEVEL_INFO, "%spixels_per_inch: %i", indent, mode.pixels_per_inch);
	kore_log(KORE_LOG_LEVEL_INFO, "%sfrequency: %i", indent, mode.frequency);
	kore_log(KORE_LOG_LEVEL_INFO, "%sbits_per_pixel: %i", indent, mode.bits_per_pixel);
}

int kickstart(int argc, char **argv) {
	bool print_modes = false;
	if (argc > 1 && strcmp(argv[1], "--print-modes") == 0) {
		print_modes = true;
	}
	kore_display_init();
	int count = kore_count_displays();
	kore_log(KORE_LOG_LEVEL_INFO, "display count: %i", count);
	kore_log(KORE_LOG_LEVEL_INFO, "primary display: %i", kore_primary_display());
	for (int i = 0; i < count; i++) {
		bool available = kore_display_available(i);
		kore_log(KORE_LOG_LEVEL_INFO, "display %i:", i);
		kore_log(KORE_LOG_LEVEL_INFO, "\tavailable: %s", available ? "true" : "false");
		if (available) {
			kore_log(KORE_LOG_LEVEL_INFO, "\tname: %s", kore_display_name(i));
			kore_display_mode_t mode = kore_display_current_mode(i);
			print_mode("\t", mode);
			kore_log(KORE_LOG_LEVEL_INFO, "");
			int mode_count = kore_display_count_available_modes(i);
			kore_log(KORE_LOG_LEVEL_INFO, "\tavailable modes: %i", mode_count);
			if (print_modes) {
				for (int j = 0; j < mode_count; j++) {
					kore_display_mode_t mode = kore_display_available_mode(i, j);
					print_mode("\t\t", mode);
					kore_log(KORE_LOG_LEVEL_INFO, "");
				}
			}
		}
	}
	return 0;
}