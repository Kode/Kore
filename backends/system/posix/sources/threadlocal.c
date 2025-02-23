#include <kore3/threads/threadlocal.h>

void kore_thread_local_init(kore_thread_local *local) {
	pthread_key_create(&local->impl.key, NULL);
}

void kore_thread_local_destroy(kore_thread_local *local) {
	pthread_key_delete(local->impl.key);
}

void *kore_thread_local_get(kore_thread_local *local) {
	return pthread_getspecific(local->impl.key);
}

void kore_thread_local_set(kore_thread_local *local, void *data) {
	pthread_setspecific(local->impl.key, data);
}
