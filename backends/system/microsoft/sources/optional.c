#include "optional.h"

set_thread_description_type set_thread_description = NULL;

void kore_microsoft_optional_init(void) {
	HMODULE kernel32       = LoadLibraryA("kernel32.dll");
	set_thread_description = (set_thread_description_type)GetProcAddress(kernel32, "SetThreadDescription");
}
