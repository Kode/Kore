#ifndef KORE_VULKAN_BUFFER_STRUCTS_HEADER
#define KORE_VULKAN_BUFFER_STRUCTS_HEADER

#include "vulkanmini.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

typedef struct kore_vulkan_buffer {
	VkDevice device;
	VkBuffer buffer;
	VkDeviceMemory memory;
	uint64_t size;

	void *locked_data;
	uint64_t locked_data_offset;
	uint64_t locked_data_size;
} kore_vulkan_buffer;

#ifdef __cplusplus
}
#endif

#endif
