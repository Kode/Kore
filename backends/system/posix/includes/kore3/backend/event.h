#ifndef KORE_POSIX_EVENT_HEADER
#define KORE_POSIX_EVENT_HEADER

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	pthread_cond_t  event;
	pthread_mutex_t mutex;
	volatile bool   set;
	bool            auto_reset;
} kore_event_impl;

#ifdef __cplusplus
}
#endif

#endif
