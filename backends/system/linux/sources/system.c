// #include "kore3/graphics4/graphics.h"
#include <kore3/display.h>
#include <kore3/input/gamepad.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/input/pen.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/video.h>
#include <kore3/window.h>

#ifndef __FreeBSD__
#include <kore3/backend/gamepad.h>
#endif

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <kore3/backend/funcs.h>

bool kore_internal_handle_messages() {
	if (!procs.handle_messages()) {
		return false;
	}
#ifndef __FreeBSD__
	kore_linux_updateHIDGamepads();
#endif // TODO: add #else with proper call to FreeBSD backend impl
	return true;
}

const char *kore_system_id() {
	return "Linux";
}

void kore_set_keep_screen_on(bool on) {}

void kore_keyboard_show() {}

void kore_keyboard_hide() {}

bool kore_keyboard_active() {
	return true;
}

void kore_load_url(const char *url) {
#define MAX_COMMAND_BUFFER_SIZE 256
#define HTTP                    "http://"
#define HTTPS                   "https://"
	if (strncmp(url, HTTP, sizeof(HTTP) - 1) == 0 || strncmp(url, HTTPS, sizeof(HTTPS) - 1) == 0) {
		char openUrlCommand[MAX_COMMAND_BUFFER_SIZE];
		snprintf(openUrlCommand, MAX_COMMAND_BUFFER_SIZE, "xdg-open %s", url);
		int err = system(openUrlCommand);
		if (err != 0) {
			kore_log(KORE_LOG_LEVEL_WARNING, "Error opening url %s", url);
		}
	}
#undef HTTPS
#undef HTTP
#undef MAX_COMMAND_BUFFER_SIZE
}

void kore_vibrate(int ms) {}

const char *kore_language() {
	return "en";
}

static char save[2000];
static bool saveInitialized = false;

const char *kore_internal_save_path() {
	// first check for an existing directory in $HOME
	// if one exists, use it, else create one in $XDG_DATA_HOME
	// See: https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
	if (!saveInitialized) {
		const char *homedir;

		if ((homedir = getenv("HOME")) == NULL) {
			homedir = getpwuid(getuid())->pw_dir;
		}

		strcpy(save, homedir);
		strcat(save, "/.");
		strcat(save, kore_application_name());
		strcat(save, "/");
		struct stat st;
		if (stat(save, &st) == 0) {
			// use existing folder in $HOME
		}
		else {
			// use XDG folder
			const char *data_home;
			if ((data_home = getenv("XDG_DATA_HOME")) == NULL) {
				// $XDG_DATA_HOME is not defined, fall back to the default, $HOME/.local/share
				strcpy(save, homedir);
				strcat(save, "/.local/share/");
			}
			else {
				// use $XDG_DATA_HOME
				strcpy(save, data_home);
				if (data_home[strlen(data_home) - 1] != '/') {
					strcat(save, "/");
				}
			}
			strcat(save, kore_application_name());
			strcat(save, "/");
			int res = mkdir(save, 0700);
			if (res != 0 && errno != EEXIST) {
				kore_log(KORE_LOG_LEVEL_ERROR, "Could not create save directory '%s'. Error %d", save, errno);
			}
		}

		saveInitialized = true;
	}
	return save;
}

static const char *videoFormats[] = {"ogv", NULL};

const char **kore_video_formats() {
	return videoFormats;
}

#include <sys/time.h>
#include <time.h>

double kore_frequency(void) {
	return 1000000.0;
}

static struct timeval start;

kore_ticks kore_timestamp(void) {
	struct timeval now;
	gettimeofday(&now, NULL);
	now.tv_sec -= start.tv_sec;
	now.tv_usec -= start.tv_usec;
	return (kore_ticks)now.tv_sec * 1000000 + (kore_ticks)now.tv_usec;
}

void kore_login() {}

void kore_unlock_achievement(int id) {}

void kore_linux_init_procs();

int kore_init(const char *name, int width, int height, kore_window_parameters *win, kore_framebuffer_parameters *frame) {
#ifndef __FreeBSD__
	kore_linux_initHIDGamepads();
#endif // TODO: add #else with proper call to FreeBSD backend impl

	gettimeofday(&start, NULL);
	kore_linux_init_procs();
	kore_display_init();

	kore_set_application_name(name);

	// kore_g4_internal_init();

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
	if (win->title == NULL) {
		win->title = name;
	}

	int window = kore_window_create(win, frame);

	return window;
}

void kore_internal_shutdown() {
	// kore_g4_internal_destroy();
#ifndef __FreeBSD__
	kore_linux_closeHIDGamepads();
#endif // TODO: add #else with proper call to FreeBSD backend impl
	procs.shutdown();
	kore_internal_shutdown_callback();
}

#ifndef KORE_NO_MAIN
int main(int argc, char **argv) {
	return kickstart(argc, argv);
}
#endif

void kore_copy_to_clipboard(const char *text) {
	procs.copy_to_clipboard(text);
}

static int parse_number_at_end_of_line(char *line) {
	char *end   = &line[strlen(line) - 2];
	int   num   = 0;
	int   multi = 1;
	while (*end >= '0' && *end <= '9') {
		num += (*end - '0') * multi;
		multi *= 10;
		--end;
	}
	return num;
}

int kore_cpu_cores(void) {
	char  line[1024];
	FILE *file = fopen("/proc/cpuinfo", "r");

	if (file != NULL) {
		int cores[1024];
		memset(cores, 0, sizeof(cores));

		int cpu_count       = 0;
		int physical_id     = -1;
		int per_cpu_cores   = -1;
		int processor_count = 0;

		while (fgets(line, sizeof(line), file)) {
			if (strncmp(line, "processor", 9) == 0) {
				++processor_count;
				if (physical_id >= 0 && per_cpu_cores > 0) {
					if (physical_id + 1 > cpu_count) {
						cpu_count = physical_id + 1;
					}
					cores[physical_id] = per_cpu_cores;
					physical_id        = -1;
					per_cpu_cores      = -1;
				}
			}
			else if (strncmp(line, "physical id", 11) == 0) {
				physical_id = parse_number_at_end_of_line(line);
			}
			else if (strncmp(line, "cpu cores", 9) == 0) {
				per_cpu_cores = parse_number_at_end_of_line(line);
			}
		}
		fclose(file);

		if (physical_id >= 0 && per_cpu_cores > 0) {
			if (physical_id + 1 > cpu_count) {
				cpu_count = physical_id + 1;
			}
			cores[physical_id] = per_cpu_cores;
		}

		int proper_cpu_count = 0;
		for (int i = 0; i < cpu_count; ++i) {
			proper_cpu_count += cores[i];
		}

		if (proper_cpu_count > 0) {
			return proper_cpu_count;
		}
		else {
			return processor_count == 0 ? 1 : processor_count;
		}
	}
	else {
		return 1;
	};
}

int kore_hardware_threads(void) {
#ifndef __FreeBSD__
	return sysconf(_SC_NPROCESSORS_ONLN);
#else
	return kore_cpu_cores();
#endif
}

#include <xkbcommon/xkbcommon.h>

int xkb_to_kore(xkb_keysym_t symbol) {
#define KEY(xkb, kore) \
	case xkb:          \
		return kore;
	switch (symbol) {
		KEY(XKB_KEY_Right, KORE_KEY_RIGHT)
		KEY(XKB_KEY_Left, KORE_KEY_LEFT)
		KEY(XKB_KEY_Up, KORE_KEY_UP)
		KEY(XKB_KEY_Down, KORE_KEY_DOWN)
		KEY(XKB_KEY_space, KORE_KEY_SPACE)
		KEY(XKB_KEY_BackSpace, KORE_KEY_BACKSPACE)
		KEY(XKB_KEY_Tab, KORE_KEY_TAB)
		KEY(XKB_KEY_Return, KORE_KEY_RETURN)
		KEY(XKB_KEY_Shift_L, KORE_KEY_SHIFT)
		KEY(XKB_KEY_Shift_R, KORE_KEY_SHIFT)
		KEY(XKB_KEY_Control_L, KORE_KEY_CONTROL)
		KEY(XKB_KEY_Control_R, KORE_KEY_CONTROL)
		KEY(XKB_KEY_Alt_L, KORE_KEY_ALT)
		KEY(XKB_KEY_Alt_R, KORE_KEY_ALT)
		KEY(XKB_KEY_Delete, KORE_KEY_DELETE)
		KEY(XKB_KEY_comma, KORE_KEY_COMMA)
		KEY(XKB_KEY_period, KORE_KEY_PERIOD)
		KEY(XKB_KEY_bracketleft, KORE_KEY_OPEN_BRACKET)
		KEY(XKB_KEY_bracketright, KORE_KEY_CLOSE_BRACKET)
		KEY(XKB_KEY_braceleft, KORE_KEY_OPEN_CURLY_BRACKET)
		KEY(XKB_KEY_braceright, KORE_KEY_CLOSE_CURLY_BRACKET)
		KEY(XKB_KEY_parenleft, KORE_KEY_OPEN_PAREN)
		KEY(XKB_KEY_parenright, KORE_KEY_CLOSE_PAREN)
		KEY(XKB_KEY_backslash, KORE_KEY_BACK_SLASH)
		KEY(XKB_KEY_apostrophe, KORE_KEY_QUOTE)
		KEY(XKB_KEY_colon, KORE_KEY_COLON)
		KEY(XKB_KEY_semicolon, KORE_KEY_SEMICOLON)
		KEY(XKB_KEY_minus, KORE_KEY_HYPHEN_MINUS)
		KEY(XKB_KEY_underscore, KORE_KEY_UNDERSCORE)
		KEY(XKB_KEY_slash, KORE_KEY_SLASH)
		KEY(XKB_KEY_bar, KORE_KEY_PIPE)
		KEY(XKB_KEY_question, KORE_KEY_QUESTIONMARK)
		KEY(XKB_KEY_less, KORE_KEY_LESS_THAN)
		KEY(XKB_KEY_greater, KORE_KEY_GREATER_THAN)
		KEY(XKB_KEY_asterisk, KORE_KEY_ASTERISK)
		KEY(XKB_KEY_ampersand, KORE_KEY_AMPERSAND)
		KEY(XKB_KEY_asciicircum, KORE_KEY_CIRCUMFLEX)
		KEY(XKB_KEY_percent, KORE_KEY_PERCENT)
		KEY(XKB_KEY_dollar, KORE_KEY_DOLLAR)
		KEY(XKB_KEY_numbersign, KORE_KEY_HASH)
		KEY(XKB_KEY_at, KORE_KEY_AT)
		KEY(XKB_KEY_exclam, KORE_KEY_EXCLAMATION)
		KEY(XKB_KEY_equal, KORE_KEY_EQUALS)
		KEY(XKB_KEY_plus, KORE_KEY_ADD)
		KEY(XKB_KEY_quoteleft, KORE_KEY_BACK_QUOTE)
		KEY(XKB_KEY_quotedbl, KORE_KEY_DOUBLE_QUOTE)
		KEY(XKB_KEY_asciitilde, KORE_KEY_TILDE)
		KEY(XKB_KEY_Pause, KORE_KEY_PAUSE)
		KEY(XKB_KEY_Scroll_Lock, KORE_KEY_SCROLL_LOCK)
		KEY(XKB_KEY_Home, KORE_KEY_HOME)
		KEY(XKB_KEY_Page_Up, KORE_KEY_PAGE_UP)
		KEY(XKB_KEY_Page_Down, KORE_KEY_PAGE_DOWN)
		KEY(XKB_KEY_End, KORE_KEY_END)
		KEY(XKB_KEY_Insert, KORE_KEY_INSERT)
		KEY(XKB_KEY_KP_Enter, KORE_KEY_RETURN)
		KEY(XKB_KEY_KP_Multiply, KORE_KEY_MULTIPLY)
		KEY(XKB_KEY_KP_Add, KORE_KEY_ADD)
		KEY(XKB_KEY_KP_Subtract, KORE_KEY_SUBTRACT)
		KEY(XKB_KEY_KP_Decimal, KORE_KEY_DECIMAL)
		KEY(XKB_KEY_KP_Divide, KORE_KEY_DIVIDE)
		KEY(XKB_KEY_KP_0, KORE_KEY_NUMPAD_0)
		KEY(XKB_KEY_KP_1, KORE_KEY_NUMPAD_1)
		KEY(XKB_KEY_KP_2, KORE_KEY_NUMPAD_2)
		KEY(XKB_KEY_KP_3, KORE_KEY_NUMPAD_3)
		KEY(XKB_KEY_KP_4, KORE_KEY_NUMPAD_4)
		KEY(XKB_KEY_KP_5, KORE_KEY_NUMPAD_5)
		KEY(XKB_KEY_KP_6, KORE_KEY_NUMPAD_6)
		KEY(XKB_KEY_KP_7, KORE_KEY_NUMPAD_7)
		KEY(XKB_KEY_KP_8, KORE_KEY_NUMPAD_8)
		KEY(XKB_KEY_KP_9, KORE_KEY_NUMPAD_9)
		KEY(XKB_KEY_KP_Insert, KORE_KEY_INSERT)
		KEY(XKB_KEY_KP_Delete, KORE_KEY_DELETE)
		KEY(XKB_KEY_KP_End, KORE_KEY_END)
		KEY(XKB_KEY_KP_Home, KORE_KEY_HOME)
		KEY(XKB_KEY_KP_Left, KORE_KEY_LEFT)
		KEY(XKB_KEY_KP_Up, KORE_KEY_UP)
		KEY(XKB_KEY_KP_Right, KORE_KEY_RIGHT)
		KEY(XKB_KEY_KP_Down, KORE_KEY_DOWN)
		KEY(XKB_KEY_KP_Page_Up, KORE_KEY_PAGE_UP)
		KEY(XKB_KEY_KP_Page_Down, KORE_KEY_PAGE_DOWN)
		KEY(XKB_KEY_Menu, KORE_KEY_CONTEXT_MENU)
		KEY(XKB_KEY_a, KORE_KEY_A)
		KEY(XKB_KEY_b, KORE_KEY_B)
		KEY(XKB_KEY_c, KORE_KEY_C)
		KEY(XKB_KEY_d, KORE_KEY_D)
		KEY(XKB_KEY_e, KORE_KEY_E)
		KEY(XKB_KEY_f, KORE_KEY_F)
		KEY(XKB_KEY_g, KORE_KEY_G)
		KEY(XKB_KEY_h, KORE_KEY_H)
		KEY(XKB_KEY_i, KORE_KEY_I)
		KEY(XKB_KEY_j, KORE_KEY_J)
		KEY(XKB_KEY_k, KORE_KEY_K)
		KEY(XKB_KEY_l, KORE_KEY_L)
		KEY(XKB_KEY_m, KORE_KEY_M)
		KEY(XKB_KEY_n, KORE_KEY_N)
		KEY(XKB_KEY_o, KORE_KEY_O)
		KEY(XKB_KEY_p, KORE_KEY_P)
		KEY(XKB_KEY_q, KORE_KEY_Q)
		KEY(XKB_KEY_r, KORE_KEY_R)
		KEY(XKB_KEY_s, KORE_KEY_S)
		KEY(XKB_KEY_t, KORE_KEY_T)
		KEY(XKB_KEY_u, KORE_KEY_U)
		KEY(XKB_KEY_v, KORE_KEY_V)
		KEY(XKB_KEY_w, KORE_KEY_W)
		KEY(XKB_KEY_x, KORE_KEY_X)
		KEY(XKB_KEY_y, KORE_KEY_Y)
		KEY(XKB_KEY_z, KORE_KEY_Z)
		KEY(XKB_KEY_1, KORE_KEY_1)
		KEY(XKB_KEY_2, KORE_KEY_2)
		KEY(XKB_KEY_3, KORE_KEY_3)
		KEY(XKB_KEY_4, KORE_KEY_4)
		KEY(XKB_KEY_5, KORE_KEY_5)
		KEY(XKB_KEY_6, KORE_KEY_6)
		KEY(XKB_KEY_7, KORE_KEY_7)
		KEY(XKB_KEY_8, KORE_KEY_8)
		KEY(XKB_KEY_9, KORE_KEY_9)
		KEY(XKB_KEY_0, KORE_KEY_0)
		KEY(XKB_KEY_Escape, KORE_KEY_ESCAPE)
		KEY(XKB_KEY_F1, KORE_KEY_F1)
		KEY(XKB_KEY_F2, KORE_KEY_F2)
		KEY(XKB_KEY_F3, KORE_KEY_F3)
		KEY(XKB_KEY_F4, KORE_KEY_F4)
		KEY(XKB_KEY_F5, KORE_KEY_F5)
		KEY(XKB_KEY_F6, KORE_KEY_F6)
		KEY(XKB_KEY_F7, KORE_KEY_F7)
		KEY(XKB_KEY_F8, KORE_KEY_F8)
		KEY(XKB_KEY_F9, KORE_KEY_F9)
		KEY(XKB_KEY_F10, KORE_KEY_F10)
		KEY(XKB_KEY_F11, KORE_KEY_F11)
		KEY(XKB_KEY_F12, KORE_KEY_F12)
	default:
		return KORE_KEY_UNKNOWN;
	}
#undef KEY
}