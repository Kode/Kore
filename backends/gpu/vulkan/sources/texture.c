#include <kore3/vulkan/texture_functions.h>

#include "vulkanunit.h"

#include <kore3/gpu/texture.h>

void kore_vulkan_texture_set_name(kore_gpu_texture *texture, const char *name) {
	const VkDebugUtilsObjectNameInfoEXT name_info = {
	    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	    .objectType   = VK_OBJECT_TYPE_IMAGE,
	    .objectHandle = (uint64_t)texture->vulkan.image,
	    .pObjectName  = name,
	};
	vkSetDebugUtilsObjectName(texture->vulkan.device, &name_info);
}

uint32_t kore_vulkan_texture_image_layout_index(kore_gpu_texture *texture, uint32_t mip_level, uint32_t array_layer) {
	return mip_level + (array_layer * texture->vulkan.mip_level_count);
}

void kore_vulkan_texture_transition(kore_gpu_command_list *list, kore_gpu_texture *texture, VkImageLayout layout, uint32_t base_array_layer,
                                    uint32_t array_layer_count, uint32_t base_mip_level, uint32_t mip_level_count) {
	for (uint32_t array_layer = base_array_layer; array_layer < base_array_layer + array_layer_count; ++array_layer) {
		for (uint32_t mip_level = base_mip_level; mip_level < base_mip_level + mip_level_count; ++mip_level) {
			if (texture->vulkan.image_layouts[kore_vulkan_texture_image_layout_index(texture, mip_level, array_layer)] != layout) {
				VkImageMemoryBarrier barrier = {
				    .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				    .pNext         = NULL,
				    .srcAccessMask = 0,
				    .dstAccessMask = 0,
				    .oldLayout     = texture->vulkan.image_layouts[kore_vulkan_texture_image_layout_index(texture, mip_level, array_layer)],
				    .newLayout     = layout,
				    .image         = texture->vulkan.image,
				    .subresourceRange =
				        {
				            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
				            .baseMipLevel   = mip_level,
				            .levelCount     = 1,
				            .baseArrayLayer = array_layer,
				            .layerCount     = 1,
				        },
				    .srcAccessMask = 0,
				    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				};

				vkCmdPipelineBarrier(list->vulkan.command_buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL,
				                     1, &barrier);

				texture->vulkan.image_layouts[kore_vulkan_texture_image_layout_index(texture, mip_level, array_layer)] = layout;
			}
		}
	}
}
