#ifndef KORE_D3D12_DESCRIPTORSET_STRUCTS_HEADER
#define KORE_D3D12_DESCRIPTORSET_STRUCTS_HEADER

#include "d3d12mini.h"

#include "device_structs.h"

#include <kore3/libs/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

typedef struct kore_d3d12_descriptor_set_allocation {
	oa_allocation_t descriptor_allocation;
	size_t          descriptor_count;

	size_t dynamic_descriptor_count;

	oa_allocation_t bindless_descriptor_allocation;
	size_t          bindless_descriptor_count;

	oa_allocation_t sampler_allocation;
	size_t          sampler_count;

	uint64_t execution_index;
	uint32_t command_list_reference_count;
} kore_d3d12_descriptor_set_allocation;

#define KORE_D3D12_ALLOCATIONS_PER_DESCRIPTORSET 3

typedef struct kore_d3d12_descriptor_set {
	struct kore_gpu_device              *device;
	kore_d3d12_descriptor_set_allocation allocations[KORE_D3D12_ALLOCATIONS_PER_DESCRIPTORSET];
	uint32_t                             current_allocation_index;
} kore_d3d12_descriptor_set;

#ifdef __cplusplus
}
#endif

#endif
