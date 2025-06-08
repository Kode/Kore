#ifndef KORE_WEBGPU_COMMANDLIST_STRUCTS_HEADER
#define KORE_WEBGPU_COMMANDLIST_STRUCTS_HEADER

#include "webgpu.h"

#ifdef __cplusplus
extern "C" {
#endif

struct kore_webgpu_device;
struct kore_webgpu_texture;
struct kore_webgpu_compute_pipeline;
struct kore_webgpu_ray_pipeline;
struct kore_webgpu_rendery_pipeline;
struct kore_webgpu_descriptor_set;
struct kore_gpu_query_set;

#define KORE_WEBGPU_ROOT_CONSTANTS_SIZE 4096

typedef struct kore_webgpu_compute_bind_group {
	uint32_t index;
	WGPUBindGroup bind_group;
	uint32_t dynamic_count;
	uint32_t dynamic_offsets[64];
} kore_webgpu_compute_bind_group;

typedef struct kore_webgpu_command_list {
	WGPUCommandEncoder     command_encoder;
	WGPURenderPassEncoder  render_pass_encoder;
	WGPUComputePassEncoder compute_pass_encoder;

	uint8_t       root_constants_data[256];
	WGPUBuffer    root_constants_buffer;
	WGPUBindGroup root_constants_bind_group;
	uint32_t      root_constants_offset;
	bool          root_constants_written;

	WGPUComputePipeline compute_pipeline;
	kore_webgpu_compute_bind_group compute_bind_groups[16];
	uint32_t compute_bind_groups_count;
} kore_webgpu_command_list;

#ifdef __cplusplus
}
#endif

#endif
