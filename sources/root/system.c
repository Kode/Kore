#include <kore3/system.h>

#include <kore3/window.h>

#include <kore3/io/filereader.h>
#include <kore3/io/filewriter.h>

#include <stdlib.h>
#include <string.h>

#if !defined(KORE_WASM) && !defined(KORE_EMSCRIPTEN) && !defined(KORE_ANDROID) && !defined(KORE_WINDOWS) && !defined(KORE_CONSOLE)
double kore_time(void) {
	return kore_timestamp() / kore_frequency();
}
#endif

static void (*update_callback)(void *)                = NULL;
static void *update_callback_data                     = NULL;
static void (*foreground_callback)(void *)            = NULL;
static void *foreground_callback_data                 = NULL;
static void (*background_callback)(void *)            = NULL;
static void *background_callback_data                 = NULL;
static void (*pause_callback)(void *)                 = NULL;
static void *pause_callback_data                      = NULL;
static void (*resume_callback)(void *)                = NULL;
static void *resume_callback_data                     = NULL;
static void (*shutdown_callback)(void *)              = NULL;
static void *shutdown_callback_data                   = NULL;
static void (*drop_files_callback)(wchar_t *, void *) = NULL;
static void *drop_files_callback_data                 = NULL;
static char *(*cut_callback)(void *)                  = NULL;
static void *cut_callback_data                        = NULL;
static char *(*copy_callback)(void *)                 = NULL;
static void *copy_callback_data                       = NULL;
static void (*paste_callback)(char *, void *)         = NULL;
static void *paste_callback_data                      = NULL;
static void (*login_callback)(void *)                 = NULL;
static void *login_callback_data                      = NULL;
static void (*logout_callback)(void *)                = NULL;
static void *logout_callback_data                     = NULL;

#if defined(KORE_IOS) || defined(KORE_MACOS)
bool withAutoreleasepool(bool (*f)(void));
#endif

void kore_set_update_callback(void (*callback)(void *), void *data) {
	update_callback      = callback;
	update_callback_data = data;
}

void kore_set_foreground_callback(void (*callback)(void *), void *data) {
	foreground_callback      = callback;
	foreground_callback_data = data;
}

void kore_set_resume_callback(void (*callback)(void *), void *data) {
	resume_callback      = callback;
	resume_callback_data = data;
}

void kore_set_pause_callback(void (*callback)(void *), void *data) {
	pause_callback      = callback;
	pause_callback_data = data;
}

void kore_set_background_callback(void (*callback)(void *), void *data) {
	background_callback      = callback;
	background_callback_data = data;
}

void kore_set_shutdown_callback(void (*callback)(void *), void *data) {
	shutdown_callback      = callback;
	shutdown_callback_data = data;
}

void kore_set_drop_files_callback(void (*callback)(wchar_t *, void *), void *data) {
	drop_files_callback      = callback;
	drop_files_callback_data = data;
}

void kore_set_cut_callback(char *(*callback)(void *), void *data) {
	cut_callback      = callback;
	cut_callback_data = data;
}

void kore_set_copy_callback(char *(*callback)(void *), void *data) {
	copy_callback      = callback;
	copy_callback_data = data;
}

void kore_set_paste_callback(void (*callback)(char *, void *), void *data) {
	paste_callback      = callback;
	paste_callback_data = data;
}

void kore_set_login_callback(void (*callback)(void *), void *data) {
	login_callback      = callback;
	login_callback_data = data;
}

void kore_set_logout_callback(void (*callback)(void *), void *data) {
	logout_callback      = callback;
	logout_callback_data = data;
}

void kore_internal_update_callback(void) {
	if (update_callback != NULL) {
		update_callback(update_callback_data);
	}
}

void kore_internal_foreground_callback(void) {
	if (foreground_callback != NULL) {
		foreground_callback(foreground_callback_data);
	}
}

void kore_internal_resume_callback(void) {
	if (resume_callback != NULL) {
		resume_callback(resume_callback_data);
	}
}

void kore_internal_pause_callback(void) {
	if (pause_callback != NULL) {
		pause_callback(pause_callback_data);
	}
}

void kore_internal_background_callback(void) {
	if (background_callback != NULL) {
		background_callback(background_callback_data);
	}
}

void kore_internal_shutdown_callback(void) {
	if (shutdown_callback != NULL) {
		shutdown_callback(shutdown_callback_data);
	}
}

void kore_internal_drop_files_callback(wchar_t *filePath) {
	if (drop_files_callback != NULL) {
		drop_files_callback(filePath, drop_files_callback_data);
	}
}

char *kore_internal_cut_callback(void) {
	if (cut_callback != NULL) {
		return cut_callback(cut_callback_data);
	}
	return NULL;
}

char *kore_internal_copy_callback(void) {
	if (copy_callback != NULL) {
		return copy_callback(copy_callback_data);
	}
	return NULL;
}

void kore_internal_paste_callback(char *value) {
	if (paste_callback != NULL) {
		paste_callback(value, paste_callback_data);
	}
}

void kore_internal_login_callback(void) {
	if (login_callback != NULL) {
		login_callback(login_callback_data);
	}
}

void kore_internal_logout_callback(void) {
	if (logout_callback != NULL) {
		logout_callback(logout_callback_data);
	}
}

static bool running = false;
// static bool showWindowFlag = true;
static char application_name[1024] = {"Kore Application"};

const char *kore_application_name(void) {
	return application_name;
}

void kore_set_application_name(const char *name) {
	strcpy(application_name, name);
}

void kore_stop(void) {
	running = false;

	// TODO (DK) destroy graphics + windows, but afaik Application::~Application() was never called, so it's the same behavior now as well

	// for (int windowIndex = 0; windowIndex < sizeof(windowIds) / sizeof(int); ++windowIndex) {
	//	Graphics::destroy(windowIndex);
	//}
}

bool kore_internal_frame(void) {
	kore_internal_update_callback();
	kore_internal_handle_messages();
	return running;
}

void kore_start(void) {
	running = true;

#if !defined(KORE_WASM) && !defined(KORE_EMSCRIPTEN)
	// if (Graphics::hasWindow()) Graphics::swapBuffers();

#if defined(KORE_IOS) || defined(KORE_MACOS)
	while (withAutoreleasepool(kore_internal_frame)) {
	}
#else
	while (kore_internal_frame()) {
	}
#endif
	kore_internal_shutdown();
#endif
}

int kore_width(void) {
	return kore_window_width(0);
}

int kore_height(void) {
	return kore_window_height(0);
}

#ifndef KHA
void kore_memory_emergency(void) {}
#endif

#if !defined(KORE_SONY) && !defined(KORE_NINTENDO)
static float safe_zone = 0.9f;

float kore_safe_zone(void) {
#ifdef KORE_ANDROID
	return 1.0f;
#else
	return safe_zone;
#endif
}

bool kore_automatic_safe_zone(void) {
#ifdef KORE_ANDROID
	return true;
#else
	return false;
#endif
}

void kore_set_safe_zone(float value) {
	safe_zone = value;
}
#endif

#if !defined(KORE_SONY)
bool is_save_load_initialized(void) {
	return true;
}

bool is_ps4_japanese_button_style(void) {
	return false;
}

bool is_save_load_broken(void) {
	return false;
}
#endif

#if !defined(KORE_CONSOLE)

#define SAVE_RESULT_NONE    0
#define SAVE_RESULT_SUCCESS 1
#define SAVE_RESULT_FAILURE 2
volatile int save_result = SAVE_RESULT_SUCCESS;

void kore_disallow_user_change(void) {}

void kore_allow_user_change(void) {}

static uint8_t *current_file      = NULL;
static size_t   current_file_size = 0;

bool kore_save_file_loaded(void) {
	return true;
}

uint8_t *kore_get_save_file(void) {
	return current_file;
}

size_t kore_get_save_file_size(void) {
	return current_file_size;
}

void kore_load_save_file(const char *filename) {
	free(current_file);
	current_file      = NULL;
	current_file_size = 0;

	kore_file_reader reader;
	if (kore_file_reader_open(&reader, filename, KORE_FILE_TYPE_SAVE)) {
		current_file_size = kore_file_reader_size(&reader);
		current_file      = (uint8_t *)malloc(current_file_size);
		kore_file_reader_read(&reader, current_file, current_file_size);
		kore_file_reader_close(&reader);
	}
}

void kore_save_save_file(const char *filename, uint8_t *data, size_t size) {
	kore_file_writer writer;
	if (kore_file_writer_open(&writer, filename)) {
		kore_file_writer_write(&writer, data, (int)size);
		kore_file_writer_close(&writer);
	}
}

bool kore_save_is_saving(void) {
	return false;
}

bool kore_waiting_for_login(void) {
	return false;
}

#if !defined(KORE_WINDOWS) && !defined(KORE_LINUX) && !defined(KORE_MACOS)
void kore_copy_to_clipboard(const char *text) {
	kore_log(KORE_LOG_LEVEL_WARNING, "Oh no, kore_copy_to_clipboard is not implemented for this system.");
}
#endif

#endif
