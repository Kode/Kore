#ifndef KORE_VULKAN_DEVICE_STRUCTS_HEADER
#define KORE_VULKAN_DEVICE_STRUCTS_HEADER

#include "vulkanmini.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_VULKAN_EXECUTION_FENCE_COUNT 8

typedef struct kore_vulkan_execution_fence {
	VkFence  fences[KORE_VULKAN_EXECUTION_FENCE_COUNT];
	uint64_t fence_values[KORE_VULKAN_EXECUTION_FENCE_COUNT];

	uint64_t next_execution_index;
	uint64_t completed_index;
} kore_vulkan_execution_fence;

typedef struct kore_vulkan_device {
	VkDevice                         device;
	VkCommandPool                    command_pool;
	VkQueue                          queue;
	VkPhysicalDeviceMemoryProperties device_memory_properties;
	VkDescriptorPool                 descriptor_pool;

	kore_vulkan_execution_fence execution_fence;

	bool has_dynamic_rendering;
} kore_vulkan_device;

typedef struct kore_vulkan_query_set {
	int nothing;
} kore_vulkan_query_set;

#ifdef __cplusplus
}
#endif

#endif
