#ifndef KORE_POSIX_THREAD_HEADER
#define KORE_POSIX_THREAD_HEADER

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void *param;
	void (*thread)(void *param);
	pthread_t pthread;
} kore_thread_impl;

#ifdef __cplusplus
}
#endif

#endif
