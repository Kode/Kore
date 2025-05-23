#include <kore3/threads/event.h>

#include <assert.h>
#include <errno.h>
#include <sys/time.h>

void kore_event_init(kore_event *event, bool auto_reset) {
	event->impl.auto_reset = auto_reset;
	event->impl.set        = false;

	pthread_cond_init(&event->impl.event, NULL);

	// pthread_mutexattr_t attr;
	// pthread_mutexattr_init(&attr);
	// pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&event->impl.mutex, NULL); //&attr);
}

void kore_event_destroy(kore_event *event) {
	pthread_cond_destroy(&event->impl.event);
	pthread_mutex_destroy(&event->impl.mutex);
}

void kore_event_signal(kore_event *event) {
	pthread_mutex_lock(&event->impl.mutex);
	if (!event->impl.set) {
		event->impl.set = true;
		pthread_cond_signal(&event->impl.event);
	}
	pthread_mutex_unlock(&event->impl.mutex);
}

void kore_event_wait(kore_event *event) {
	pthread_mutex_lock(&event->impl.mutex);
	while (!event->impl.set) {
		pthread_cond_wait(&event->impl.event, &event->impl.mutex);
	}
	if (event->impl.auto_reset) {
		event->impl.set = false;
	}
	pthread_mutex_unlock(&event->impl.mutex);
}

bool kore_event_try_to_wait(kore_event *event, double seconds) {
	pthread_mutex_lock(&event->impl.mutex);

	struct timeval tv;
	gettimeofday(&tv, 0);

	int             isec = (int)seconds;
	int             usec = (int)((seconds - isec) * 1000000.0);
	struct timespec spec;
	spec.tv_nsec = (tv.tv_usec + usec) * 1000;
	if (spec.tv_nsec > 1000000000) {
		spec.tv_nsec -= 1000000000;
		isec += 1;
	}
	spec.tv_sec = tv.tv_sec + isec;

	while (!event->impl.set) {
		int result = pthread_cond_timedwait(&event->impl.event, &event->impl.mutex, &spec);
		if (result == 0 || result == ETIMEDOUT) {
			break;
		}
	}
	bool result = event->impl.set;
	if (event->impl.auto_reset) {
		event->impl.set = false;
	}
	pthread_mutex_unlock(&event->impl.mutex);
	return result;
}

void kore_event_reset(kore_event *event) {
	pthread_mutex_lock(&event->impl.mutex);
	event->impl.set = false;
	pthread_mutex_unlock(&event->impl.mutex);
}
