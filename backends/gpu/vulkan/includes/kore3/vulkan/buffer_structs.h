#ifndef KORE_VULKAN_BUFFER_STRUCTS_HEADER
#define KORE_VULKAN_BUFFER_STRUCTS_HEADER

#include "vulkanmini.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

// add-ons to kore_gpu_buffer_usage, top 16 bits only
typedef enum kore_vulkan_buffer_usage {
	KORE_VULKAN_BUFFER_USAGE_VERTEX = 0x00010000,
} kore_vulkan_buffer_usage;

#define KORE_VULKAN_MAX_BUFFER_RANGES 16

typedef struct kore_vulkan_buffer_range {
	uint64_t offset;
	uint64_t size;
	uint64_t execution_index;
} kore_vulkan_buffer_range;

typedef struct kore_vulkan_buffer {
	struct kore_gpu_device *device;
	VkBuffer                buffer;
	VkDeviceMemory          memory;
	uint64_t                size;

	bool     host_visible;
	void    *locked_data;
	uint64_t locked_data_offset;
	uint64_t locked_data_size;

	kore_vulkan_buffer_range ranges[KORE_VULKAN_MAX_BUFFER_RANGES];
	uint32_t                 ranges_count;
} kore_vulkan_buffer;

#ifdef __cplusplus
}
#endif

#endif
