#ifndef KORE_D3D12_DEVICE_STRUCTS_HEADER
#define KORE_D3D12_DEVICE_STRUCTS_HEADER

#include "d3d12mini.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/texture.h>
#include <util/indexallocator.h>
#include <util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ID3D12CommandAllocator;
struct ID3D12DescriptorHeap;
struct ID3D12QueryHeap;

#define KORE_D3D12_FRAME_COUNT 2

typedef struct kore_d3d12_device {
	struct ID3D12Device5 *device;
	struct ID3D12CommandQueue *graphics_queue;
	struct ID3D12CommandQueue *compute_queue;
	struct ID3D12CommandQueue *copy_queue;
	struct IDXGISwapChain *swap_chain;

	uint32_t cbv_srv_uav_increment;
	uint32_t sampler_increment;

	uint32_t framebuffer_index;
	kore_gpu_texture framebuffer_textures[KORE_D3D12_FRAME_COUNT];

	struct ID3D12Fence *frame_fence;
	HANDLE frame_event;
	uint64_t current_frame_index;

	struct ID3D12Fence *execution_fence;
	HANDLE execution_event;
	uint64_t execution_index;

	struct ID3D12DescriptorHeap *descriptor_heap;
	oa_allocator_t descriptor_heap_allocator;

	struct ID3D12DescriptorHeap *sampler_heap;
	oa_allocator_t sampler_heap_allocator;

	struct ID3D12DescriptorHeap *all_samplers;
	kore_index_allocator sampler_index_allocator;

	kore_gpu_command_list management_list;
} kore_d3d12_device;

typedef struct kore_d3d12_query_set {
	struct ID3D12QueryHeap *query_heap;
	uint8_t query_type;
} kore_d3d12_query_set;

typedef struct kore_d3d12_raytracing_volume {
	kore_gpu_buffer *vertex_buffer;
	uint64_t vertex_count;
	kore_gpu_buffer *index_buffer;
	uint32_t index_count;

	kore_gpu_buffer scratch_buffer;
	kore_gpu_buffer acceleration_structure;
} kore_d3d12_raytracing_volume;

typedef struct kore_d3d12_raytracing_hierarchy {
	uint32_t volumes_count;
	kore_gpu_buffer instances;

	kore_gpu_buffer scratch_buffer;
	kore_gpu_buffer update_scratch_buffer;
	kore_gpu_buffer acceleration_structure;
} kore_d3d12_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
