#ifndef KORE_VULKAN_DESCRIPTORSET_STRUCTS_HEADER
#define KORE_VULKAN_DESCRIPTORSET_STRUCTS_HEADER

#include "device_structs.h"

#include <kore3/util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_vulkan_descriptor_set {
	VkDescriptorSet descriptor_set;
} kore_vulkan_descriptor_set;

#ifdef __cplusplus
}
#endif

#endif
