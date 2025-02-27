#ifndef KORE_WASM_ATOMIC_HEADER
#define KORE_WASM_ATOMIC_HEADER

#include <kore3/global.h>

static inline bool kore_atomic_compare_exchange(volatile int32_t *pointer, int32_t old_value, int32_t new_value) {
	return false;
}

static inline bool kore_atomic_compare_exchange_pointer(void *volatile *pointer, void *old_value, void *new_value) {
	return false;
}

static inline int32_t kore_atomic_increment(volatile int32_t *pointer) {
	return 0;
}

static inline int32_t kore_atomic_decrement(volatile int32_t *pointer) {
	return 0;
}

static inline void kore_atomic_exchange(volatile int32_t *pointer, int32_t value) {}

static inline void kore_atomic_exchange_float(volatile float *pointer, float value) {}

static inline void kore_atomic_exchange_double(volatile double *pointer, double value) {}

#endif
