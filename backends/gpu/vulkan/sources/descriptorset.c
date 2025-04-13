#include <kore3/vulkan/descriptorset_functions.h>
#include <kore3/vulkan/descriptorset_structs.h>

#include <kore3/vulkan/texture_functions.h>

#include <kore3/util/align.h>

void kore_vulkan_descriptor_set_set_uniform_buffer_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set, kore_gpu_buffer *buffer,
                                                              uint32_t index) {
	VkDescriptorBufferInfo buffer_info = {
	    .buffer = buffer->vulkan.buffer,
	    .offset = 0,
	    .range  = buffer->vulkan.size,
	};

	VkWriteDescriptorSet write = {
	    .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	    .dstSet          = set->descriptor_set,
	    .dstBinding      = index,
	    .descriptorCount = 1,
	    .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	    .pBufferInfo     = &buffer_info,
	};

	vkUpdateDescriptorSets(device->vulkan.device, 1, &write, 0, NULL);
}

void kore_vulkan_descriptor_set_set_dynamic_uniform_buffer_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set, kore_gpu_buffer *buffer,
                                                                      uint32_t range, uint32_t index) {
	VkDescriptorBufferInfo buffer_info = {
	    .buffer = buffer->vulkan.buffer,
	    .offset = 0,
	    .range  = range,
	};

	VkWriteDescriptorSet write = {
	    .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	    .dstSet          = set->descriptor_set,
	    .dstBinding      = index,
	    .descriptorCount = 1,
	    .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
	    .pBufferInfo     = &buffer_info,
	};

	vkUpdateDescriptorSets(device->vulkan.device, 1, &write, 0, NULL);
}

void kore_vulkan_descriptor_set_set_texture_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                       uint32_t index) {
	VkImageViewCreateInfo view_create_info = {
	    .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	    .pNext      = NULL,
	    .image      = texture_view->texture->vulkan.image,
	    .viewType   = VK_IMAGE_VIEW_TYPE_2D,
	    .format     = convert_to_vulkan_format(texture_view->texture->vulkan.format),
	    .components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A},
	    .subresourceRange =
	        {
	            .aspectMask   = kore_gpu_texture_format_is_depth(texture_view->texture->vulkan.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT,
	            .baseMipLevel = 0,
	            .levelCount   = 1,
	            .baseArrayLayer = 0,
	            .layerCount     = 1,
	        },
	    .flags = 0,
	};

	VkImageView image_view;
	VkResult    result = vkCreateImageView(device->vulkan.device, &view_create_info, NULL, &image_view);
	assert(result == VK_SUCCESS);

	VkDescriptorImageInfo image_info = {
	    .imageView   = image_view,
	    .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
	};

	VkWriteDescriptorSet write = {
	    .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	    .dstSet          = set->descriptor_set,
	    .dstBinding      = index,
	    .descriptorCount = 1,
	    .descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	    .pImageInfo      = &image_info,
	};

	vkUpdateDescriptorSets(device->vulkan.device, 1, &write, 0, NULL);
}

void kore_vulkan_descriptor_set_set_sampler(kore_gpu_device *device, kore_vulkan_descriptor_set *set, kore_gpu_sampler *sampler, uint32_t index) {
	VkDescriptorImageInfo image_info = {
	    .sampler     = sampler->vulkan.sampler,
	    .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
	};

	VkWriteDescriptorSet write = {
	    .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	    .dstSet          = set->descriptor_set,
	    .dstBinding      = index,
	    .descriptorCount = 1,
	    .descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER,
	    .pImageInfo      = &image_info,
	};

	vkUpdateDescriptorSets(device->vulkan.device, 1, &write, 0, NULL);
}

void kore_vulkan_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {}

void kore_vulkan_descriptor_set_prepare_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view, bool writable) {
	if (texture_view->texture->vulkan.image_layout != VK_IMAGE_LAYOUT_GENERAL) {
		VkImageMemoryBarrier barrier = {
		    .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		    .pNext         = NULL,
		    .srcAccessMask = 0,
		    .dstAccessMask = 0,
		    .oldLayout     = texture_view->texture->vulkan.image_layout,
		    .newLayout     = VK_IMAGE_LAYOUT_GENERAL,
		    .image         = texture_view->texture->vulkan.image,
		    .subresourceRange =
		        {
		            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
		            .baseMipLevel   = 0,
		            .levelCount     = 1,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		    .srcAccessMask = 0,
		    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		};

		vkCmdPipelineBarrier(list->vulkan.command_buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1,
		                     &barrier);

		texture_view->texture->vulkan.image_layout = VK_IMAGE_LAYOUT_GENERAL;
	}
}
