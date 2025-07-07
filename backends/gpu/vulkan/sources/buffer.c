#include <kore3/vulkan/buffer_functions.h>

#include <kore3/gpu/buffer.h>

static uint64_t find_max_execution_index_all(kore_gpu_buffer *buffer) {
	uint64_t max_execution_index = 0;

	for (uint32_t range_index = 0; range_index < buffer->vulkan.ranges_count; ++range_index) {
		uint64_t execution_index = buffer->vulkan.ranges[range_index].execution_index;
		if (execution_index > max_execution_index) {
			max_execution_index = execution_index;
		}
	}

	return max_execution_index;
}

static uint64_t find_max_execution_index(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	uint64_t max_execution_index = 0;

	for (uint32_t range_index = 0; range_index < buffer->vulkan.ranges_count; ++range_index) {
		kore_vulkan_buffer_range range = buffer->vulkan.ranges[range_index];

		if (range.size == UINT64_MAX || (offset >= range.offset && offset < range.offset + range.size) ||
		    (offset + size > range.offset && offset + size <= range.offset + range.size)) {
			uint64_t execution_index = buffer->vulkan.ranges[range_index].execution_index;
			if (execution_index > max_execution_index) {
				max_execution_index = execution_index;
			}
		}
	}

	return max_execution_index;
}

void kore_vulkan_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {
	VkDebugUtilsObjectNameInfoEXT name_info = {
	    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	    .objectType   = VK_OBJECT_TYPE_BUFFER,
	    .objectHandle = (uint64_t)buffer->vulkan.buffer,
	    .pObjectName  = name,
	};
	vulkanSetDebugUtilsObjectName(buffer->vulkan.device->vulkan.device, &name_info);
}

void kore_vulkan_buffer_destroy(kore_gpu_buffer *buffer) {
	vkFreeMemory(buffer->vulkan.device->vulkan.device, buffer->vulkan.memory, NULL);
	vkDestroyBuffer(buffer->vulkan.device->vulkan.device, buffer->vulkan.buffer, NULL);
}

void *kore_vulkan_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	if (find_completed_execution(buffer->vulkan.device) >= find_max_execution_index_all(buffer)) {
		buffer->vulkan.locked_data_offset = 0;
		buffer->vulkan.locked_data_size   = UINT64_MAX;

		VkResult result = vkMapMemory(buffer->vulkan.device->vulkan.device, buffer->vulkan.memory, 0, buffer->vulkan.size, 0, &buffer->vulkan.locked_data);
		assert(result == VK_SUCCESS);
		return buffer->vulkan.locked_data;
	}
	else {
		return NULL;
	}
}

void *kore_vulkan_buffer_lock_all(kore_gpu_buffer *buffer) {
	wait_for_execution(buffer->vulkan.device, find_max_execution_index_all(buffer));

	buffer->vulkan.locked_data_offset = 0;
	buffer->vulkan.locked_data_size   = UINT64_MAX;

	VkResult result = vkMapMemory(buffer->vulkan.device->vulkan.device, buffer->vulkan.memory, 0, buffer->vulkan.size, 0, &buffer->vulkan.locked_data);
	assert(result == VK_SUCCESS);
	return buffer->vulkan.locked_data;
}

void *kore_vulkan_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	if (find_completed_execution(buffer->vulkan.device) >= find_max_execution_index(buffer, offset, size)) {
		buffer->vulkan.locked_data_offset = offset;
		buffer->vulkan.locked_data_size   = size;

		VkResult result = vkMapMemory(buffer->vulkan.device->vulkan.device, buffer->vulkan.memory, offset, size, 0, &buffer->vulkan.locked_data);
		assert(result == VK_SUCCESS);
		return buffer->vulkan.locked_data;
	}
	else {
		return NULL;
	}
}

void *kore_vulkan_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	wait_for_execution(buffer->vulkan.device, find_max_execution_index(buffer, offset, size));

	buffer->vulkan.locked_data_offset = offset;
	buffer->vulkan.locked_data_size   = size;

	VkResult result = vkMapMemory(buffer->vulkan.device->vulkan.device, buffer->vulkan.memory, offset, size, 0, &buffer->vulkan.locked_data);
	assert(result == VK_SUCCESS);
	return buffer->vulkan.locked_data;
}

void kore_vulkan_buffer_unlock(kore_gpu_buffer *buffer) {
	vkUnmapMemory(buffer->vulkan.device->vulkan.device, buffer->vulkan.memory);
}
