#ifndef KORE_MICROSOFT_FIBER_HEADER
#define KORE_MICROSOFT_FIBER_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void *fiber;
	void (*func)(void *param);
	void *param;
} kore_fiber_impl;

#ifdef __cplusplus
}
#endif

#endif
