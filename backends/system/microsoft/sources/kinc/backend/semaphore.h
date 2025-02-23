#ifndef KORE_MICROSOFT_SEMAPHORE_HEADER
#define KORE_MICROSOFT_SEMAPHORE_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void *handle;
} kore_semaphore_impl;

#ifdef __cplusplus
}
#endif

#endif
