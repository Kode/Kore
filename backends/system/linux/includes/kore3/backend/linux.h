#ifndef KORE_LINUX_HEADER
#define KORE_LINUX_HEADER

bool kore_linux_wayland(void);

#ifdef KORE_VULKAN

#include <vulkan/vulkan_core.h>

VkResult kinc_wayland_vulkan_create_surface(VkInstance instance, int window_index, VkSurfaceKHR *surface);
VkBool32 kinc_wayland_vulkan_get_physical_device_presentation_support(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
VkResult kinc_x11_vulkan_create_surface(VkInstance instance, int window_index, VkSurfaceKHR *surface);
VkBool32 kinc_x11_vulkan_get_physical_device_presentation_support(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);

#endif

#endif
