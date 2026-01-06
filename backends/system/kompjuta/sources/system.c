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
	return 0;
}

int kore_cpu_cores(void) {
	return 4;
}

int kore_hardware_threads(void) {
	return 4;
}

void kore_internal_shutdown(void) {}

const char *kore_internal_save_path() {
	return "";
}

extern int kickstart(int argc, char **argv);


void kore_kompjuta_init_heap(void);

void start(void) {
	kore_kompjuta_init_heap();
	kickstart(0, NULL);
}

extern uint8_t __stack_top[];

__attribute__((naked, noreturn))
void _start(void) {
	__asm__ volatile(
		"la sp, __stack_top\n"
		"call start\n"
		"1: j 1b\n"
    );
}
