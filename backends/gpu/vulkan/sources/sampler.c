#include <kore3/vulkan/sampler_functions.h>

#include "vulkanunit.h"

#include <kore3/gpu/sampler.h>

void kore_vulkan_sampler_set_name(kore_gpu_sampler *sampler, const char *name) {
	const VkDebugUtilsObjectNameInfoEXT name_info = {
	    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	    .objectType   = VK_OBJECT_TYPE_SAMPLER,
	    .objectHandle = (uint64_t)sampler->vulkan.sampler,
	    .pObjectName  = name,
	};
	vkSetDebugUtilsObjectName(sampler->vulkan.device, &name_info);
}
