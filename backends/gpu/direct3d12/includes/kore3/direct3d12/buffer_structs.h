#ifndef KORE_D3D12_BUFFER_STRUCTS_HEADER
#define KORE_D3D12_BUFFER_STRUCTS_HEADER

#include "d3d12mini.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

#define KORE_D3D12_MAX_BUFFER_RANGES 16

typedef struct kore_d3d12_buffer_range {
	uint64_t offset;
	uint64_t size;
	uint64_t execution_index;
} kore_d3d12_buffer_range;

typedef struct kore_d3d12_buffer {
	struct kore_gpu_device *device;

	struct ID3D12Resource *resource;
	uint32_t resource_state;
	size_t size;

	void *locked_data;
	uint64_t locked_data_offset;
	uint64_t locked_data_size;

	bool cpu_write;
	bool cpu_read;

	kore_d3d12_buffer_range ranges[KORE_D3D12_MAX_BUFFER_RANGES];
	uint32_t ranges_count;
} kore_d3d12_buffer;

#ifdef __cplusplus
}
#endif

#endif
