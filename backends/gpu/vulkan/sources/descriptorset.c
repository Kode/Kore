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

void kore_vulkan_descriptor_set_set_sampled_image_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set,
                                                             const kore_gpu_texture_view *texture_view, uint32_t index) {
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
	            .baseMipLevel = texture_view->base_mip_level,
	            .levelCount   = texture_view->mip_level_count,
	            .baseArrayLayer = texture_view->base_array_layer,
	            .layerCount     = texture_view->array_layer_count,
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

void kore_vulkan_descriptor_set_set_storage_image_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set,
                                                             const kore_gpu_texture_view *texture_view, uint32_t index) {
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
	            .baseMipLevel = texture_view->base_mip_level,
	            .levelCount   = texture_view->mip_level_count,
	            .baseArrayLayer = texture_view->base_array_layer,
	            .layerCount     = texture_view->array_layer_count,
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
	    .descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
	    .pImageInfo      = &image_info,
	};

	vkUpdateDescriptorSets(device->vulkan.device, 1, &write, 0, NULL);
}

void kore_vulkan_descriptor_set_set_sampled_image_array_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set,
                                                                   const kore_gpu_texture_view *texture_view, uint32_t index) {
	VkImageViewCreateInfo view_create_info = {
	    .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	    .pNext      = NULL,
	    .image      = texture_view->texture->vulkan.image,
	    .viewType   = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
	    .format     = convert_to_vulkan_format(texture_view->texture->vulkan.format),
	    .components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A},
	    .subresourceRange =
	        {
	            .aspectMask   = kore_gpu_texture_format_is_depth(texture_view->texture->vulkan.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT,
	            .baseMipLevel = texture_view->base_mip_level,
	            .levelCount   = texture_view->mip_level_count,
	            .baseArrayLayer = texture_view->base_array_layer,
	            .layerCount     = texture_view->array_layer_count,
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

void kore_vulkan_descriptor_set_set_sampled_cube_image_descriptor(kore_gpu_device *device, kore_vulkan_descriptor_set *set,
                                                                  const kore_gpu_texture_view *texture_view, uint32_t index) {
	assert(!kore_gpu_texture_format_is_depth(texture_view->texture->vulkan.format));

	VkImageViewCreateInfo view_create_info = {
	    .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	    .pNext      = NULL,
	    .image      = texture_view->texture->vulkan.image,
	    .viewType   = VK_IMAGE_VIEW_TYPE_CUBE,
	    .format     = convert_to_vulkan_format(texture_view->texture->vulkan.format),
	    .components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A},
	    .subresourceRange =
	        {
	            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
	            .baseMipLevel   = texture_view->base_mip_level,
	            .levelCount     = texture_view->mip_level_count,
	            .baseArrayLayer = texture_view->base_array_layer,
	            .layerCount     = texture_view->array_layer_count,
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

void kore_vulkan_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	if (buffer->vulkan.host_visible) {
		kore_vulkan_command_list_queue_buffer_access(list, &buffer->vulkan, (uint32_t)offset, (uint32_t)size);
	}
}

void kore_vulkan_descriptor_set_prepare_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view, bool writable) {
	if (writable && texture_view->texture->vulkan.is_framebuffer) {
		list->vulkan.framebuffer_access = true;
	}

	kore_vulkan_texture_transition(list, texture_view->texture, VK_IMAGE_LAYOUT_GENERAL, texture_view->base_array_layer, texture_view->array_layer_count,
	                               texture_view->base_mip_level, texture_view->mip_level_count);
}

void kore_vulkan_desciptor_set_use_free_allocation(kore_vulkan_descriptor_set *set) {}
