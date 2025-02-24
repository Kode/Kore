#include <kore3/error.h>

#include <kore3/log.h>

#include <stdlib.h>

#ifdef KORE_WINDOWS

#include <kore3/backend/microsoft.h>
#include <kore3/backend/windowsmini.h>
#endif

void kore_affirm(bool condition) {
	if (!condition) {
		kore_error();
	}
}

void kore_affirm_message(bool condition, const char *format, ...) {
	if (!condition) {
		va_list args;
		va_start(args, format);
		kore_error_args(format, args);
		va_end(args);
	}
}

void kore_affirm_args(bool condition, const char *format, va_list args) {
	if (!condition) {
		kore_error_args(format, args);
	}
}

void kore_error(void) {
	kore_error_message("Unknown error");
}

void kore_error_message(const char *format, ...) {
	{
		va_list args;
		va_start(args, format);
		kore_log_args(KORE_LOG_LEVEL_ERROR, format, args);
		va_end(args);
	}

#ifdef KORE_WINDOWS
	{
		va_list args;
		va_start(args, format);
		wchar_t buffer[4096];
		kore_microsoft_format(format, args, buffer);
		MessageBoxW(NULL, buffer, L"Error", 0);
		va_end(args);
	}
#endif

#ifndef KORE_NO_CLIB
	exit(EXIT_FAILURE);
#endif
}

void kore_error_args(const char *format, va_list args) {
	kore_log_args(KORE_LOG_LEVEL_ERROR, format, args);

#ifdef KORE_WINDOWS
	wchar_t buffer[4096];
	kore_microsoft_format(format, args, buffer);
	MessageBoxW(NULL, buffer, L"Error", 0);
#endif

#ifndef KORE_NO_CLIB
	exit(EXIT_FAILURE);
#endif
}
