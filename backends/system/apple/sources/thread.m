#include <stdio.h>
#include <string.h>

#include <Foundation/Foundation.h>

#include <kore3/threads/mutex.h>
#include <kore3/threads/thread.h>

#include <pthread.h>
#include <stdio.h>
#include <wchar.h>

static void *ThreadProc(void *arg) {
	@autoreleasepool {
		kore_thread *t = (kore_thread *)arg;
		t->impl.thread(t->impl.param);
		pthread_exit(NULL);
		return NULL;
	}
}

void kore_thread_init(kore_thread *t, void (*thread)(void *param), void *param) {
	t->impl.param = param;
	t->impl.thread = thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	// pthread_attr_setstacksize(&attr, 1024 * 64);
	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = 0;
	pthread_attr_setschedparam(&attr, &sp);
	pthread_create(&t->impl.pthread, &attr, &ThreadProc, t);
	// Kt::affirmD(ret == 0);
	pthread_attr_destroy(&attr);
}

void kore_thread_wait_and_destroy(kore_thread *thread) {
	int ret;
	do {
		ret = pthread_join(thread->impl.pthread, NULL);
	} while (ret != 0);
}

bool kore_thread_try_to_destroy(kore_thread *thread) {
	return pthread_join(thread->impl.pthread, NULL) == 0;
}

void kore_threads_init(void) {}

void kore_threads_quit(void) {}
