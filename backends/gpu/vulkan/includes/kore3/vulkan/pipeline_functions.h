#ifndef KORE_VULKAN_PIPELINE_FUNCTIONS_HEADER
#define KORE_VULKAN_PIPELINE_FUNCTIONS_HEADER

#include "device_structs.h"
#include "pipeline_structs.h"

#include "vulkanmini.h"

#ifdef __cplusplus
extern "C" {
#endif

void kore_vulkan_render_pipeline_init(kore_vulkan_device *device, kore_vulkan_render_pipeline *pipeline,
                                      const kore_vulkan_render_pipeline_parameters *parameters, const VkDescriptorSetLayout *descriptor_set_layouts,
                                      uint32_t descriptor_set_layouts_count);

void kore_vulkan_render_pipeline_destroy(kore_vulkan_render_pipeline *pipeline);

void kore_vulkan_compute_pipeline_init(kore_vulkan_device *device, kore_vulkan_compute_pipeline *pipeline,
                                       const kore_vulkan_compute_pipeline_parameters *parameters, const VkDescriptorSetLayout *descriptor_set_layouts,
                                       uint32_t descriptor_set_layouts_count);

void kore_vulkan_compute_pipeline_destroy(kore_vulkan_compute_pipeline *pipeline);

void kore_vulkan_ray_pipeline_init(kore_gpu_device *device, kore_vulkan_ray_pipeline *pipeline, const kore_vulkan_ray_pipeline_parameters *parameters);

void kore_vulkan_ray_pipeline_destroy(kore_vulkan_ray_pipeline *pipeline);

#ifdef __cplusplus
}
#endif

#endif
