#include <kore3/system.h>
#include <kore3/threads/semaphore.h>

#ifdef __APPLE__

void kore_semaphore_init(kore_semaphore *semaphore, int current, int max) {
	semaphore->impl.semaphore = dispatch_semaphore_create(current);
}

void kore_semaphore_destroy(kore_semaphore *semaphore) {}

void kore_semaphore_release(kore_semaphore *semaphore, int count) {
	for (int i = 0; i < count; ++i) {
		dispatch_semaphore_signal(semaphore->impl.semaphore);
	}
}

void kore_semaphore_acquire(kore_semaphore *semaphore) {
	dispatch_semaphore_wait(semaphore->impl.semaphore, DISPATCH_TIME_FOREVER);
}

bool kore_semaphore_try_to_acquire(kore_semaphore *semaphore, double seconds) {
	return dispatch_semaphore_wait(semaphore->impl.semaphore, dispatch_time(DISPATCH_TIME_NOW, (int64_t)(seconds * 1000 * 1000 * 1000))) == 0;
}

#else

void kore_semaphore_init(kore_semaphore *semaphore, int current, int max) {
	sem_init(&semaphore->impl.semaphore, 0, current);
}

void kore_semaphore_destroy(kore_semaphore *semaphore) {
	sem_destroy(&semaphore->impl.semaphore);
}

void kore_semaphore_release(kore_semaphore *semaphore, int count) {
	for (int i = 0; i < count; ++i) {
		sem_post(&semaphore->impl.semaphore);
	}
}

void kore_semaphore_acquire(kore_semaphore *semaphore) {
	sem_wait(&semaphore->impl.semaphore);
}

bool kore_semaphore_try_to_acquire(kore_semaphore *semaphore, double seconds) {
	double now = kinc_time();
	do {
		if (sem_trywait(&semaphore->impl.semaphore) == 0) {
			return true;
		}
	} while (kinc_time() < now + seconds);
	return false;
}

#endif
