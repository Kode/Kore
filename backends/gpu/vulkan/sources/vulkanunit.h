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

static PFN_vkCmdBeginDebugUtilsLabelEXT    vulkanCmdBeginDebugUtilsLabel    = NULL;
static PFN_vkCmdEndDebugUtilsLabelEXT      vulkanCmdEndDebugUtilsLabel      = NULL;
static PFN_vkCmdInsertDebugUtilsLabelEXT   vulkanCmdInsertDebugUtilsLabel   = NULL;
static PFN_vkSetDebugUtilsObjectNameEXT    vulkanSetDebugUtilsObjectName    = NULL;
static PFN_vkCreateDebugUtilsMessengerEXT  vulkanCreateDebugUtilsMessenger  = NULL;
static PFN_vkDestroyDebugUtilsMessengerEXT vulkanDestroyDebugUtilsMessenger = NULL;
static PFN_vkCmdBeginRendering             vulkanCmdBeginRendering          = NULL;
static PFN_vkCmdEndRendering               vulkanCmdEndRendering            = NULL;

static VkFormat                convert_to_vulkan_format(kore_gpu_texture_format format);
static kore_gpu_texture_format convert_from_vulkan_format(VkFormat format);

typedef struct render_pass_attachment {
	VkFormat            format;
	VkAttachmentLoadOp  load_op;
	VkAttachmentStoreOp store_op;
	VkAttachmentLoadOp  stencil_load_op;
	VkAttachmentStoreOp stencil_store_op;
} render_pass_attachment;

typedef struct render_pass_parameters {
	render_pass_attachment attachments[8];
	uint32_t               attachments_count;
	render_pass_attachment depth_attachment;
} render_pass_parameters;

static void find_pipeline_render_pass(VkDevice device, VkFormat formats[8], uint32_t formats_count, VkFormat depth_format, VkRenderPass *render_pass);

static void find_render_pass(VkDevice device, const render_pass_parameters *parameters, VkRenderPass *render_pass);

static void find_framebuffer(VkDevice device, uint32_t width, uint32_t height, VkImageView image_views[9], uint32_t image_views_count, VkRenderPass render_pass,
                             VkFramebuffer *framebuffer);

struct kore_gpu_device;

static uint64_t find_completed_execution(struct kore_gpu_device *device);

static void wait_for_execution(struct kore_gpu_device *device, uint64_t index);

#endif
