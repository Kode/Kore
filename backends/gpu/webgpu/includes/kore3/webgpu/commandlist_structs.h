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

typedef struct kore_webgpu_buffer_access {
	int nothing;
} kore_webgpu_buffer_access;

typedef struct kore_webgpu_command_list {
	WGPUCommandEncoder    command_encoder;
	WGPURenderPassEncoder render_pass_encoder;
} kore_webgpu_command_list;

#ifdef __cplusplus
}
#endif

#endif
