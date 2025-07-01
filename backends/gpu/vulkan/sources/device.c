#include <kore3/vulkan/device_functions.h>

#include "vulkanunit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#ifdef KORE_WINDOWS
#include <kore3/backend/windows.h>
#endif

#ifdef KORE_LINUX
#include <kore3/backend/linux.h>
#endif

#include <kore3/error.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/window.h>

#include <assert.h>
#include <stdlib.h>

static VkInstance       instance;
static VkPhysicalDevice gpu;
static VkSwapchainKHR   swapchain = VK_NULL_HANDLE;
static kore_gpu_texture framebuffers[4];
static VkFormat         framebuffer_format;
static uint32_t         framebuffer_count = 0;
static uint32_t         framebuffer_index = 0;
static uint32_t         graphics_queue_family_index;
static VkSurfaceKHR     surface           = VK_NULL_HANDLE;
static bool             surface_destroyed = true;
#ifdef VALIDATE
static bool                     validation;
static VkDebugUtilsMessengerEXT debug_utils_messenger;
#else
static const bool validation = false;
#endif

static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_types,
                               const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
	if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Vulkan ERROR: Code %d : %s", callback_data->messageIdNumber, callback_data->pMessage);
		kore_debug_break();
	}
	else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		kore_log(KORE_LOG_LEVEL_WARNING, "Vulkan WARNING: Code %d : %s", callback_data->messageIdNumber, callback_data->pMessage);
	}
	return VK_FALSE;
}

#ifndef KORE_ANDROID
static VKAPI_ATTR void *VKAPI_CALL vulkan_realloc(void *pUserData, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
#ifdef _MSC_VER
	return _aligned_realloc(pOriginal, size, alignment);
#else
	return realloc(pOriginal, size);
#endif
}

static VKAPI_ATTR void *VKAPI_CALL vulkan_alloc(void *pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
#ifdef _MSC_VER
	return _aligned_malloc(size, alignment);
#else
	void *ptr;

	if (alignment % sizeof(void *) != 0) {
		alignment *= (sizeof(void *) / alignment);
	}

	if (posix_memalign(&ptr, alignment, size) != 0) {
		return NULL;
	}
	return ptr;
#endif
}

static VKAPI_ATTR void VKAPI_CALL vulkan_free(void *pUserData, void *pMemory) {
#ifdef _MSC_VER
	_aligned_free(pMemory);
#else
	free(pMemory);
#endif
}
#endif

static VkSemaphore framebuffer_availables[4];
static uint32_t    framebuffer_available_index = 0;

static void init_framebuffer_availables(kore_gpu_device *device) {
	const VkSemaphoreCreateInfo semaphore_create_info = {
	    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	};

	for (int i = 0; i < 4; ++i) {
		VkResult result = vkCreateSemaphore(device->vulkan.device, &semaphore_create_info, NULL, &framebuffer_availables[i]);
		assert(result == VK_SUCCESS);
	}
}

static VkSemaphore *get_next_framebuffer_available_semaphore(void) {
	framebuffer_available_index = (framebuffer_available_index + 1) % 4;
	return &framebuffer_availables[framebuffer_available_index];
}

static VkSemaphore *get_framebuffer_available_semaphore(void) {
	return &framebuffer_availables[framebuffer_available_index];
}

typedef struct extension_request {
	const char *name;
	bool        optional;
	bool        found;
} extension_request;

static void check_extensions(extension_request *requests, uint32_t requests_count, VkExtensionProperties *extension_properties,
                             uint32_t extension_properties_count) {
	for (uint32_t request_index = 0; request_index < requests_count; ++request_index) {
		for (uint32_t extension_property_index = 0; extension_property_index < extension_properties_count; ++extension_property_index) {
			if (strcmp(requests[request_index].name, extension_properties[extension_property_index].extensionName) == 0) {
				requests[request_index].found = true;
				break;
			}
		}

		if (!requests[request_index].found) {
			if (requests[request_index].optional) {
				kore_log(KORE_LOG_LEVEL_WARNING, "Extension %s not found.", requests[request_index].name);
			}
			else {
				kore_error_message("Required extension %s not found.", requests[request_index].name);
			}
		}
	}
}

static VkExtensionProperties instance_extension_properties[256];

static void check_instance_extensions(extension_request *requests, uint32_t requests_count) {
	uint32_t instance_extension_properties_count = sizeof(instance_extension_properties) / sizeof(instance_extension_properties[0]);

	VkResult result = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_properties_count, instance_extension_properties);
	assert(result == VK_SUCCESS);

	check_extensions(requests, requests_count, instance_extension_properties, instance_extension_properties_count);
}

static VkExtensionProperties device_extension_properties[256];

static void check_device_extensions(extension_request *requests, uint32_t requests_count) {
	uint32_t device_extension_properties_count = sizeof(device_extension_properties) / sizeof(device_extension_properties[0]);

	VkResult result = vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_properties_count, device_extension_properties);
	assert(result == VK_SUCCESS);

	check_extensions(requests, requests_count, device_extension_properties, device_extension_properties_count);
}

static bool check_layers(const char **layers, int layers_count, VkLayerProperties *layer_properties, int layer_properties_count) {
	for (int layer_index = 0; layer_index < layers_count; ++layer_index) {
		bool found = false;

		for (int layer_property_index = 0; layer_property_index < layer_properties_count; ++layer_property_index) {
			if (strcmp(layers[layer_index], layer_properties[layer_property_index].layerName) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			kore_log(KORE_LOG_LEVEL_WARNING, "Failed to find extension %s", layers[layer_index]);
			return false;
		}
	}

	return true;
}

static VkLayerProperties instance_layer_properties[256];

static bool check_instance_layers(const char **instance_layers, int instance_layers_count) {
	uint32_t instance_layer_properties_count = sizeof(instance_layer_properties) / sizeof(instance_layer_properties[0]);

	VkResult result = vkEnumerateInstanceLayerProperties(&instance_layer_properties_count, instance_layer_properties);
	assert(result == VK_SUCCESS);

	return check_layers(instance_layers, instance_layers_count, instance_layer_properties, instance_layer_properties_count);
}

static VkLayerProperties device_layer_properties[256];

static bool check_device_layers(const char **device_layers, int device_layers_count) {
	uint32_t device_layer_properties_count = sizeof(device_layer_properties) / sizeof(device_layer_properties[0]);

	VkResult result = vkEnumerateDeviceLayerProperties(gpu, &device_layer_properties_count, device_layer_properties);
	assert(result == VK_SUCCESS);

	return check_layers(device_layers, device_layers_count, device_layer_properties, device_layer_properties_count);
}

static void load_instance_functions(void) {
#define GET_INSTANCE_FUNCTION(entrypoint, ext)                                                                \
	{                                                                                                         \
		vulkan##entrypoint = (PFN_vk##entrypoint##ext)vkGetInstanceProcAddr(instance, "vk" #entrypoint #ext); \
		if (vulkan##entrypoint == NULL) {                                                                     \
			kore_error_message("vkGetInstanceProcAddr failed to find vk" #entrypoint);                        \
		}                                                                                                     \
	}

	GET_INSTANCE_FUNCTION(CmdBeginDebugUtilsLabel, EXT);
	GET_INSTANCE_FUNCTION(CmdEndDebugUtilsLabel, EXT);
	GET_INSTANCE_FUNCTION(CmdInsertDebugUtilsLabel, EXT);
	GET_INSTANCE_FUNCTION(SetDebugUtilsObjectName, EXT);
	GET_INSTANCE_FUNCTION(CreateDebugUtilsMessenger, EXT);
	GET_INSTANCE_FUNCTION(DestroyDebugUtilsMessenger, EXT);

	GET_INSTANCE_FUNCTION(CmdBeginRendering, KHR);
	GET_INSTANCE_FUNCTION(CmdEndRendering, KHR);

#undef GET_INSTANCE_FUNCTION
}

static void load_device_functions(VkDevice device) {
#define GET_DEVICE_FUNCTION(entrypoint, ext)                                                              \
	{                                                                                                     \
		vulkan##entrypoint = (PFN_vk##entrypoint##ext)vkGetDeviceProcAddr(device, "vk" #entrypoint #ext); \
		if (vulkan##entrypoint == NULL) {                                                                 \
			kore_error_message("vkGetDeviceProcAddr failed to find vk" #entrypoint);                      \
		}                                                                                                 \
	}

	//

#undef GET_DEVICE_FUNCTION
}

static bool presentation_support(VkPhysicalDevice gpu, uint32_t queue_index) {
#if defined(KORE_WINDOWS)
	return vkGetPhysicalDeviceWin32PresentationSupportKHR(gpu, queue_index);
#elif defined(KORE_LINUX)
#ifndef KORE_NO_WAYLAND
	if (kore_linux_wayland()) {
		return kore_wayland_vulkan_get_physical_device_presentation_support(gpu, queue_index);
	}
	else {
#endif
		return kore_x11_vulkan_get_physical_device_presentation_support(gpu, queue_index);
#ifndef KORE_NO_WAYLAND
	}
#endif
#endif
}

void find_gpu(void) {
	VkPhysicalDevice physical_devices[64];
	uint32_t         gpu_count = sizeof(physical_devices) / sizeof(physical_devices[0]);

	VkResult result = vkEnumeratePhysicalDevices(instance, &gpu_count, physical_devices);

	if (result != VK_SUCCESS || gpu_count == 0) {
		kore_error_message("No Vulkan device found");
		return;
	}

	float best_score = -1.0;

	for (uint32_t gpu_index = 0; gpu_index < gpu_count; ++gpu_index) {
		VkPhysicalDevice current_gpu = physical_devices[gpu_index];

		VkQueueFamilyProperties queue_props[64];
		uint32_t                queue_count = sizeof(queue_props) / sizeof(queue_props[0]);
		vkGetPhysicalDeviceQueueFamilyProperties(current_gpu, &queue_count, queue_props);

		bool can_present = false;
		bool can_render  = false;

		for (uint32_t queue_index = 0; queue_index < queue_count; ++queue_index) {
			if (presentation_support(current_gpu, queue_index)) {
				can_present = true;
			}

			if ((queue_props[queue_index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
				can_render = true;
			}
		}

		if (!can_present || !can_render) {
			continue;
		}

		float score = 0.0;

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(current_gpu, &properties);

		switch (properties.deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score += 10;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score += 7;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			score += 5;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			score += 1;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			break;
		case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
			break;
		}

		if (score > best_score) {
			gpu        = current_gpu;
			best_score = score;
		}
	}

	if (gpu == VK_NULL_HANDLE) {
		kore_error_message("No Vulkan device that supports presentation found");
		return;
	}

	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(gpu, &properties);
	kore_log(KORE_LOG_LEVEL_INFO, "Chosen Vulkan device: %s", properties.deviceName);
}

uint32_t find_graphics_queue_family(void) {
	VkQueueFamilyProperties queue_family_props[16];
	uint32_t                queue_family_count = sizeof(queue_family_props) / sizeof(queue_family_props[0]);

	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_family_props);

	for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; ++queue_family_index) {
		if ((queue_family_props[queue_family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && presentation_support(gpu, queue_family_index)) {
			return queue_family_index;
		}
	}

	kore_error_message("Graphics or present queue not found");
	return 0;
}

static VkSurfaceFormatKHR find_surface_format(VkSurfaceKHR surface) {
	VkSurfaceFormatKHR surface_formats[256];

	uint32_t formats_count = sizeof(surface_formats) / sizeof(surface_formats[0]);
	VkResult result        = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formats_count, surface_formats);
	assert(result == VK_SUCCESS);

	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (formats_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED) {
		return surface_formats[0];
	}
	else {
		assert(formats_count >= 1);
		for (uint32_t i = 0; i < formats_count; ++i) {
			// avoid SRGB to avoid automatic gamma-correction
			if (surface_formats[i].format != VK_FORMAT_B8G8R8A8_SRGB) {
				return surface_formats[i];
			}
		}
		return surface_formats[0];
	}
}

static VkResult create_surface(VkInstance instance, int window_index, VkSurfaceKHR *surface) {
#if defined(KORE_WINDOWS)
	const VkWin32SurfaceCreateInfoKHR surface_create_info = {
	    .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
	    .pNext     = NULL,
	    .flags     = 0,
	    .hinstance = GetModuleHandle(NULL),
	    .hwnd      = kore_windows_window_handle(0),
	};
	return vkCreateWin32SurfaceKHR(instance, &surface_create_info, NULL, surface);
#elif defined(KORE_LINUX)
#ifndef KORE_NO_WAYLAND
	if (kore_linux_wayland()) {
		return kore_wayland_vulkan_create_surface(instance, window_index, surface);
	}
	else {
#endif
		return kore_x11_vulkan_create_surface(instance, window_index, surface);
#ifndef KORE_NO_WAYLAND
	}
#endif
#endif
}

static void create_swapchain(kore_gpu_device *device) {
	uint32_t window_width = kore_window_width(0);
	window_width          = window_width > 4 ? window_width : window_width;

	uint32_t window_height = kore_window_height(0);
	window_height          = window_height > 4 ? window_height : window_height;

	bool vsync = true; // kore_window_vsynced(0); // TODO

	VkResult result = VK_SUCCESS;

	if (surface_destroyed) {
		if (swapchain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(device->vulkan.device, swapchain, NULL);
			swapchain = VK_NULL_HANDLE;
		}

		if (surface == VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(instance, surface, NULL);
		}

		result = create_surface(instance, 0, &surface);
		assert(result == VK_SUCCESS);
		surface_destroyed = false;
	}

	VkBool32 surface_supported = false;
	result                     = vkGetPhysicalDeviceSurfaceSupportKHR(gpu, graphics_queue_family_index, surface, &surface_supported);
	assert(result == VK_SUCCESS && surface_supported);

	VkSurfaceFormatKHR format = find_surface_format(surface);
	framebuffer_format        = format.format;

	VkSurfaceCapabilitiesKHR surface_capabilities = {0};
	result                                        = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surface_capabilities);
	assert(result == VK_SUCCESS);

	VkPresentModeKHR present_modes[32];
	uint32_t         present_mode_count = sizeof(present_modes) / sizeof(present_modes[0]);
	result                              = vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, present_modes);
	assert(result == VK_SUCCESS);

	if (surface_capabilities.currentExtent.width != (uint32_t)-1) {
		window_width = surface_capabilities.currentExtent.width;
		window_width = window_width > 4 ? window_width : window_width;

		window_height = surface_capabilities.currentExtent.height;
		window_height = window_height > 4 ? window_height : window_height;
	}

	VkCompositeAlphaFlagBitsKHR composite_alpha;
	if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
		composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}
	else if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
		composite_alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	}
	else if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
		composite_alpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
	}
	else if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
		composite_alpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
	}
	else {
		kore_error_message("Vulkan driver problem, no supported composite alpha.");
	}

	const VkSwapchainCreateInfoKHR swapchain_info = {
	    .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
	    .pNext                 = NULL,
	    .surface               = surface,
	    .minImageCount         = surface_capabilities.minImageCount,
	    .imageFormat           = format.format,
	    .imageColorSpace       = format.colorSpace,
	    .imageExtent.width     = window_width,
	    .imageExtent.height    = window_height,
	    .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	    .preTransform          = (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
	                                                                                                                : surface_capabilities.currentTransform,
	    .compositeAlpha        = composite_alpha,
	    .imageArrayLayers      = 1,
	    .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices   = NULL,
	    .presentMode           = vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR,
	    .oldSwapchain          = VK_NULL_HANDLE,
	    .clipped               = true,
	    .oldSwapchain          = swapchain,
	};

	result = vkCreateSwapchainKHR(device->vulkan.device, &swapchain_info, NULL, &swapchain);
	assert(result == VK_SUCCESS);

	VkImage images[4];
	framebuffer_count = sizeof(images) / sizeof(images[0]);
	result            = vkGetSwapchainImagesKHR(device->vulkan.device, swapchain, &framebuffer_count, images);
	assert(result == VK_SUCCESS);

	for (uint32_t i = 0; i < framebuffer_count; ++i) {
		framebuffers[i].vulkan.image = images[i];
		for (uint32_t image_layout_index = 0;
		     image_layout_index < sizeof(framebuffers[i].vulkan.image_layouts) / sizeof(framebuffers[i].vulkan.image_layouts[0]); ++image_layout_index) {
			framebuffers[i].vulkan.image_layouts[image_layout_index] = VK_IMAGE_LAYOUT_UNDEFINED;
		}
		framebuffers[i].width         = window_width;
		framebuffers[i].height        = window_height;
		framebuffers[i].vulkan.format = convert_from_vulkan_format(framebuffer_format);
	}
}

static void create_descriptor_pool(kore_gpu_device *device) {
	VkDescriptorPoolSize pool_sizes[] = {
	    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 128}, {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 128},
	    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 128}, {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 128},
	    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 128},  {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 128},
	    {VK_DESCRIPTOR_TYPE_SAMPLER, 128},
	};

	VkDescriptorPoolCreateInfo pool_create_info = {
	    .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
	    .pNext         = NULL,
	    .maxSets       = 128 * sizeof(pool_sizes) / sizeof(pool_sizes[0]),
	    .poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]),
	    .pPoolSizes    = pool_sizes,
	};

	VkResult result = vkCreateDescriptorPool(device->vulkan.device, &pool_create_info, NULL, &device->vulkan.descriptor_pool);
	assert(result == VK_SUCCESS);
}

void kore_vulkan_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
	const char *instance_layers[64];
	int         instance_layers_count = 0;

#ifdef VALIDATE
	instance_layers[instance_layers_count++] = "VK_LAYER_KHRONOS_validation";
#endif

	if (check_instance_layers(instance_layers, instance_layers_count)) {
		kore_log(KORE_LOG_LEVEL_INFO, "Running with Vulkan validation layers enabled.");
#ifdef VALIDATE
		validation = true;
#endif
	}
	else {
		--instance_layers_count; // Remove VK_LAYER_KHRONOS_validation
	}

	extension_request instance_extensions[] = {
	    {
	        .name = VK_KHR_SURFACE_EXTENSION_NAME,
	    },
	    {
	        .name = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
	    },
	    {
	        .name     = VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	        .optional = true,
	    },
#if defined(KORE_WINDOWS)
	    {
	        .name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	    },
#elif defined(KORE_LINUX)
	    {
	        .name     = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
	        .optional = true,
	    },
	    {
	        .name     = VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
	        .optional = true,
	    },
#endif
	};

	check_instance_extensions(instance_extensions, KORE_ARRAY_SIZE(instance_extensions));

	const char *instance_extension_names[KORE_ARRAY_SIZE(instance_extensions)];
	for (uint32_t index = 0; index < KORE_ARRAY_SIZE(instance_extensions); ++index) {
		instance_extension_names[index] = instance_extensions[index].name;
	}

	const VkApplicationInfo application_info = {
	    .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	    .pNext              = NULL,
	    .pApplicationName   = kore_application_name(),
	    .applicationVersion = 0,
	    .pEngineName        = "Kore",
	    .engineVersion      = 0,
	    .apiVersion         = VK_API_VERSION_1_0,
	};

	const VkInstanceCreateInfo instance_create_info = {
	    .sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	    .pNext            = NULL,
	    .pApplicationInfo = &application_info,

	    .enabledLayerCount   = instance_layers_count,
	    .ppEnabledLayerNames = (const char *const *)instance_layers,

	    .enabledExtensionCount   = KORE_ARRAY_SIZE(instance_extension_names),
	    .ppEnabledExtensionNames = instance_extension_names,
	};

#ifndef KORE_ANDROID
	const VkAllocationCallbacks allocator_callbacks = {
	    .pfnAllocation   = vulkan_alloc,
	    .pfnFree         = vulkan_free,
	    .pfnReallocation = vulkan_realloc,
	};
	VkResult result = vkCreateInstance(&instance_create_info, &allocator_callbacks, &instance);
#else
	VkResult result = vkCreateInstance(&instance_create_info, NULL, &instance);
#endif
	switch (result) {
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		kore_error_message("Vulkan driver is incompatible");
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		kore_error_message("Vulkan extension not found");
	}

	if (result != VK_SUCCESS) {
		kore_error_message("Can not create Vulkan instance");
	}

	find_gpu();

	const char *device_layers[64];
	int         device_layers_count = 0;

	device_layers[device_layers_count++] = "VK_LAYER_KHRONOS_validation";

#ifdef VALIDATE
	if (check_device_layers(device_layers, device_layers_count)) {
		validation |= true;
	}
	else {
		--device_layers_count; // Remove VK_LAYER_KHRONOS_validation
	}
#endif

#ifndef VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME // For Dave's Debian
#define VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME "VK_KHR_format_feature_flags2"
#endif

	extension_request device_extensions[] = {
	    {
	        .name = VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	    },
	    {
	        // Allows negative viewport height to flip viewport
	        .name = VK_KHR_MAINTENANCE1_EXTENSION_NAME,
	    },
	    {
	        .name     = VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME,
	        .optional = true,
	    },
	    {
	        // required by VK_KHR_CREATE_RENDERPASS_2
	        .name     = VK_KHR_MULTIVIEW_EXTENSION_NAME,
	        .optional = true,
	    },
	    {
	        // required by VK_KHR_CREATE_RENDERPASS_2
	        .name     = VK_KHR_MAINTENANCE1_EXTENSION_NAME,
	        .optional = true,
	    },
	    {
	        // required by VK_KHR_DEPTH_STENCIL_RESOLVE
	        .name     = VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
	        .optional = true,
	    },
	    {
	        // required by VK_KHR_DYNAMIC_RENDERING
	        .name     = VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
	        .optional = true,
	    },
	    {
	        .name     = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
	        .optional = true,
	    },
#ifdef KORE_VKRT
	    {
	        .name = VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
	    },
	    {
	        .name = VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	    },
	    {
	        .name = VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
	    },
	    {
	        .name = VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	    },
	    {
	        .name = VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
	    },
	    {
	        .name = VK_KHR_SPIRV_1_4_EXTENSION_NAME,
	    },
	    {
	        .name = VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
	    },
#endif
	};

	check_device_extensions(device_extensions, KORE_ARRAY_SIZE(device_extensions));

	assert(device_extensions[5].name == VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
	device->vulkan.has_dynamic_rendering = device_extensions[5].found;

	const char *device_extension_names[KORE_ARRAY_SIZE(device_extensions)];

	for (uint32_t index = 0; index < KORE_ARRAY_SIZE(device_extensions); ++index) {
		device_extension_names[index] = device_extensions[index].name;
	}

	load_instance_functions();
	load_device_functions(device->vulkan.device);

#ifdef VALIDATE
	if (validation) {
		const VkDebugUtilsMessengerCreateInfoEXT create_info = {
		    .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		    .flags           = 0,
		    .pfnUserCallback = debug_callback,
		    .pUserData       = NULL,
		    .pNext           = NULL,
		    .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		};

		result = vulkanCreateDebugUtilsMessenger(instance, &create_info, NULL, &debug_utils_messenger);
		assert(result == VK_SUCCESS);
	}
#endif

	graphics_queue_family_index = find_graphics_queue_family();

	const float queue_priorities[1] = {0.0};

	const VkDeviceQueueCreateInfo queue_create_info = {
	    .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
	    .pNext            = NULL,
	    .queueFamilyIndex = graphics_queue_family_index,
	    .queueCount       = 1,
	    .pQueuePriorities = queue_priorities,
	};

	const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering = {
	    .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
	    .pNext            = NULL,
	    .dynamicRendering = VK_TRUE,
	};

#ifdef KORE_VKRT
	const VkPhysicalDeviceRayTracingPipelineFeaturesKHR raytracing_pipeline = {
	    .sType              = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
	    .pNext              = &dynamic_rendering,
	    .rayTracingPipeline = VK_TRUE,
	};

	const VkPhysicalDeviceAccelerationStructureFeaturesKHR raytracing_acceleration_structure = {
	    .sType                 = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
	    .pNext                 = &raytracing_pipeline,
	    .accelerationStructure = VK_TRUE,
	};

	const VkPhysicalDeviceBufferDeviceAddressFeatures buffer_device_address = {
	    .sType               = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
	    .pNext               = &raytracing_acceleration_structure,
	    .bufferDeviceAddress = VK_TRUE,
	};
#endif

	VkDeviceCreateInfo device_create_info = {
	    .sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	    .pNext                = NULL,
	    .queueCreateInfoCount = 1,
	    .pQueueCreateInfos    = &queue_create_info,

	    .enabledLayerCount   = device_layers_count,
	    .ppEnabledLayerNames = (const char *const *)device_layers,

	    .enabledExtensionCount   = KORE_ARRAY_SIZE(device_extension_names),
	    .ppEnabledExtensionNames = device_extension_names,

#ifdef KORE_VKRT
	    .pNext = &buffer_device_address,
#else
	    .pNext = NULL,
#endif
	};

	if (device->vulkan.has_dynamic_rendering) {
		device_create_info.pNext = &dynamic_rendering;
	}

	result = vkCreateDevice(gpu, &device_create_info, NULL, &device->vulkan.device);
	assert(result == VK_SUCCESS);

	vkGetDeviceQueue(device->vulkan.device, graphics_queue_family_index, 0, &device->vulkan.queue);

	vkGetPhysicalDeviceMemoryProperties(gpu, &device->vulkan.device_memory_properties);

	const VkCommandPoolCreateInfo command_pool_create_info = {
	    .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	    .pNext            = NULL,
	    .queueFamilyIndex = graphics_queue_family_index,
	    .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	};

	result = vkCreateCommandPool(device->vulkan.device, &command_pool_create_info, NULL, &device->vulkan.command_pool);
	assert(result == VK_SUCCESS);

	create_swapchain(device);

	init_framebuffer_availables(device);

	create_descriptor_pool(device);
}

void kore_vulkan_device_destroy(kore_gpu_device *device) {
	vkDestroyCommandPool(device->vulkan.device, device->vulkan.command_pool, NULL);
	vkDestroyDevice(device->vulkan.device, NULL);
}

void kore_vulkan_device_set_name(kore_gpu_device *device, const char *name) {
	VkDebugUtilsObjectNameInfoEXT name_info = {
	    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	    .objectType   = VK_OBJECT_TYPE_DEVICE,
	    .objectHandle = (uint64_t)device->vulkan.device,
	    .pObjectName  = name,
	};
	vulkanSetDebugUtilsObjectName(device->vulkan.device, &name_info);
}

static bool memory_type_from_properties(kore_gpu_device *device, uint32_t type_bits, VkFlags requirements_mask, uint32_t *type_index) {
	for (uint32_t i = 0; i < 32; ++i) {
		if ((type_bits & 1) == 1) {
			if ((device->vulkan.device_memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				*type_index = i;
				return true;
			}
		}
		type_bits >>= 1;
	}
	return false;
}

void kore_vulkan_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	buffer->vulkan.device = device->vulkan.device;

	buffer->vulkan.size = parameters->size;

	VkBufferUsageFlags usage = 0;
	if ((parameters->usage_flags & KORE_VULKAN_BUFFER_USAGE_VERTEX) != 0) {
		usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_INDEX) != 0) {
		usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_INDIRECT) != 0) {
		usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	}
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_COPY_SRC) != 0) {
		usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	}
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_COPY_DST) != 0) {
		usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
#ifdef KORE_VKRT
	usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
#endif

	if (usage == 0 || usage == KORE_GPU_BUFFER_USAGE_COPY_SRC) {
		usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}

	const VkBufferCreateInfo create_info = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .pNext = NULL,
	    .size  = parameters->size,
	    .usage = usage,
	    .flags = 0,
	};

	VkResult result = vkCreateBuffer(device->vulkan.device, &create_info, NULL, &buffer->vulkan.buffer);
	assert(result == VK_SUCCESS);

	VkMemoryRequirements memory_requirements = {0};
	vkGetBufferMemoryRequirements(device->vulkan.device, buffer->vulkan.buffer, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {
	    .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .pNext           = NULL,
	    .memoryTypeIndex = 0,
	    .allocationSize  = memory_requirements.size,
	};

	bool host_visible = false;
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0 || (parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0) {
		host_visible = true;
	}

	bool memory_type_found = memory_type_from_properties(device, memory_requirements.memoryTypeBits,
	                                                     host_visible ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	                                                     &memory_allocate_info.memoryTypeIndex);
	assert(memory_type_found);

#ifdef KORE_VKRT
	const VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
	    .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR,
	    .pNext = &memory_allocate_flags_info,
	};
#endif

	result = vkAllocateMemory(device->vulkan.device, &memory_allocate_info, NULL, &buffer->vulkan.memory);
	assert(result == VK_SUCCESS);

	result = vkBindBufferMemory(device->vulkan.device, buffer->vulkan.buffer, buffer->vulkan.memory, 0);
	assert(result == VK_SUCCESS);
}

void kore_vulkan_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	list->vulkan.device                = device->vulkan.device;
	list->vulkan.has_dynamic_rendering = device->vulkan.has_dynamic_rendering;
	list->vulkan.command_pool          = device->vulkan.command_pool;
	list->vulkan.presenting            = false;
	list->vulkan.render_pass_status    = KORE_VULKAN_RENDER_PASS_STATUS_NONE;

	const VkFenceCreateInfo fence_create_info = {
	    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	};

	vkCreateFence(device->vulkan.device, &fence_create_info, NULL, &list->vulkan.fence);

	const VkCommandBufferAllocateInfo allocate_info = {
	    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	    .pNext              = NULL,
	    .commandPool        = device->vulkan.command_pool,
	    .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	    .commandBufferCount = 1,
	};

	VkResult result = vkAllocateCommandBuffers(device->vulkan.device, &allocate_info, &list->vulkan.command_buffer);
	assert(result == VK_SUCCESS);

	const VkCommandBufferBeginInfo begin_info = {
	    .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	    .pNext            = NULL,
	    .flags            = 0,
	    .pInheritanceInfo = NULL,
	};

	vkBeginCommandBuffer(list->vulkan.command_buffer, &begin_info);
}

void kore_vulkan_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
	texture->vulkan.device = device->vulkan.device;

	VkFormat format = convert_to_vulkan_format(parameters->format);

	VkFormatProperties format_properties;
	vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

	assert((format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0);

	texture->width                  = parameters->width;
	texture->height                 = parameters->height;
	texture->vulkan.format          = parameters->format;
	texture->vulkan.mip_level_count = parameters->mip_level_count;

	VkImageUsageFlags usage = 0;

	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_COPY_SRC) != 0) {
		usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_COPY_DST) != 0) {
		usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	if ((parameters->usage & KORE_VULKAN_TEXTURE_USAGE_SAMPLED) != 0) {
		usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}

	if ((parameters->usage & KORE_VULKAN_TEXTURE_USAGE_STORAGE) != 0) {
		usage |= VK_IMAGE_USAGE_STORAGE_BIT;
	}

	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_RENDER_ATTACHMENT) != 0) {
		if (kore_gpu_texture_format_is_depth(parameters->format)) {
			usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		else {
			usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
	}

	VkImageCreateInfo image_create_info = {
	    .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
	    .pNext         = NULL,
	    .imageType     = VK_IMAGE_TYPE_2D,
	    .format        = format,
	    .extent        = {parameters->width, parameters->height, 1},
	    .mipLevels     = parameters->mip_level_count,
	    .arrayLayers   = parameters->depth_or_array_layers,
	    .samples       = VK_SAMPLE_COUNT_1_BIT,
	    .tiling        = VK_IMAGE_TILING_OPTIMAL,
	    .usage         = usage,
	    .flags         = parameters->depth_or_array_layers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0,
	    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VkMemoryAllocateInfo memory_allocate_info = {
	    .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .pNext           = NULL,
	    .allocationSize  = 0,
	    .memoryTypeIndex = 0,
	};

	VkResult result = vkCreateImage(device->vulkan.device, &image_create_info, NULL, &texture->vulkan.image);
	assert(result == VK_SUCCESS);

	for (uint32_t image_layout_index = 0; image_layout_index < sizeof(texture->vulkan.image_layouts) / sizeof(texture->vulkan.image_layouts[0]);
	     ++image_layout_index) {
		texture->vulkan.image_layouts[image_layout_index] = VK_IMAGE_LAYOUT_UNDEFINED;
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device->vulkan.device, texture->vulkan.image, &memory_requirements);

	texture->vulkan.device_size = memory_allocate_info.allocationSize = memory_requirements.size;

	bool memory_type_found =
	    memory_type_from_properties(device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memory_allocate_info.memoryTypeIndex);
	assert(memory_type_found);

	result = vkAllocateMemory(device->vulkan.device, &memory_allocate_info, NULL, &texture->vulkan.device_memory);
	assert(result == VK_SUCCESS);

	result = vkBindImageMemory(device->vulkan.device, texture->vulkan.image, texture->vulkan.device_memory, 0);
	assert(result == VK_SUCCESS);
}

kore_gpu_texture *kore_vulkan_device_get_framebuffer(kore_gpu_device *device) {
	VkSemaphore next_framebuffer_available = *get_next_framebuffer_available_semaphore();

	VkResult err;
	do {
		err = vkAcquireNextImageKHR(device->vulkan.device, swapchain, UINT64_MAX, next_framebuffer_available, VK_NULL_HANDLE, &framebuffer_index);
		if (err == VK_ERROR_SURFACE_LOST_KHR) {
			surface_destroyed = true;
			create_swapchain(device);
		}
		else if (err == VK_ERROR_OUT_OF_DATE_KHR) {
			create_swapchain(device);
		}
	} while (err != VK_SUCCESS && err != VK_SUBOPTIMAL_KHR);

	return &framebuffers[framebuffer_index];
}

kore_gpu_texture_format kore_vulkan_device_framebuffer_format(kore_gpu_device *device) {
	return convert_from_vulkan_format(framebuffer_format);
}

void kore_vulkan_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	kore_vulkan_texture_transition(list, &framebuffers[framebuffer_index], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0, 1, 0, 1);

	vkEndCommandBuffer(list->vulkan.command_buffer);

	VkPipelineStageFlags stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	VkSubmitInfo submit_info = {
	    .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	    .pNext                = NULL,
	    .waitSemaphoreCount   = 0,
	    .pWaitSemaphores      = NULL,
	    .pWaitDstStageMask    = &stage_mask,
	    .commandBufferCount   = 1,
	    .pCommandBuffers      = &list->vulkan.command_buffer,
	    .signalSemaphoreCount = 0,
	    .pSignalSemaphores    = NULL,
	};

	if (list->vulkan.presenting) {
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores    = get_framebuffer_available_semaphore();
	}

	VkResult result = vkQueueSubmit(device->vulkan.queue, 1, &submit_info, list->vulkan.fence);
	assert(result == VK_SUCCESS);

	if (list->vulkan.presenting) {
		const VkPresentInfoKHR present_info = {
		    .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		    .pNext              = NULL,
		    .swapchainCount     = 1,
		    .pSwapchains        = &swapchain,
		    .pImageIndices      = &framebuffer_index,
		    .pWaitSemaphores    = NULL,
		    .waitSemaphoreCount = 0,
		};

		result = vkQueuePresentKHR(device->vulkan.queue, &present_info);
		if (result == VK_ERROR_SURFACE_LOST_KHR) {
			result = vkDeviceWaitIdle(device->vulkan.device);
			assert(result == VK_SUCCESS);

			surface_destroyed = true;

			create_swapchain(device);
		}
		else if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			result = vkDeviceWaitIdle(device->vulkan.device);

			assert(result == VK_SUCCESS);

			create_swapchain(device);
		}
		else {
			assert(result == VK_SUCCESS);
		}
	}

	// TODO: Use multiple command buffers to avoid waits
	result = vkWaitForFences(device->vulkan.device, 1, &list->vulkan.fence, VK_TRUE, UINT64_MAX);
	assert(result == VK_SUCCESS);

	result = vkResetFences(device->vulkan.device, 1, &list->vulkan.fence);
	assert(result == VK_SUCCESS);

	const VkCommandBufferBeginInfo begin_info = {
	    .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	    .pNext            = NULL,
	    .flags            = 0,
	    .pInheritanceInfo = NULL,
	};

	vkBeginCommandBuffer(list->vulkan.command_buffer, &begin_info);
}

void kore_vulkan_device_wait_until_idle(kore_gpu_device *device) {
	vkDeviceWaitIdle(device->vulkan.device);
}

void kore_vulkan_device_create_descriptor_set(kore_gpu_device *device, VkDescriptorSetLayout *descriptor_set_layout, kore_vulkan_descriptor_set *set) {
	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
	    .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
	    .pNext              = NULL,
	    .descriptorPool     = device->vulkan.descriptor_pool,
	    .descriptorSetCount = 1,
	    .pSetLayouts        = descriptor_set_layout,
	};

	VkResult result = vkAllocateDescriptorSets(device->vulkan.device, &descriptor_set_allocate_info, &set->descriptor_set);
	assert(result == VK_SUCCESS);
}

static VkSamplerAddressMode convert_address_mode(kore_gpu_address_mode mode) {
	switch (mode) {
	case KORE_GPU_ADDRESS_MODE_CLAMP_TO_EDGE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case KORE_GPU_ADDRESS_MODE_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case KORE_GPU_ADDRESS_MODE_MIRROR_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	}

	return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

static VkFilter convert_filter(kore_gpu_filter_mode filter) {
	switch (filter) {
	case KORE_GPU_FILTER_MODE_NEAREST:
		return VK_FILTER_NEAREST;
	case KORE_GPU_FILTER_MODE_LINEAR:
		return VK_FILTER_LINEAR;
	}

	return VK_FILTER_NEAREST;
}

static VkSamplerMipmapMode convert_mipmap_filter(kore_gpu_mipmap_filter_mode filter) {
	switch (filter) {
	case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}

	return VK_SAMPLER_MIPMAP_MODE_NEAREST;
}

static VkCompareOp convert_compare_function(kore_gpu_compare_function func) {
	switch (func) {
	case KORE_GPU_COMPARE_FUNCTION_UNDEFINED:
		assert(false);
		return VK_COMPARE_OP_NEVER;
	case KORE_GPU_COMPARE_FUNCTION_NEVER:
		return VK_COMPARE_OP_NEVER;
	case KORE_GPU_COMPARE_FUNCTION_LESS:
		return VK_COMPARE_OP_LESS;
	case KORE_GPU_COMPARE_FUNCTION_EQUAL:
		return VK_COMPARE_OP_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_LESS_EQUAL:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER:
		return VK_COMPARE_OP_GREATER;
	case KORE_GPU_COMPARE_FUNCTION_NOT_EQUAL:
		return VK_COMPARE_OP_NOT_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER_EQUAL:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_ALWAYS:
		return VK_COMPARE_OP_ALWAYS;
	}

	assert(false);
	return VK_COMPARE_OP_NEVER;
}

void kore_vulkan_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {
	sampler->vulkan.device = device->vulkan.device;

	VkSamplerCreateInfo sampler_create_info = {
	    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,

	    .addressModeU = convert_address_mode(parameters->address_mode_u),
	    .addressModeV = convert_address_mode(parameters->address_mode_v),
	    .addressModeW = convert_address_mode(parameters->address_mode_w),

	    .mipmapMode = convert_mipmap_filter(parameters->mipmap_filter),

	    .magFilter = convert_filter(parameters->mag_filter),
	    .minFilter = convert_filter(parameters->min_filter),

	    .compareEnable = parameters->compare != KORE_GPU_COMPARE_FUNCTION_UNDEFINED,
	    .compareOp     = parameters->compare != KORE_GPU_COMPARE_FUNCTION_UNDEFINED ? convert_compare_function(parameters->compare) : 0,

	    .anisotropyEnable = parameters->max_anisotropy > 1,
	    .maxAnisotropy    = parameters->max_anisotropy,

	    .maxLod = parameters->lod_max_clamp,
	    .minLod = parameters->lod_min_clamp,
	};

	vkCreateSampler(device->vulkan.device, &sampler_create_info, NULL, &sampler->vulkan.sampler);
}

void kore_vulkan_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                 uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_vulkan_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                    uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_vulkan_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_vulkan_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return row_bytes;
}

void kore_vulkan_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_vulkan_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_vulkan_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
