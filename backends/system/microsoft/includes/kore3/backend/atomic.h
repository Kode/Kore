#ifndef KORE_MICROSOFT_ATOMIC_HEADER
#define KORE_MICROSOFT_ATOMIC_HEADER

#ifndef _WIN64
#include <kore3/error.h>
#endif

#include <intrin.h>

static inline bool kore_atomic_compare_exchange(volatile int32_t *pointer, int32_t old_value, int32_t new_value) {
	return _InterlockedCompareExchange((volatile long *)pointer, new_value, old_value) == old_value;
}

static inline bool kore_atomic_compare_exchange_pointer(void *volatile *pointer, void *old_value, void *new_value) {
	return _InterlockedCompareExchangePointer(pointer, new_value, old_value) == old_value;
}

static inline int32_t kore_atomic_increment(volatile int32_t *pointer) {
	return _InterlockedIncrement((volatile long *)pointer) - 1;
}

static inline int32_t kore_atomic_decrement(volatile int32_t *pointer) {
	return _InterlockedDecrement((volatile long *)pointer) + 1;
}

static inline void kore_atomic_exchange(volatile int32_t *pointer, int32_t value) {
	_InterlockedExchange((volatile long *)pointer, value);
}

static inline void kore_atomic_exchange_float(volatile float *pointer, float value) {
	_InterlockedExchange((volatile long *)pointer, *(long *)&value);
}

static inline void kore_atomic_exchange_double(volatile double *pointer, double value) {
#ifdef _WIN64
	_InterlockedExchange64((volatile __int64 *)pointer, *(__int64 *)&value);
#else
	kore_error_message("kore_atomic_exchange_double is not supported for 32 bit Windows builds");
#endif
}

#endif
