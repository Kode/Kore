#include <kore3/threads/thread.h>

#ifdef KORE_VTUNE
#include <ittnotify.h>
#endif

#ifdef KORE_SUPERLUMINAL
#include <Superluminal/PerformanceAPI_capi.h>
#endif

typedef struct thread_start_params {
	void (*func)(void *param);
	void *param;
} thread_start_params;

static DWORD WINAPI thread_start(LPVOID arg) {
	thread_start_params *params = (thread_start_params *)arg;

	void (*func)(void *param) = params->func;
	void *param               = params->param;

	free(params);

	func(param);

	return 0;
}

void kore_thread_init(kore_thread *thread, void (*func)(void *param), void *param) {
	thread_start_params *params = (thread_start_params *)malloc(sizeof(thread_start_params));
	assert(params != NULL);

	params->func  = func;
	params->param = param;

	thread->impl.handle = CreateThread(0, 65536, thread_start, params, 0, 0);
	assert(thread->impl.handle != NULL);
}

void kore_thread_wait_and_destroy(kore_thread *thread) {
	WaitForSingleObject(thread->impl.handle, INFINITE);
	CloseHandle(thread->impl.handle);
}

bool kore_thread_try_to_destroy(kore_thread *thread) {
	DWORD code;
	GetExitCodeThread(thread->impl.handle, &code);
	if (code != STILL_ACTIVE) {
		CloseHandle(thread->impl.handle);
		return true;
	}
	return false;
}

typedef HRESULT(WINAPI *SetThreadDescriptionType)(HANDLE hThread, PCWSTR lpThreadDescription);
static SetThreadDescriptionType MySetThreadDescription        = NULL;
static bool                     set_thread_description_loaded = false;

void kore_thread_set_name(const char *name) {
	if (!set_thread_description_loaded) {
		HMODULE kernel32              = LoadLibraryA("kernel32.dll");
		MySetThreadDescription        = (SetThreadDescriptionType)GetProcAddress(kernel32, "SetThreadDescription");
		set_thread_description_loaded = true;
	}

	if (MySetThreadDescription != NULL) {
		wchar_t wide_name[256];
		MultiByteToWideChar(CP_ACP, 0, name, -1, wide_name, 256);
		MySetThreadDescription(GetCurrentThread(), wide_name);
	}

#ifdef KORE_VTUNE
	__itt_thread_set_name(name);
#endif

#ifdef KORE_SUPERLUMINAL
	PerformanceAPI_SetCurrentThreadName(name);
#endif
}

void kore_thread_sleep(int milliseconds) {
	Sleep(milliseconds);
}
