#include <kore3/threads/semaphore.h>

void kore_semaphore_init(kore_semaphore *semaphore, int current, int max) {
	semaphore->impl.handle = CreateSemaphoreA(NULL, current, max, NULL);
}

void kore_semaphore_destroy(kore_semaphore *semaphore) {
	CloseHandle(semaphore->impl.handle);
	semaphore->impl.handle = NULL;
}

void kore_semaphore_release(kore_semaphore *semaphore, int count) {
	ReleaseSemaphore(semaphore->impl.handle, count, NULL);
}

void kore_semaphore_acquire(kore_semaphore *semaphore) {
	WaitForSingleObject(semaphore->impl.handle, INFINITE);
}

bool kore_semaphore_try_to_acquire(kore_semaphore *semaphore, double seconds) {
	return WaitForSingleObject(semaphore->impl.handle, (DWORD)(seconds * 1000)) == WAIT_OBJECT_0;
}
