#ifndef KORE_VULKAN_COMMANDLIST_STRUCTS_HEADER
#define KORE_VULKAN_COMMANDLIST_STRUCTS_HEADER

#include <kore3/util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_vulkan_device;
struct kore_vulkan_texture;
struct kore_vulkan_compute_pipeline;
struct kore_vulkan_ray_pipeline;
struct kore_vulkan_rendery_pipeline;
struct kore_vulkan_descriptor_set;
struct kore_g5_query_set;

typedef struct kore_vulkan_buffer_access {
	int nothing;
} kore_vulkan_buffer_access;

typedef struct kore_vulkan_command_list {
	VkDevice device;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	VkFence fence;
	bool presenting;
} kore_vulkan_command_list;

#ifdef __cplusplus
}
#endif

#endif
