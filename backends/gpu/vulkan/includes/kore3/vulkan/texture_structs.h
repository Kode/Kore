#ifndef KORE_VULKAN_TEXTURE_STRUCTS_HEADER
#define KORE_VULKAN_TEXTURE_STRUCTS_HEADER

#include <kore3/vulkan/vulkanmini.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_vulkan_texture {
	uint32_t width;
	uint32_t height;

	VkDeviceMemory device_memory;
	VkDeviceSize   device_size;

	VkImage       image;
	VkImageLayout image_layouts[64];

	kore_gpu_texture_format format;
	uint32_t                mip_level_count;

	VkDevice device;
} kore_vulkan_texture;

#ifdef __cplusplus
}
#endif

#endif
