#ifndef KORE_D3D12_COMMANDLIST_STRUCTS_HEADER
#define KORE_D3D12_COMMANDLIST_STRUCTS_HEADER

#include "d3d12mini.h"

#include <kore3/libs/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_d3d12_device;
struct kore_d3d12_texture;
struct kore_d3d12_compute_pipeline;
struct kore_d3d12_ray_pipeline;
struct kore_d3d12_rendery_pipeline;
struct kore_d3d12_descriptor_set;
struct kore_d3d12_raytracing_volume;
struct kore_d3d12_raytracing_hierarchy;
struct kore_gpu_query_set;

struct ID3D12Fence;

// Allocators can not be re-used while a command-list is executing. We carry along a bag of allocators so we only have to wait when we ran out of in-flight
// allocators. Increasing this value exchanges more memory against potentially less wait-times (depending on actual command-list usage).
#define KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT 3

#define KORE_D3D12_COMMAND_LIST_DYNAMIC_DESCRIPTORS_COUNT 64

#define KORE_D3D12_COMMAND_LIST_MAX_QUEUED_BUFFER_ACCESSES 256

#define KORE_D3D12_COMMAND_LIST_MAX_QUEUED_TEXTURE_ACCESSES 256

#define KORE_D3D12_COMMAND_LIST_MAX_QUEUED_DESCRIPTOR_SET_ACCESSES 256

typedef struct kore_d3d12_buffer_access {
	kore_d3d12_buffer *buffer;
	uint64_t           offset;
	uint64_t           size;
} kore_d3d12_buffer_access;

typedef struct kore_d3d12_command_list {
	struct kore_d3d12_device *device;

	int list_type;

	struct ID3D12CommandAllocator *allocator[KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT];
	uint64_t                       allocator_execution_index[KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT];
	oa_allocation_t                dynamic_descriptor_allocations[KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT];
	uint32_t                       dynamic_descriptor_offsets[KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT];
	uint8_t                        current_allocator_index;

	struct ID3D12GraphicsCommandList4 *list;

	struct ID3D12DescriptorHeap *rtv_descriptors;
	uint32_t                     rtv_increment;

	struct ID3D12DescriptorHeap *dsv_descriptor;
	uint32_t                     dsv_increment;

	// set when a framebuffer is attached to a render-pass so we don't render into it during scan-out
	uint64_t blocking_frame_index;

	struct kore_d3d12_render_pipeline *render_pipe;

	struct kore_d3d12_compute_pipeline *compute_pipe;

	struct kore_d3d12_ray_pipeline *ray_pipe;

	struct kore_gpu_query_set *occlusion_query_set;
	uint32_t                   current_occlusion_query_index;

	struct kore_gpu_query_set *timestamp_query_set;
	uint32_t                   timestamp_beginning_of_pass_write_index;
	uint32_t                   timestamp_end_of_pass_write_index;

	kore_d3d12_buffer_access queued_buffer_accesses[KORE_D3D12_COMMAND_LIST_MAX_QUEUED_BUFFER_ACCESSES];
	uint32_t                 queued_buffer_accesses_count;

	kore_d3d12_texture *queued_texture_accesses[KORE_D3D12_COMMAND_LIST_MAX_QUEUED_TEXTURE_ACCESSES];
	uint32_t            queued_texture_accesses_count;

	struct kore_d3d12_raytracing_volume *queued_raytracing_volume_accesses[KORE_D3D12_COMMAND_LIST_MAX_QUEUED_TEXTURE_ACCESSES];
	uint32_t                             queued_raytracing_volume_accesses_count;

	struct kore_d3d12_raytracing_hierarchy *queued_raytracing_hierarchy_accesses[KORE_D3D12_COMMAND_LIST_MAX_QUEUED_TEXTURE_ACCESSES];
	uint32_t                                queued_raytracing_hierarchy_accesses_count;

	struct kore_d3d12_descriptor_set *queued_descriptor_set_accesses[KORE_D3D12_COMMAND_LIST_MAX_QUEUED_DESCRIPTOR_SET_ACCESSES];
	uint32_t                          queued_descriptor_set_accesses_count;

	bool presenting;
} kore_d3d12_command_list;

#ifdef __cplusplus
}
#endif

#endif
