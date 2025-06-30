#include "vulkanunit.h"

static void create_default_render_pass(kore_vulkan_device *device, VkFormat formats[8], uint32_t formats_count, VkFormat depth_format,
                                       VkRenderPass *render_pass) {
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

	create_render_pass(device, &parameters, render_pass);
}

static void create_render_pass(kore_vulkan_device *device, const render_pass_parameters *parameters, VkRenderPass *render_pass) {
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

	vkCreateRenderPass(device->device, &render_pass_create_info, NULL, render_pass);
}
