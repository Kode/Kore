#ifndef KORE_VULKAN_DEVICE_FUNCTIONS_HEADER
#define KORE_VULKAN_DEVICE_FUNCTIONS_HEADER

#include <kore3/gpu/device.h>

#include <kinc/math/matrix.h>

#include "descriptorset_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct VkDescriptorSetLayout;

void kore_vulkan_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist);

void kore_vulkan_device_destroy(kore_gpu_device *device);

void kore_vulkan_device_set_name(kore_gpu_device *device, const char *name);

void kore_vulkan_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer);

void kore_vulkan_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list);

void kore_vulkan_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture);

void kore_vulkan_device_create_descriptor_set(kore_gpu_device *device, VkDescriptorSetLayout *descriptor_set_layout, kore_vulkan_descriptor_set *set);

void kore_vulkan_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler);

kore_gpu_texture *kore_vulkan_device_get_framebuffer(kore_gpu_device *device);

kore_gpu_texture_format kore_vulkan_device_framebuffer_format(kore_gpu_device *device);

void kore_vulkan_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list);

void kore_vulkan_device_wait_until_idle(kore_gpu_device *device);

void kore_vulkan_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                 uint32_t index_count, kore_gpu_raytracing_volume *volume);

void kore_vulkan_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kinc_matrix4x4_t *volume_transforms,
                                                    uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy);

void kore_vulkan_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set);

void kore_vulkan_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence);

uint32_t kore_vulkan_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes);

void kore_vulkan_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value);

void kore_vulkan_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value);

#ifdef __cplusplus
}
#endif

#endif
