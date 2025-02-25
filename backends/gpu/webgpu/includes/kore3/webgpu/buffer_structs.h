#ifndef KORE_WEBGPU_BUFFER_STRUCTS_HEADER
#define KORE_WEBGPU_BUFFER_STRUCTS_HEADER

#include "webgpu.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

typedef struct kore_webgpu_buffer {
	WGPUBuffer buffer;
	uint64_t size;

	WGPUBuffer copy_buffer;
	bool has_copy_buffer;
	bool copy_scheduled;
} kore_webgpu_buffer;

#ifdef __cplusplus
}
#endif

#endif
