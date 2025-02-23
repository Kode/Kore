#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	pthread_mutex_t mutex;
} kore_mutex_impl;

typedef struct {
	int nothing;
} kore_uber_mutex_impl;

#ifdef __cplusplus
}
#endif
