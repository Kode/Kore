#ifndef KORE_D3D12_DESCRIPTORSET_STRUCTS_HEADER
#define KORE_D3D12_DESCRIPTORSET_STRUCTS_HEADER

#include "d3d12mini.h"

#include "device_structs.h"

#include <kore3/util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_d3d12_descriptor_set {
	oa_allocation_t descriptor_allocation;
	size_t descriptor_count;

	size_t dynamic_descriptor_count;

	oa_allocation_t bindless_descriptor_allocation;
	size_t bindless_descriptor_count;

	oa_allocation_t sampler_allocation;
	size_t sampler_count;

	uint64_t execution_index;
} kore_d3d12_descriptor_set;

#ifdef __cplusplus
}
#endif

#endif
