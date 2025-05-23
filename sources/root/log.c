#include <kore3/log.h>

#include <stdio.h>
#include <string.h>

#ifdef KORE_MICROSOFT
#include <kore3/backend/microsoft.h>
#include <kore3/backend/windowsmini.h>
#endif

#ifdef KORE_ANDROID
#include <android/log.h>
#endif

void kore_log(kore_log_level level, const char *format, ...) {
	va_list args;
	va_start(args, format);
	kore_log_args(level, format, args);
	va_end(args);
}

#define UTF8

void kore_log_args(kore_log_level level, const char *format, va_list args) {
#ifdef KORE_ANDROID
	va_list args_android_copy;
	va_copy(args_android_copy, args);
	switch (level) {
	case KORE_LOG_LEVEL_INFO:
		__android_log_vprint(ANDROID_LOG_INFO, "Kore", format, args_android_copy);
		break;
	case KORE_LOG_LEVEL_WARNING:
		__android_log_vprint(ANDROID_LOG_WARN, "Kore", format, args_android_copy);
		break;
	case KORE_LOG_LEVEL_ERROR:
		__android_log_vprint(ANDROID_LOG_ERROR, "Kore", format, args_android_copy);
		break;
	}
	va_end(args_android_copy);
#endif
#ifdef KORE_MICROSOFT
#ifdef UTF8
	wchar_t buffer[4096];
	kore_microsoft_format(format, args, buffer);
	wcscat(buffer, L"\r\n");
	OutputDebugStringW(buffer);
#ifdef KORE_WINDOWS
	DWORD written;
	WriteConsoleW(GetStdHandle(level == KORE_LOG_LEVEL_INFO ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE), buffer, (DWORD)wcslen(buffer), &written, NULL);
#endif
#else
	char buffer[4096];
	vsnprintf(buffer, 4090, format, args);
	strcat(buffer, "\r\n");
	OutputDebugStringA(buffer);
#ifdef KORE_WINDOWS
	DWORD written;
	WriteConsoleA(GetStdHandle(level == KORE_LOG_LEVEL_INFO ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE), buffer, (DWORD)strlen(buffer), &written, NULL);
#endif
#endif
#else
	char buffer[4096];
	vsnprintf(buffer, 4090, format, args);
	strcat(buffer, "\n");
	fprintf(level == KORE_LOG_LEVEL_INFO ? stdout : stderr, "%s", buffer);
#endif
}
