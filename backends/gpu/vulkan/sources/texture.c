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
