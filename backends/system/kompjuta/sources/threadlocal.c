#include <kore3/threads/threadlocal.h>

#include <string.h>

void kore_thread_local_init(kore_thread_local *local) {}

void kore_thread_local_destroy(kore_thread_local *local) {}

void *kore_thread_local_get(kore_thread_local *local) {
	return NULL;
}

void kore_thread_local_set(kore_thread_local *local, void *data) {}
