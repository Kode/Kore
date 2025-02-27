#ifndef KORE_WASM_MUTEX_HEADER
#define KORE_WASM_MUTEX_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int nothing;
} kore_mutex_impl;

typedef struct {
	int nothing;
} kore_uber_mutex_impl;

#ifdef __cplusplus
}
#endif

#endif
