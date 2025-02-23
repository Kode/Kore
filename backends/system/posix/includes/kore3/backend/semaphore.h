#pragma once

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
#ifdef __APPLE__
	dispatch_semaphore_t semaphore;
#else
	sem_t semaphore;
#endif
} kore_semaphore_impl;

#ifdef __cplusplus
}
#endif
