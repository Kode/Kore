#include "pch.h"

#include "system.h"

#include "window.h"

#include <kinc/io/filereader.h>
#include <kinc/io/filewriter.h>
#include <kinc/log.h>

#include <stdlib.h>
#include <string.h>

#if !defined(KORE_HTML5) && !defined(KORE_ANDROID) && !defined(KORE_WINDOWS) && !defined(KORE_CONSOLE)
double kinc_time() {
	return kinc_timestamp() / kinc_frequency();
}
#endif

static void (*update_callback)() = NULL;
static void (*foreground_callback)() = NULL;
static void (*background_callback)() = NULL;
static void (*pause_callback)() = NULL;
static void (*resume_callback)() = NULL;
static void (*shutdown_callback)() = NULL;
static void (*drop_files_callback)(wchar_t *) = NULL;
static char *(*cut_callback)() = NULL;
static char *(*copy_callback)() = NULL;
static void (*paste_callback)(char *) = NULL;
static void (*login_callback)() = NULL;
static void (*logout_callback)() = NULL;

#if defined(KORE_IOS) || defined(KORE_MACOS)
bool withAutoreleasepool(bool (*f)());
#endif

void kinc_set_update_callback(void (*value)()) {
	update_callback = value;
}

void kinc_set_foreground_callback(void (*value)()) {
	foreground_callback = value;
}

void kinc_set_resume_callback(void (*value)()) {
	resume_callback = value;
}

void kinc_set_pause_callback(void (*value)()) {
	pause_callback = value;
}

void kinc_set_background_callback(void (*value)()) {
	background_callback = value;
}

void kinc_set_shutdown_callback(void (*value)()) {
	shutdown_callback = value;
}

void kinc_set_drop_files_callback(void (*value)(wchar_t *)) {
	drop_files_callback = value;
}

void kinc_set_cut_callback(char *(*value)()) {
	cut_callback = value;
}

void kinc_set_copy_callback(char *(*value)()) {
	copy_callback = value;
}

void kinc_set_paste_callback(void (*value)(char *)) {
	paste_callback = value;
}

void kinc_set_login_callback(void (*value)()) {
	login_callback = value;
}

void kinc_set_logout_callback(void (*value)()) {
	logout_callback = value;
}

void kinc_internal_update_callback() {
	if (update_callback != NULL) {
		update_callback();
	}
}

void kinc_internal_foreground_callback() {
	if (foreground_callback != NULL) {
		foreground_callback();
	}
}

void kinc_internal_resume_callback() {
	if (resume_callback != NULL) {
		resume_callback();
	}
}

void kinc_internal_pause_callback() {
	if (pause_callback != NULL) {
		pause_callback();
	}
}

void kinc_internal_background_callback() {
	if (background_callback != NULL) {
		background_callback();
	}
}

void kinc_internal_shutdown_callback() {
	if (shutdown_callback != NULL) {
		shutdown_callback();
	}
}

void kinc_internal_drop_files_callback(wchar_t *filePath) {
	if (drop_files_callback != NULL) {
		drop_files_callback(filePath);
	}
}

char *kinc_internal_cut_callback() {
	if (cut_callback != NULL) {
		return cut_callback();
	}
	return NULL;
}

char *kinc_internal_copy_callback() {
	if (copy_callback != NULL) {
		return copy_callback();
	}
	return NULL;
}

void kinc_internal_paste_callback(char *value) {
	if (paste_callback != NULL) {
		paste_callback(value);
	}
}

void kinc_internal_login_callback() {
	if (login_callback != NULL) {
		login_callback();
	}
}

void kinc_internal_logout_callback() {
	if (logout_callback != NULL) {
		logout_callback();
	}
}

static bool running = false;
// static bool showWindowFlag = true;
static char application_name[1024] = {"Kinc Application"};

const char *kinc_application_name() {
	return application_name;
}

void kinc_set_application_name(const char *name) {
	strcpy(application_name, name);
}

#ifdef KORE_METAL
void shutdownMetalCompute();
#endif

void kinc_stop() {
	running = false;

	// TODO (DK) destroy graphics + windows, but afaik Application::~Application() was never called, so it's the same behavior now as well

	// for (int windowIndex = 0; windowIndex < sizeof(windowIds) / sizeof(int); ++windowIndex) {
	//	Graphics::destroy(windowIndex);
	//}

#ifdef KORE_METAL
	shutdownMetalCompute();
#endif
}

bool kinc_internal_frame() {
	kinc_internal_update_callback();
	kinc_internal_handle_messages();
	return running;
}

void kinc_start() {
	running = true;

#if !defined(KORE_HTML5) && !defined(KORE_TIZEN)
	// if (Graphics::hasWindow()) Graphics::swapBuffers();

#if defined(KORE_IOS) || defined(KORE_MACOS)
	while (withAutoreleasepool(kinc_internal_frame)) {
	}
#else
	while (kinc_internal_frame()) {
	}
#endif
	kinc_internal_shutdown();
#endif
}

int kinc_width() {
	return kinc_window_width(0);
}

int kinc_height() {
	return kinc_window_height(0);
}

#ifndef KHA
void kinc_memory_emergency() {}
#endif

#if !defined(KORE_SONY) && !defined(KORE_SWITCH) && !defined(KORE_SWITCH2)
static float safe_zone = 0.9f;

float kinc_safe_zone(void) {
#ifdef KORE_ANDROID
	return 1.0f;
#else
	return safe_zone;
#endif
}

bool kinc_automatic_safe_zone() {
#ifdef KORE_ANDROID
	return true;
#else
	return false;
#endif
}

void kinc_set_safe_zone(float value) {
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

#define SAVE_RESULT_NONE 0
#define SAVE_RESULT_SUCCESS 1
#define SAVE_RESULT_FAILURE 2
volatile int save_result = SAVE_RESULT_SUCCESS;

void kinc_disallow_user_change() {}

void kinc_allow_user_change() {}

static uint8_t *current_file = NULL;
static size_t current_file_size = 0;

bool kinc_save_file_loaded() {
	return true;
}

uint8_t *kinc_get_save_file() {
	return current_file;
}

size_t kinc_get_save_file_size() {
	return current_file_size;
}

void kinc_load_save_file(const char *filename) {
	free(current_file);
	current_file = NULL;
	current_file_size = 0;

	kinc_file_reader_t reader;
	if (kinc_file_reader_open(&reader, filename, KINC_FILE_TYPE_SAVE)) {
		current_file_size = kinc_file_reader_size(&reader);
		current_file = (uint8_t *)malloc(current_file_size);
		kinc_file_reader_read(&reader, current_file, current_file_size);
		kinc_file_reader_close(&reader);
	}
}

static void commit_save(const char *old_filename, const char *new_filename);

void kinc_save_save_file(const char *filename, uint8_t *data, size_t size) {
	char temp_filename[1001];
	strcpy(temp_filename, filename);
	strcat(temp_filename, ".kinctemp");

	kinc_file_writer_t writer;
	if (kinc_file_writer_open(&writer, temp_filename)) {
		kinc_file_writer_write(&writer, data, (int)size);
		kinc_file_writer_close(&writer);
		commit_save(temp_filename, filename);
	}
}

bool kinc_save_is_saving() {
	return false;
}

bool kinc_waiting_for_login() {
	return false;
}

#endif

#ifdef KORE_WINDOWS
#include <Windows.h>

static void commit_save(const char *old_filename, const char *new_filename) {
	char old_path[1001];
	strcpy(old_path, kinc_internal_save_path());
	strcat(old_path, old_filename);

	char new_path[1001];
	strcpy(new_path, kinc_internal_save_path());
	strcat(new_path, new_filename);

	if (!MoveFileExA(old_path, new_path, MOVEFILE_REPLACE_EXISTING)) {
		kinc_log(KINC_LOG_LEVEL_WARNING, "Could not save (error code %d)", GetLastError());
	}
}

double kinc_local_time() {
	SYSTEMTIME system_time;
	GetLocalTime(&system_time);
	FILETIME file_time;
	SystemTimeToFileTime(&system_time, &file_time);
	ULARGE_INTEGER large_time;
	large_time.LowPart = file_time.dwLowDateTime;
	large_time.HighPart = file_time.dwHighDateTime;
	unsigned long long time = large_time.QuadPart;
	time /= 10;
	return (double)time;
}

double kinc_hours(double time) {
	unsigned long long utime = (unsigned long long)time;
	utime *= 10;
	ULARGE_INTEGER large_time;
	large_time.QuadPart = utime;
	FILETIME file_time;
	file_time.dwLowDateTime = large_time.LowPart;
	file_time.dwHighDateTime = large_time.HighPart;
	SYSTEMTIME system_time;
	FileTimeToSystemTime(&file_time, &system_time);
	return (double)system_time.wHour;
}

double kinc_minutes(double time) {
	unsigned long long utime = (unsigned long long)time;
	utime *= 10;
	ULARGE_INTEGER large_time;
	large_time.QuadPart = utime;
	FILETIME file_time;
	file_time.dwLowDateTime = large_time.LowPart;
	file_time.dwHighDateTime = large_time.HighPart;
	SYSTEMTIME system_time;
	FileTimeToSystemTime(&file_time, &system_time);
	return (double)system_time.wMinute;
}

double kinc_month(double time) {
	unsigned long long utime = (unsigned long long)time;
	utime *= 10;
	ULARGE_INTEGER large_time;
	large_time.QuadPart = utime;
	FILETIME file_time;
	file_time.dwLowDateTime = large_time.LowPart;
	file_time.dwHighDateTime = large_time.HighPart;
	SYSTEMTIME system_time;
	FileTimeToSystemTime(&file_time, &system_time);
	return (double)(system_time.wMonth - 1);
}
#endif

#ifdef KORE_ANDROID
static void commit_save(const char *old_filename, const char *new_filename) {
	
}
#endif
