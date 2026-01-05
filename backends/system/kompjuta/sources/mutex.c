#include <kore3/threads/mutex.h>

void kore_mutex_init(kore_mutex *mutex) {}

void kore_mutex_destroy(kore_mutex *mutex) {}

bool kore_mutex_try_to_lock(kore_mutex *mutex) {
	return false;
}

void kore_mutex_lock(kore_mutex *mutex) {}

void kore_mutex_unlock(kore_mutex *mutex) {}

bool kore_uber_mutex_init(kore_uber_mutex *mutex, const char *name) {
	return false;
}

void kore_uber_mutex_destroy(kore_uber_mutex *mutex) {}

void kore_uber_mutex_lock(kore_uber_mutex *mutex) {}

void kore_uber_mutex_unlock(kore_uber_mutex *mutex) {}
