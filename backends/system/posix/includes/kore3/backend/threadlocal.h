#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	pthread_key_t key;
} kore_thread_local_impl;

#ifdef __cplusplus
}
#endif
