#include <kore3/audio/audio.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/window.h>
#include <stdio.h>
#include <stdlib.h>

int kore_init(const char *name, int width, int height, kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	kore_window_parameters defaultWin;
	if (win == NULL) {
		kore_window_options_set_defaults(&defaultWin);
		win = &defaultWin;
	}
	kore_framebuffer_parameters defaultFrame;
	if (frame == NULL) {
		kore_framebuffer_options_set_defaults(&defaultFrame);
		frame = &defaultFrame;
	}
	win->width  = width;
	win->height = height;

	return 0;
}

bool kore_internal_handle_messages() {
	return true;
}

void kore_set_keep_screen_on(bool on) {}

double kore_frequency(void) {
	return 1000.0;
}

kore_ticks kore_timestamp(void) {
	return (kore_ticks)(js_time());
}

double kore_time(void) {
	return js_time() / 1000.0;
}

int kore_cpu_cores(void) {
	return 4;
}

int kore_hardware_threads(void) {
	return 4;
}

void kore_internal_shutdown(void) {}

extern int kickstart(int argc, char **argv);

void _start(void) {
	kickstart(0, NULL);
}
