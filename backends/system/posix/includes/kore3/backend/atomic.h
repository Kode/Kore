#ifndef KORE_POSIX_ATOMIC_HEADER
#define KORE_POSIX_ATOMIC_HEADER

#include <kore3/global.h>

// clang/gcc intrinsics

static inline bool kore_atomic_compare_exchange(volatile int32_t *pointer, int32_t old_value, int32_t new_value) {
	return __sync_val_compare_and_swap(pointer, old_value, new_value) == old_value;
}

static inline bool kore_atomic_compare_exchange_pointer(void *volatile *pointer, void *old_value, void *new_value) {
	return __sync_val_compare_and_swap(pointer, old_value, new_value) == old_value;
}

static inline int32_t kore_atomic_increment(volatile int32_t *pointer) {
	return __sync_fetch_and_add(pointer, 1);
}

static inline int32_t kore_atomic_decrement(volatile int32_t *pointer) {
	return __sync_fetch_and_sub(pointer, 1);
}

#ifdef __clang__

static inline void kore_atomic_exchange(volatile int32_t *pointer, int32_t value) {
	__sync_swap(pointer, value);
}

static inline void kore_atomic_exchange_float(volatile float *pointer, float value) {
	__sync_swap((volatile int32_t *)pointer, *(int32_t *)&value);
}

static inline void kore_atomic_exchange_double(volatile double *pointer, double value) {
	__sync_swap((volatile int64_t *)pointer, *(int64_t *)&value);
}

#else

// Beware, __sync_lock_test_and_set is not a full barrier and can have platform-specific weirdness

static inline void kore_atomic_exchange(volatile int32_t *pointer, int32_t value) {
	__sync_lock_test_and_set(pointer, value);
}

static inline void kore_atomic_exchange_float(volatile float *pointer, float value) {
	__sync_lock_test_and_set((volatile int32_t *)pointer, *(int32_t *)&value);
}

static inline void kore_atomic_exchange_double(volatile double *pointer, double value) {
	__sync_lock_test_and_set((volatile int64_t *)pointer, *(int64_t *)&value);
}

#endif

#endif
