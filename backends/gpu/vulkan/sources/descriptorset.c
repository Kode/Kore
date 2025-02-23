#include <kore3/vulkan/descriptorset_functions.h>
#include <kore3/vulkan/descriptorset_structs.h>

#include <kore3/vulkan/texture_functions.h>

#include <kore3/util/align.h>

void kore_vulkan_descriptor_set_set_texture_view(kore_gpu_device *device, kore_vulkan_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                 uint32_t index) {
	VkDescriptorImageInfo image_info = {
	    .imageView = texture_view->texture->vulkan.image_view,
	    .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
	};

	VkWriteDescriptorSet write = {
	    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	    .dstSet = set->descriptor_set,
	    .dstBinding = index,
	    .descriptorCount = 1,
	    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	    .pImageInfo = &image_info,
	};

	vkUpdateDescriptorSets(device->vulkan.device, 1, &write, 0, NULL);
}

void kore_vulkan_descriptor_set_set_sampler(kore_gpu_device *device, kore_vulkan_descriptor_set *set, kore_gpu_sampler *sampler, uint32_t index) {
	VkDescriptorImageInfo image_info = {
	    .sampler = sampler->vulkan.sampler,
	    .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
	};

	VkWriteDescriptorSet write = {
	    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	    .dstSet = set->descriptor_set,
	    .dstBinding = index,
	    .descriptorCount = 1,
	    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
	    .pImageInfo = &image_info,
	};

	vkUpdateDescriptorSets(device->vulkan.device, 1, &write, 0, NULL);
}

void kore_vulkan_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer) {}

void kore_vulkan_descriptor_set_prepare_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view) {}
