#include <kore3/threads/mutex.h>

#include <assert.h>

void kore_mutex_init(kore_mutex *mutex) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex->impl.mutex, &attr);
}

void kore_mutex_destroy(kore_mutex *mutex) {
	pthread_mutex_destroy(&mutex->impl.mutex);
}

bool kore_mutex_try_to_lock(kore_mutex *mutex) {
	return pthread_mutex_trylock(&mutex->impl.mutex) == 0;
}

void kore_mutex_lock(kore_mutex *mutex) {
	pthread_mutex_lock(&mutex->impl.mutex);
}

void kore_mutex_unlock(kore_mutex *mutex) {
	pthread_mutex_unlock(&mutex->impl.mutex);
}

bool kore_uber_mutex_init(kore_uber_mutex *mutex, const char *name) {
	return false;
}

void kore_uber_mutex_destroy(kore_uber_mutex *mutex) {}

void kore_uber_mutex_lock(kore_uber_mutex *mutex) {
	assert(false);
}

void kore_uber_mutex_unlock(kore_uber_mutex *mutex) {
	assert(false);
}
