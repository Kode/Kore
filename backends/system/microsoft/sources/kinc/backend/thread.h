#ifndef KORE_MICROSOFT_THREAD_HEADER
#define KORE_MICROSOFT_THREAD_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void *handle;
	void *param;
	void (*func)(void *param);
} kore_thread_impl;

#ifdef __cplusplus
}
#endif

#endif
