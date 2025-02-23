#ifndef KORE_MICROSOFT_MUTEX_HEADER
#define KORE_MICROSOFT_MUTEX_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void *DebugInfo;
	long LockCount;
	long RecursionCount;
	void *OwningThread;
	void *LockSemaphore;
	unsigned long __w64 SpinCount;
} kore_microsoft_critical_section;

typedef struct {
	kore_microsoft_critical_section criticalSection;
} kore_mutex_impl;

typedef struct {
	void *id;
} kore_uber_mutex_impl;

#ifdef __cplusplus
}
#endif

#endif
