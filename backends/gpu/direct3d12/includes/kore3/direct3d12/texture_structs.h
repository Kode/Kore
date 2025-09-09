#ifndef KORE_D3D12_TEXTURE_STRUCTS_HEADER
#define KORE_D3D12_TEXTURE_STRUCTS_HEADER

#include "d3d12mini.h"

#ifdef __cplusplus
extern "C" {
#endif

// add-ons to kore_gpu_texture_usage, top 16 bits only
typedef enum kore_d3d12_texture_usage {
	KORE_D3D12_TEXTURE_USAGE_SRV         = 0x00010000,
	KORE_D3D12_TEXTURE_USAGE_UAV         = 0x00020000,
	KORE_D3D12_TEXTURE_USAGE_FRAMEBUFFER = 0x00040000,
} kore_d3d12_texture_usage;

typedef struct kore_d3d12_texture {
	struct kore_gpu_device *device;

	struct ID3D12Resource *resource;

	uint32_t depth_or_array_layers;
	uint32_t mip_level_count;

	uint32_t resource_states[64];
	uint32_t format;

	// used for framebuffer-synchronization
	uint64_t in_flight_frame_index;

	uint64_t execution_index;
} kore_d3d12_texture;

#ifdef __cplusplus
}
#endif

#endif
