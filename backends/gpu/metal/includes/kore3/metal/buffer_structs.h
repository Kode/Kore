#ifndef KORE_METAL_BUFFER_STRUCTS_HEADER
#define KORE_METAL_BUFFER_STRUCTS_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

typedef struct kore_metal_buffer {
	void *buffer;
} kore_metal_buffer;

#ifdef __cplusplus
}
#endif

#endif
