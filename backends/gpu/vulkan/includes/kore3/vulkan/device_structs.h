#ifndef KORE_VULKAN_DEVICE_STRUCTS_HEADER
#define KORE_VULKAN_DEVICE_STRUCTS_HEADER

#include "vulkanmini.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_vulkan_device {
	VkDevice device;
	VkCommandPool command_pool;
	VkQueue queue;
	VkPhysicalDeviceMemoryProperties device_memory_properties;
	VkDescriptorPool descriptor_pool;
} kore_vulkan_device;

typedef struct kore_vulkan_query_set {
	int nothing;
} kore_vulkan_query_set;

typedef struct kore_vulkan_raytracing_volume {
	int nothing;
} kore_vulkan_raytracing_volume;

typedef struct kore_vulkan_raytracing_hierarchy {
	int nothing;
} kore_vulkan_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
