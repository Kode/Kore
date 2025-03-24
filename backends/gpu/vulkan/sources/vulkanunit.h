#ifndef KORE_VULKAN_UNIT_HEADER
#define KORE_VULKAN_UNIT_HEADER

#include <kore3/gpu/textureformat.h>

#ifdef KORE_WINDOWS

// Windows 7
#define WINVER       0x0601
#define _WIN32_WINNT 0x0601

#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCOMM
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NODRAWTEXT
#define NOGDI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOKANJI
#define NOKEYSTATES
// #define NOMB
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NONLS
#define NOOPENFILE
#define NOPROFILER
#define NORASTEROPS
#define NOSCROLL
#define NOSERVICE
#define NOSHOWWINDOW
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOTEXTMETRIC
// #define NOUSER
#define NOVIRTUALKEYCODES
#define NOWH
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define WIN32_LEAN_AND_MEAN

// avoids a warning in the Windows headers
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0

#endif

#ifndef NDEBUG
#define VALIDATE
#endif

#include <vulkan/vulkan.h>

static PFN_vkGetPhysicalDeviceSurfaceSupportKHR      vkGetPhysicalDeviceSurfaceSupport      = NULL;
static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilities = NULL;
static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR      vkGetPhysicalDeviceSurfaceFormats      = NULL;
static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModes = NULL;
static PFN_vkCreateSwapchainKHR                      vkCreateSwapchain                      = NULL;
static PFN_vkDestroySwapchainKHR                     vkDestroySwapchain                     = NULL;
static PFN_vkGetSwapchainImagesKHR                   vkGetSwapchainImages                   = NULL;
static PFN_vkDestroySurfaceKHR                       vkDestroySurface                       = NULL;

static PFN_vkCmdBeginDebugUtilsLabelEXT    vkCmdBeginDebugUtilsLabel    = NULL;
static PFN_vkCmdEndDebugUtilsLabelEXT      vkCmdEndDebugUtilsLabel      = NULL;
static PFN_vkCmdInsertDebugUtilsLabelEXT   vkCmdInsertDebugUtilsLabel   = NULL;
static PFN_vkSetDebugUtilsObjectNameEXT    vkSetDebugUtilsObjectName    = NULL;
static PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessenger  = NULL;
static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = NULL;

static PFN_vkAcquireNextImageKHR vkAcquireNextImage = NULL;
static PFN_vkQueuePresentKHR     vkQueuePresent     = NULL;

static VkFormat convert_format(kore_gpu_texture_format format);

#endif
