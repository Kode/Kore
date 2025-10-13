#ifndef KORE_VULKAN_DESCRIPTORSET_STRUCTS_HEADER
#define KORE_VULKAN_DESCRIPTORSET_STRUCTS_HEADER

#include "device_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_vulkan_descriptor_set {
	struct kore_gpu_device *device;
	VkDescriptorSet         descriptor_set;
} kore_vulkan_descriptor_set;

#ifdef __cplusplus
}
#endif

#endif
