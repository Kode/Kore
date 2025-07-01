#include <kore3/vulkan/buffer_functions.h>

#include <kore3/gpu/buffer.h>

void kore_vulkan_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {
	VkDebugUtilsObjectNameInfoEXT name_info = {
	    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	    .objectType   = VK_OBJECT_TYPE_BUFFER,
	    .objectHandle = (uint64_t)buffer->vulkan.buffer,
	    .pObjectName  = name,
	};
	vulkanSetDebugUtilsObjectName(buffer->vulkan.device, &name_info);
}

void kore_vulkan_buffer_destroy(kore_gpu_buffer *buffer) {
	vkFreeMemory(buffer->vulkan.device, buffer->vulkan.memory, NULL);
	vkDestroyBuffer(buffer->vulkan.device, buffer->vulkan.buffer, NULL);
}

void *kore_vulkan_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	buffer->vulkan.locked_data_offset = 0;
	buffer->vulkan.locked_data_size   = UINT64_MAX;

	VkResult result = vkMapMemory(buffer->vulkan.device, buffer->vulkan.memory, 0, buffer->vulkan.size, 0, &buffer->vulkan.locked_data);
	assert(result == VK_SUCCESS);
	return buffer->vulkan.locked_data;
}

void *kore_vulkan_buffer_lock_all(kore_gpu_buffer *buffer) {
	buffer->vulkan.locked_data_offset = 0;
	buffer->vulkan.locked_data_size   = UINT64_MAX;

	VkResult result = vkMapMemory(buffer->vulkan.device, buffer->vulkan.memory, 0, buffer->vulkan.size, 0, &buffer->vulkan.locked_data);
	assert(result == VK_SUCCESS);
	return buffer->vulkan.locked_data;
}

void *kore_vulkan_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	buffer->vulkan.locked_data_offset = offset;
	buffer->vulkan.locked_data_size   = size;

	VkResult result = vkMapMemory(buffer->vulkan.device, buffer->vulkan.memory, offset, size, 0, &buffer->vulkan.locked_data);
	assert(result == VK_SUCCESS);
	return buffer->vulkan.locked_data;
}

void *kore_vulkan_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	buffer->vulkan.locked_data_offset = offset;
	buffer->vulkan.locked_data_size   = size;

	VkResult result = vkMapMemory(buffer->vulkan.device, buffer->vulkan.memory, offset, size, 0, &buffer->vulkan.locked_data);
	assert(result == VK_SUCCESS);
	return buffer->vulkan.locked_data;
}

void kore_vulkan_buffer_unlock(kore_gpu_buffer *buffer) {
	vkUnmapMemory(buffer->vulkan.device, buffer->vulkan.memory);
}
