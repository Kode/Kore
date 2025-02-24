#ifndef KORE_D3D11_COMMANDLIST_STRUCTS_HEADER
#define KORE_D3D11_COMMANDLIST_STRUCTS_HEADER

#include <kore3/util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_d3d11_device;
struct kore_d3d11_texture;
struct kore_d3d11_compute_pipeline;
struct kore_d3d11_ray_pipeline;
struct kore_d3d11_rendery_pipeline;
struct kore_d3d11_descriptor_set;
struct kore_gpu_query_set;

typedef struct kore_d3d11_buffer_access {
	int nothing;
} kore_d3d11_buffer_access;

typedef struct kore_d3d11_command_list {
	int nothing;
} kore_d3d11_command_list;

#ifdef __cplusplus
}
#endif

#endif
