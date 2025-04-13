#ifndef KORE_VULKAN_SAMPLER_STRUCTS_HEADER
#define KORE_VULKAN_SAMPLER_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_vulkan_sampler {
	VkSampler sampler;
	VkDevice  device;
} kore_vulkan_sampler;

#ifdef __cplusplus
}
#endif

#endif
