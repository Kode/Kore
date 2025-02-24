#include <kore3/threads/mutex.h>

void kore_mutex_init(kore_mutex *mutex) {
	assert(sizeof(RTL_CRITICAL_SECTION) == sizeof(kore_microsoft_critical_section));
	InitializeCriticalSection((LPCRITICAL_SECTION)&mutex->impl.criticalSection);
}

void kore_mutex_destroy(kore_mutex *mutex) {
	DeleteCriticalSection((LPCRITICAL_SECTION)&mutex->impl.criticalSection);
}

void kore_mutex_lock(kore_mutex *mutex) {
	EnterCriticalSection((LPCRITICAL_SECTION)&mutex->impl.criticalSection);
}

bool kore_mutex_try_to_lock(kore_mutex *mutex) {
	return TryEnterCriticalSection((LPCRITICAL_SECTION)&mutex->impl.criticalSection);
}

void kore_mutex_unlock(kore_mutex *mutex) {
	LeaveCriticalSection((LPCRITICAL_SECTION)&mutex->impl.criticalSection);
}

bool kore_uber_mutex_init(kore_uber_mutex *mutex, const char *name) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	mutex->impl.id = (void *)CreateMutexA(NULL, FALSE, name);
	HRESULT res = GetLastError();
	if (res && res != ERROR_ALREADY_EXISTS) {
		mutex->impl.id = NULL;
		assert(false);
		return false;
	}
	return true;
#else
	return false;
#endif
}

void kore_uber_mutex_destroy(kore_uber_mutex *mutex) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	if (mutex->impl.id) {
		CloseHandle((HANDLE)mutex->impl.id);
		mutex->impl.id = NULL;
	}
#endif
}

void kore_uber_mutex_lock(kore_uber_mutex *mutex) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	bool succ = WaitForSingleObject((HANDLE)mutex->impl.id, INFINITE) == WAIT_FAILED ? false : true;
	assert(succ);
#endif
}

void kore_uber_mutex_unlock(kore_uber_mutex *mutex) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	bool succ = ReleaseMutex((HANDLE)mutex->impl.id) == FALSE ? false : true;
	assert(succ);
#endif
}
