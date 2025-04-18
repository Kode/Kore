#ifndef KORE_VULKAN_TEXTURE_FUNCTIONS_HEADER
#define KORE_VULKAN_TEXTURE_FUNCTIONS_HEADER

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_vulkan_texture_set_name(kore_gpu_texture *texture, const char *name);

uint32_t kore_vulkan_texture_image_layout_index(kore_gpu_texture *texture, uint32_t mip_level, uint32_t array_layer);

void kore_vulkan_texture_transition(kore_gpu_command_list *list, kore_gpu_texture *textur, VkImageLayout layout, uint32_t base_array_layer,
                                    uint32_t array_layer_count, uint32_t base_mip_level, uint32_t mip_level_count);

#ifdef __cplusplus
}
#endif

#endif
