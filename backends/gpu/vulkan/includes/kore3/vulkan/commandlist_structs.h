#ifndef KORE_VULKAN_COMMANDLIST_STRUCTS_HEADER
#define KORE_VULKAN_COMMANDLIST_STRUCTS_HEADER

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

typedef enum kore_vulkan_render_pass_status {
	KORE_VULKAN_RENDER_PASS_STATUS_SET,
	KORE_VULKAN_RENDER_PASS_STATUS_ACTIVE,
	KORE_VULKAN_RENDER_PASS_STATUS_PAUSED,
	KORE_VULKAN_RENDER_PASS_STATUS_NONE,
} kore_vulkan_render_pass_status;

typedef struct kore_vulkan_command_list {
	VkDevice        device;
	VkCommandPool   command_pool;
	VkCommandBuffer command_buffer;
	VkFence         fence;
	bool            presenting;

	kore_vulkan_render_pass_status  render_pass_status;
	kore_gpu_render_pass_parameters current_render_pass;

	bool has_dynamic_rendering;
} kore_vulkan_command_list;

#ifdef __cplusplus
}
#endif

#endif
