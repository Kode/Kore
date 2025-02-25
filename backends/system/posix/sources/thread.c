#include <kore3/threads/thread.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#if !defined(KORE_IOS) && !defined(KORE_MACOS)

struct thread_start {
	void (*thread)(void *param);
	void *param;
};

#define THREAD_STARTS 64
static struct thread_start starts[THREAD_STARTS];
static int thread_start_index = 0;

static void *ThreadProc(void *arg) {
	intptr_t start_index = (intptr_t)arg;
	starts[start_index].thread(starts[start_index].param);
	pthread_exit(NULL);
	return NULL;
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
	intptr_t start_index = thread_start_index++;
	if (thread_start_index >= THREAD_STARTS) {
		thread_start_index = 0;
	}
	starts[start_index].thread = thread;
	starts[start_index].param = param;
	int ret = pthread_create(&t->impl.pthread, &attr, &ThreadProc, (void *)start_index);
	assert(ret == 0);
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

void kore_threads_init() {}

void kore_threads_quit() {}

#endif

#if !defined(KORE_IOS) && !defined(KORE_MACOS)
// Alternatively _GNU_SOURCE can be defined to make
// the headers declare it but let's not make it too
// easy to write Linux-specific POSIX-code
int pthread_setname_np(pthread_t thread, const char *name);
#endif

void kore_thread_set_name(const char *name) {
#if !defined(KORE_IOS) && !defined(KORE_MACOS)
	pthread_setname_np(pthread_self(), name);
#else
	pthread_setname_np(name);
#endif
}

void kore_thread_sleep(int milliseconds) {
	usleep(1000 * (useconds_t)milliseconds);
}
