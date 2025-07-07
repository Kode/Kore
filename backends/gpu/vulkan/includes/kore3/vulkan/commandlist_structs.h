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
	kore_vulkan_buffer *buffer;
	uint64_t            offset;
	uint64_t            size;
} kore_vulkan_buffer_access;

typedef enum kore_vulkan_render_pass_status {
	KORE_VULKAN_RENDER_PASS_STATUS_SET,
	KORE_VULKAN_RENDER_PASS_STATUS_ACTIVE,
	KORE_VULKAN_RENDER_PASS_STATUS_PAUSED,
	KORE_VULKAN_RENDER_PASS_STATUS_NONE,
} kore_vulkan_render_pass_status;

#define KORE_VULKAN_INTERNAL_COMMAND_BUFFER_COUNT 3

#define KORE_VULKAN_COMMAND_LIST_MAX_QUEUED_BUFFER_ACCESSES 256

typedef struct kore_vulkan_command_list {
	VkDevice      device;
	VkCommandPool command_pool;

	// use multiple command buffers to avoid waits in kore_vulkan_device_execute_command_list
	VkCommandBuffer command_buffers[KORE_VULKAN_INTERNAL_COMMAND_BUFFER_COUNT];
	uint64_t        command_buffer_execution_indices[KORE_VULKAN_INTERNAL_COMMAND_BUFFER_COUNT];
	uint32_t        active_command_buffer;

	bool framebuffer_access;
	bool presenting;

	kore_vulkan_render_pass_status  render_pass_status;
	kore_gpu_render_pass_parameters current_render_pass;

	kore_vulkan_buffer_access queued_buffer_accesses[KORE_VULKAN_COMMAND_LIST_MAX_QUEUED_BUFFER_ACCESSES];
	uint32_t                  queued_buffer_accesses_count;

	bool has_dynamic_rendering;
} kore_vulkan_command_list;

#ifdef __cplusplus
}
#endif

#endif
