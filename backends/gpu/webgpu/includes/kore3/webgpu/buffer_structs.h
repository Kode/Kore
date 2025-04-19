#ifndef KORE_WEBGPU_BUFFER_STRUCTS_HEADER
#define KORE_WEBGPU_BUFFER_STRUCTS_HEADER

#include "webgpu.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

// add-ons to kore_gpu_buffer_usage, top 16 bits only
typedef enum kore_webgpu_buffer_usage {
	KORE_WEBGPU_BUFFER_USAGE_VERTEX = 0x00010000,
	KORE_WEBGPU_BUFFER_USAGE_UNIFORM = 0x00020000,
	KORE_WEBGPU_BUFFER_USAGE_STORAGE = 0x00040000,
} kore_webgpu_buffer_usage;

typedef struct kore_webgpu_buffer {
	WGPUBuffer buffer;
	uint64_t   size;

	WGPUBuffer copy_buffer;
	bool       has_copy_buffer;
	bool       copy_scheduled;
} kore_webgpu_buffer;

#ifdef __cplusplus
}
#endif

#endif
