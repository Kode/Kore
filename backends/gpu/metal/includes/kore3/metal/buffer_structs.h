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

#define KORE_METAL_MAX_BUFFER_RANGES 16

typedef struct kore_metal_buffer_range {
	uint64_t offset;
	uint64_t size;
	uint64_t execution_index;
} kore_metal_buffer_range;

typedef struct kore_metal_buffer {
	void *buffer;
	void *locked_data;

	uint64_t size;
	bool     host_visible;

	kore_metal_buffer_range ranges[KORE_METAL_MAX_BUFFER_RANGES];
	uint32_t                ranges_count;

	struct kore_gpu_device *device;
} kore_metal_buffer;

#ifdef __cplusplus
}
#endif

#endif
