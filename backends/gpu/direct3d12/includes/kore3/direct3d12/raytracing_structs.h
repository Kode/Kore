#ifndef KORE_D3D12_RAYTRACING_STRUCTS_HEADER
#define KORE_D3D12_RAYTRACING_STRUCTS_HEADER

#include "d3d12mini.h"

#include <kore3/gpu/buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

typedef struct kore_d3d12_raytracing_volume {
	struct kore_gpu_device *device;

	uint64_t execution_index;

	kore_gpu_buffer *vertex_buffer;
	uint64_t         vertex_count;
	kore_gpu_buffer *index_buffer;
	uint32_t         index_count;

	kore_gpu_buffer scratch_buffer;
	kore_gpu_buffer acceleration_structure;
} kore_d3d12_raytracing_volume;

typedef struct kore_d3d12_raytracing_hierarchy {
	struct kore_gpu_device *device;

	uint64_t execution_index;

	kore_d3d12_raytracing_volume *volumes[128];
	uint32_t                      volumes_count;
	kore_gpu_buffer               instances;

	kore_gpu_buffer scratch_buffer;
	kore_gpu_buffer update_scratch_buffer;
	kore_gpu_buffer acceleration_structure;
} kore_d3d12_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
