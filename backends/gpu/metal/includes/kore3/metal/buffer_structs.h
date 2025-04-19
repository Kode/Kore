#ifndef KORE_METAL_BUFFER_STRUCTS_HEADER
#define KORE_METAL_BUFFER_STRUCTS_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

typedef enum kore_metal_buffer_usage {
	KORE_METAL_BUFFER_USAGE_VERTEX = 0x00010000,
} kore_metal_buffer_usage;

typedef struct kore_metal_buffer {
	void *buffer;
	void *locked_data;
} kore_metal_buffer;

#ifdef __cplusplus
}
#endif

#endif
