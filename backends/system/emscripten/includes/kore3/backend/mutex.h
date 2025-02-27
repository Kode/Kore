#ifndef KORE_EMSCRIPTEN_MUTEX_HEADER
#define KORE_EMSCRIPTEN_MUTEX_HEADER

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
