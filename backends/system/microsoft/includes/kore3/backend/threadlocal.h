#ifndef KORE_MICROSOFT_THREADLOCAL_HEADER
#define KORE_MICROSOFT_THREADLOCAL_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int slot;
} kore_thread_local_impl;

#ifdef __cplusplus
}
#endif

#endif
