#include <kore3/threads/thread.h>

void kore_thread_init(kore_thread *t, void (*thread)(void *param), void *param) {}

void kore_thread_wait_and_destroy(kore_thread *thread) {}

bool kore_thread_try_to_destroy(kore_thread *thread) {
	return false;
}

void kore_threads_init() {}

void kore_threads_quit() {}

void kore_thread_sleep(int milliseconds) {}
