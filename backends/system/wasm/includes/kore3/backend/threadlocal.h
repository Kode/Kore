#ifndef KORE_WASM_THREADLOCAL_HEADER
#define KORE_WASM_THREADLOCAL_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int nothing;
} kore_thread_local_impl;

#ifdef __cplusplus
}
#endif

#endif
