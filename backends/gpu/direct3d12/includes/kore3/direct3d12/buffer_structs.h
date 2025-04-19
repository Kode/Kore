#ifndef KORE_D3D12_BUFFER_STRUCTS_HEADER
#define KORE_D3D12_BUFFER_STRUCTS_HEADER

#include "d3d12mini.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

#define KORE_D3D12_MAX_BUFFER_RANGES 16

// add-ons to kore_gpu_buffer_usage, top 16 bits only
typedef enum kore_d3d12_buffer_usage {
	KORE_D3D12_BUFFER_USAGE_VERTEX = 0x00010000,
	KORE_D3D12_BUFFER_USAGE_CBV    = 0x00020000,
	KORE_D3D12_BUFFER_USAGE_SRV    = 0x00040000,
	KORE_D3D12_BUFFER_USAGE_UAV    = 0x00080000,
} kore_d3d12_buffer_usage;

typedef struct kore_d3d12_buffer_range {
	uint64_t offset;
	uint64_t size;
	uint64_t execution_index;
} kore_d3d12_buffer_range;

typedef struct kore_d3d12_buffer {
	struct kore_gpu_device *device;

	struct ID3D12Resource *resource;
	uint32_t               resource_state;
	size_t                 size;

	void    *locked_data;
	uint64_t locked_data_offset;
	uint64_t locked_data_size;

	bool cpu_write;
	bool cpu_read;

	kore_d3d12_buffer_range ranges[KORE_D3D12_MAX_BUFFER_RANGES];
	uint32_t                ranges_count;
} kore_d3d12_buffer;

#ifdef __cplusplus
}
#endif

#endif
