#include <kore3/audio/audio.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/window.h>
#include <stdio.h>
#include <stdlib.h>

__attribute__((import_module("imports"), import_name("js_time"))) int js_time();

extern int kore_internal_window_width;
extern int kore_internal_window_height;

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
	win->width = width;
	win->height = height;

	kore_internal_window_width = width;
	kore_internal_window_height = height;

	// kore_g4_internal_init();
	// kore_g4_internal_init_window(0, frame->depth_bits, frame->stencil_bits, true);

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

__attribute__((export_name("_start"))) void _start(void) {
	kickstart(0, NULL);
}

__attribute__((export_name("_update"))) void _update(void) {
	kore_internal_update_callback();
	kore_audio_update();
}

__attribute__((export_name("_mousedown"))) void _mousedown(int button, int x, int y) {
	kore_internal_mouse_trigger_press(0, button, x, y);
}

__attribute__((export_name("_mouseup"))) void _mouseup(int button, int x, int y) {
	kore_internal_mouse_trigger_release(0, button, x, y);
}

__attribute__((export_name("_mousemove"))) void _mousemove(int x, int y) {
	kore_internal_mouse_trigger_move(0, x, y);
}

__attribute__((export_name("_wheel"))) void _wheel(int delta) {
	kore_internal_mouse_trigger_scroll(0, delta);
}

__attribute__((export_name("_keydown"))) void _keydown(int key) {
	kore_internal_keyboard_trigger_key_down(key);
}

__attribute__((export_name("_keyup"))) void _keyup(int key) {
	kore_internal_keyboard_trigger_key_up(key);
}
