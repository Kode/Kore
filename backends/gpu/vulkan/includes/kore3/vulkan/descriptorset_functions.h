#ifndef KORE_VULKAN_DESCRIPTORSET_FUNCTIONS_HEADER
#define KORE_VULKAN_DESCRIPTORSET_FUNCTIONS_HEADER

#include "buffer_structs.h"
#include "descriptorset_structs.h"
#include "device_structs.h"

#include <kore3/gpu/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_vulkan_descriptor_set_set_uniform_buffer_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set, kore_gpu_buffer *buffer,
                                                              uint32_t index);
void kore_vulkan_descriptor_set_set_dynamic_uniform_buffer_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set, kore_gpu_buffer *buffer,
                                                                      uint32_t range, uint32_t index);
void kore_vulkan_descriptor_set_set_texture_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                       uint32_t index);
void kore_vulkan_descriptor_set_set_texture_array_view_srv(kore_gpu_device *device, kore_vulkan_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                           uint32_t index);
void kore_vulkan_descriptor_set_set_texture_cube_view_srv(kore_gpu_device *device, kore_vulkan_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                          uint32_t index);
void kore_vulkan_descriptor_set_set_sampler(kore_gpu_device *device, kore_vulkan_descriptor_set *set, kore_gpu_sampler *sampler, uint32_t index);

void kore_vulkan_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint64_t offset, uint64_t size);
void kore_vulkan_descriptor_set_prepare_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view, bool writable);

#ifdef __cplusplus
}
#endif

#endif
