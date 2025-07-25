#include "vulkanunit.h"

#define FRAMEBUFFER_COUNT 256

// TODO
typedef struct stored_framebuffer {
	int nothing;
} stored_framebuffer;

static void find_framebuffer(VkDevice device, uint32_t width, uint32_t height, VkImageView image_views[9], uint32_t image_views_count, VkRenderPass render_pass,
                             VkFramebuffer *framebuffer) {
	VkFramebufferCreateInfo framebuffer_create_info = {
	    .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
	    .renderPass      = render_pass,
	    .attachmentCount = image_views_count,
	    .pAttachments    = image_views,
	    .width           = width,
	    .height          = height,
	    .layers          = 1,
	};

	vkCreateFramebuffer(device, &framebuffer_create_info, NULL, framebuffer);
}

#define RENDER_PASSES_COUNT 256

typedef struct stored_render_pass {
	render_pass_parameters parameters;
	VkRenderPass           pass;
} stored_render_pass;

static stored_render_pass render_passes[RENDER_PASSES_COUNT] = {0};

static uint32_t render_passes_count = 0;

static void find_pipeline_render_pass(VkDevice device, VkFormat formats[8], uint32_t formats_count, VkFormat depth_format, VkRenderPass *render_pass) {
	render_pass_parameters parameters = {
	    .attachments_count = formats_count,
	    .depth_attachment =
	        {
	            .format           = depth_format,
	            .load_op          = VK_ATTACHMENT_LOAD_OP_CLEAR,
	            .store_op         = VK_ATTACHMENT_STORE_OP_STORE,
	            .stencil_load_op  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	            .stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	        },
	};

	for (uint32_t attachment_index = 0; attachment_index < formats_count; ++attachment_index) {
		parameters.attachments[attachment_index] = (render_pass_attachment){
		    .format           = formats[attachment_index],
		    .load_op          = VK_ATTACHMENT_LOAD_OP_CLEAR,
		    .store_op         = VK_ATTACHMENT_STORE_OP_STORE,
		    .stencil_load_op  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		    .stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		};
	}

	find_render_pass(device, &parameters, render_pass);
}

static void find_render_pass(VkDevice device, const render_pass_parameters *parameters, VkRenderPass *render_pass) {
	for (uint32_t render_pass_index = 0; render_pass_index < render_passes_count; ++render_pass_index) {
		if (memcmp(&render_passes[render_pass_index].parameters, parameters, sizeof(render_pass_parameters)) == 0) {
			*render_pass = render_passes[render_pass_index].pass;
			return;
		}
	}

	assert(render_passes_count < RENDER_PASSES_COUNT);

	VkAttachmentDescription attachment_descriptions[9];
	VkAttachmentReference   attachment_references[8];

	for (uint32_t attachment_index = 0; attachment_index < parameters->attachments_count; ++attachment_index) {
		const render_pass_attachment *attachment = &parameters->attachments[attachment_index];

		attachment_descriptions[attachment_index] = (VkAttachmentDescription){
		    .format         = attachment->format,
		    .samples        = VK_SAMPLE_COUNT_1_BIT,
		    .loadOp         = attachment->load_op,
		    .storeOp        = attachment->store_op,
		    .stencilLoadOp  = attachment->stencil_load_op,
		    .stencilStoreOp = attachment->stencil_store_op,
		    .initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		    .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		};

		attachment_references[attachment_index] = (VkAttachmentReference){
		    .attachment = attachment_index,
		    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		};
	}

	// depth at the last index
	attachment_descriptions[parameters->attachments_count] = (VkAttachmentDescription){
	    .format         = parameters->depth_attachment.format,
	    .samples        = VK_SAMPLE_COUNT_1_BIT,
	    .loadOp         = parameters->depth_attachment.load_op,
	    .storeOp        = parameters->depth_attachment.store_op,
	    .stencilLoadOp  = parameters->depth_attachment.stencil_load_op,
	    .stencilStoreOp = parameters->depth_attachment.stencil_store_op,
	    .initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	VkAttachmentReference depth_reference = {
	    .attachment = parameters->attachments_count,
	    .layout     = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
	};

	bool has_depth = parameters->depth_attachment.format != VK_FORMAT_UNDEFINED;

	VkSubpassDescription subpass_description = {
	    .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
	    .colorAttachmentCount    = parameters->attachments_count,
	    .pColorAttachments       = attachment_references,
	    .pDepthStencilAttachment = has_depth ? &depth_reference : NULL,
	};

	VkSubpassDependency dependency = {
	    .srcSubpass    = VK_SUBPASS_EXTERNAL,
	    .dstSubpass    = 0,
	    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .srcAccessMask = 0,
	    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	VkRenderPassCreateInfo render_pass_create_info = {
	    .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
	    .attachmentCount = has_depth ? parameters->attachments_count + 1 : parameters->attachments_count,
	    .pAttachments    = attachment_descriptions,
	    .subpassCount    = 1,
	    .pSubpasses      = &subpass_description,
	    .dependencyCount = 1,
	    .pDependencies   = &dependency,
	};

	vkCreateRenderPass(device, &render_pass_create_info, NULL, render_pass);

	render_passes[render_passes_count].parameters = *parameters;
	render_passes[render_passes_count].pass       = *render_pass;
	++render_passes_count;
}
