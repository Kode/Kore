#ifndef KORE_VULKAN_TEXTURE_STRUCTS_HEADER
#define KORE_VULKAN_TEXTURE_STRUCTS_HEADER

#include <kore3/vulkan/vulkanmini.h>

#ifdef __cplusplus
extern "C" {
#endif

// add-ons to kore_gpu_texture_usage, top 16 bits only
typedef enum kore_vulkan_texture_usage {
	KORE_VULKAN_TEXTURE_USAGE_SAMPLED = 0x00010000,
	KORE_VULKAN_TEXTURE_USAGE_STORAGE = 0x00020000,
} kore_vulkan_texture_usage;

typedef struct kore_vulkan_texture {
	VkDeviceMemory device_memory;
	VkDeviceSize   device_size;

	VkImage       image;
	VkImageLayout image_layouts[64];

	kore_gpu_texture_format format;
	uint32_t                mip_level_count;

	bool is_framebuffer;

	VkDevice device;
} kore_vulkan_texture;

#ifdef __cplusplus
}
#endif

#endif
