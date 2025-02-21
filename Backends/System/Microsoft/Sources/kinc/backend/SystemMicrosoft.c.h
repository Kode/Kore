#include "SystemMicrosoft.h"

#include <kinc/error.h>

#define S_OK ((HRESULT)0L)

static void winerror(HRESULT result) {
	LPVOID buffer = NULL;
	DWORD dw = GetLastError();

	__debugbreak();

#if defined(KINC_WINDOWS) || defined(KINC_WINDOWSAPP)
	if (dw != 0) {
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
		               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);

		kinc_error_message("Error: %s", buffer);
	}
	else {
#endif
		kinc_error_message("Unknown Windows error, return value was 0x%x.", result);
#if defined(KINC_WINDOWS) || defined(KINC_WINDOWSAPP)
	}
#endif
}

void kinc_microsoft_affirm(HRESULT result) {
	if (result != S_OK) {
		winerror(result);
	}
}

void kinc_microsoft_affirm_message(HRESULT result, const char *format, ...) {
	va_list args;
	va_start(args, format);
	kinc_affirm_args(result == S_OK, format, args);
	va_end(args);
}

void kinc_microsoft_format(const char *format, va_list args, wchar_t *buffer) {
	char cbuffer[4096];
	vsprintf(cbuffer, format, args);
	MultiByteToWideChar(CP_UTF8, 0, cbuffer, -1, buffer, 4096);
}

void kinc_microsoft_convert_string(wchar_t *destination, const char *source, int num) {
	MultiByteToWideChar(CP_UTF8, 0, source, -1, destination, num);
}
