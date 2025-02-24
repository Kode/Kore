#include <kore3/threads/semaphore.h>

void kore_semaphore_init(kore_semaphore *semaphore, int current, int max) {}

void kore_semaphore_destroy(kore_semaphore *semaphore) {}

void kore_semaphore_release(kore_semaphore *semaphore, int count) {}

void kore_semaphore_acquire(kore_semaphore *semaphore) {}

bool kore_semaphore_try_to_acquire(kore_semaphore *semaphore, double seconds) {
	return false;
}
