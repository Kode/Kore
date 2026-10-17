#include "optional.h"

#include <assert.h>

get_pointer_info_type              get_pointer_info              = NULL;
get_pointer_pen_info_type          get_pointer_pen_info          = NULL;
enable_non_client_dpi_scaling_type enable_non_client_dpi_scaling = NULL;

void kore_windows_optional_init(void) {
	HMODULE user32 = LoadLibraryA("user32.dll");
	assert(user32 != NULL);

	get_pointer_info              = (get_pointer_info_type)GetProcAddress(user32, "GetPointerInfo");
	get_pointer_pen_info          = (get_pointer_pen_info_type)GetProcAddress(user32, "GetPointerPenInfo");
	enable_non_client_dpi_scaling = (enable_non_client_dpi_scaling_type)GetProcAddress(user32, "EnableNonClientDpiScaling");
}
