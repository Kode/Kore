#ifndef KORE_D3D12_DEVICE_STRUCTS_HEADER
#define KORE_D3D12_DEVICE_STRUCTS_HEADER

#include "d3d12mini.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/raytracing.h>
#include <kore3/gpu/texture.h>
#include <kore3/libs/offalloc/offalloc.h>
#include <kore3/util/indexallocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ID3D12CommandAllocator;
struct ID3D12DescriptorHeap;
struct ID3D12QueryHeap;

struct kore_d3d12_render_pipeline;
struct kore_d3d12_compute_pipeline;
struct kore_d3d12_descriptor_set;

#define KORE_D3D12_FRAME_COUNT 2

#define KORE_D3D12_GARBAGE_SIZE 256

#define KORE_D3D12_MAX_PIPELINES 1024

typedef struct kore_d3d12_device {
	struct ID3D12Device3      *device;
	struct ID3D12CommandQueue *graphics_queue;
	struct ID3D12CommandQueue *compute_queue;
	struct ID3D12CommandQueue *copy_queue;
#ifdef KORE_WINDOWS
	struct IDXGISwapChain *swap_chain;
#endif

	uint32_t cbv_srv_uav_increment;
	uint32_t sampler_increment;

	uint32_t         framebuffer_index;
	kore_gpu_texture framebuffer_textures[KORE_D3D12_FRAME_COUNT];

	struct ID3D12Fence *frame_fence;
	HANDLE              frame_event;
	uint64_t            current_frame_index;

	struct ID3D12Fence *execution_fence;
	HANDLE              execution_event;
	uint64_t            execution_index;

	struct ID3D12DescriptorHeap *descriptor_heap;
	oa_allocator_t               descriptor_heap_allocator;

	struct ID3D12DescriptorHeap *sampler_heap;
	oa_allocator_t               sampler_heap_allocator;

	struct ID3D12DescriptorHeap *all_samplers;
	kore_index_allocator         sampler_index_allocator;

	kore_gpu_command_list management_list;

	struct kore_d3d12_render_pipeline *render_pipelines[KORE_D3D12_MAX_PIPELINES];
	size_t                             render_pipelines_count;

	struct kore_d3d12_compute_pipeline *compute_pipelines[KORE_D3D12_MAX_PIPELINES];
	size_t                              compute_pipelines_count;

	struct kore_d3d12_ray_pipeline *ray_pipelines[KORE_D3D12_MAX_PIPELINES];
	size_t                          ray_pipelines_count;

	kore_gpu_buffer                  *garbage_buffers[KORE_D3D12_GARBAGE_SIZE];
	kore_gpu_texture                 *garbage_textures[KORE_D3D12_GARBAGE_SIZE];
	kore_gpu_command_list            *garbage_command_lists[KORE_D3D12_GARBAGE_SIZE];
	struct kore_d3d12_descriptor_set *garbage_descriptor_sets[KORE_D3D12_GARBAGE_SIZE];
	kore_gpu_raytracing_volume       *garbage_raytracing_volumes[KORE_D3D12_GARBAGE_SIZE];
	kore_gpu_raytracing_hierarchy    *garbage_raytracing_hierarchies[KORE_D3D12_GARBAGE_SIZE];
} kore_d3d12_device;

typedef struct kore_d3d12_query_set {
	struct ID3D12QueryHeap *query_heap;
	uint8_t                 query_type;
} kore_d3d12_query_set;

#ifdef __cplusplus
}
#endif

#endif
