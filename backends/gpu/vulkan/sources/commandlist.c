#include <kore3/vulkan/commandlist_functions.h>

#include "vulkanunit.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>

#include <kore3/vulkan/texture_functions.h>

#include <kore3/vulkan/pipeline_structs.h>

#include <kore3/util/align.h>

#include <assert.h>

void kore_vulkan_command_list_destroy(kore_gpu_command_list *list) {
	vkFreeCommandBuffers(list->vulkan.device, list->vulkan.command_pool, 1, &list->vulkan.command_buffer);
}

VkAttachmentLoadOp convert_load_op(kore_gpu_load_op op) {
	switch (op) {
	case KORE_GPU_LOAD_OP_LOAD:
		return VK_ATTACHMENT_LOAD_OP_LOAD;
	case KORE_GPU_LOAD_OP_CLEAR:
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	}

	assert(false);
	return VK_ATTACHMENT_LOAD_OP_CLEAR;
}

VkAttachmentStoreOp convert_store_op(kore_gpu_store_op op) {
	switch (op) {
	case KORE_GPU_STORE_OP_STORE:
		return VK_ATTACHMENT_STORE_OP_STORE;
	case KORE_GPU_STORE_OP_DISCARD:
		return VK_ATTACHMENT_STORE_OP_NONE;
	}

	assert(false);
	return VK_ATTACHMENT_STORE_OP_STORE;
}

VkClearValue convert_color_clear_value(kore_gpu_texture_format format, kore_gpu_color color) {
	switch (kore_gpu_texture_format_get_type(format)) {
	case KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_TYPE_SNORM:
		return (VkClearValue){
		    .color.float32 =
		        {
		            color.r,
		            color.g,
		            color.b,
		            color.a,
		        },
		};
	case KORE_GPU_TEXTURE_FORMAT_TYPE_INT:
		return (VkClearValue){
		    .color.int32 =
		        {
		            (int32_t)(color.r * 255 - 128),
		            (int32_t)(color.g * 255 - 128),
		            (int32_t)(color.b * 255 - 128),
		            (int32_t)(color.a * 255 - 128),
		        },
		};
	case KORE_GPU_TEXTURE_FORMAT_TYPE_UINT:
		return (VkClearValue){
		    .color.uint32 =
		        {
		            (uint32_t)(color.r * 255),
		            (uint32_t)(color.g * 255),
		            (uint32_t)(color.b * 255),
		            (uint32_t)(color.a * 255),
		        },
		};
	}

	assert(false);
	return (VkClearValue){0};
}

VkClearValue convert_depth_clear_value(float value) {
	VkClearValue clear_value = {
	    .depthStencil =
	        {
	            .depth   = value,
	            .stencil = 0,
	        },
	};
	return clear_value;
}

static void pause_render_pass(kore_gpu_command_list *list) {
	if (list->vulkan.render_pass_status == KORE_VULKAN_RENDER_PASS_STATUS_ACTIVE) {
#ifndef KORE_ANDROID // TODO
		vkCmdEndRendering(list->vulkan.command_buffer);
#endif
		list->vulkan.render_pass_status = KORE_VULKAN_RENDER_PASS_STATUS_PAUSED;
	}
}

static void resume_render_pass(kore_gpu_command_list *list) {
	if (list->vulkan.render_pass_status == KORE_VULKAN_RENDER_PASS_STATUS_NONE || list->vulkan.render_pass_status == KORE_VULKAN_RENDER_PASS_STATUS_ACTIVE) {
		return;
	}

	bool paused = list->vulkan.render_pass_status == KORE_VULKAN_RENDER_PASS_STATUS_PAUSED;

	const kore_gpu_render_pass_parameters *parameters = &list->vulkan.current_render_pass;

	kore_gpu_texture *texture = parameters->color_attachments[0].texture.texture;

	if (texture->vulkan.image_layout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		VkImageMemoryBarrier barrier = {
		    .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		    .pNext         = NULL,
		    .srcAccessMask = 0,
		    .dstAccessMask = 0,
		    .oldLayout     = texture->vulkan.image_layout,
		    .newLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		    .image         = texture->vulkan.image,
		    .subresourceRange =
		        {
		            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
		            .baseMipLevel   = 0,
		            .levelCount     = 1,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		    .srcAccessMask = 0,
		    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		};

		vkCmdPipelineBarrier(list->vulkan.command_buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1,
		                     &barrier);

		texture->vulkan.image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	VkImageViewCreateInfo view_create_info = {
	    .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	    .pNext    = NULL,
	    .image    = texture->vulkan.image,
	    .viewType = VK_IMAGE_VIEW_TYPE_2D,
	    .format   = convert_to_vulkan_format(texture->vulkan.format),
	    .components =
	        {
	            .r = VK_COMPONENT_SWIZZLE_R,
	            .g = VK_COMPONENT_SWIZZLE_G,
	            .b = VK_COMPONENT_SWIZZLE_B,
	            .a = VK_COMPONENT_SWIZZLE_A,
	        },
	    .subresourceRange =
	        {
	            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
	            .baseMipLevel   = 0,
	            .levelCount     = 1,
	            .baseArrayLayer = 0,
	            .layerCount     = 1,
	        },
	    .flags = 0,
	};

	VkImageView image_view;
	VkResult    result = vkCreateImageView(list->vulkan.device, &view_create_info, NULL, &image_view);
	assert(result == VK_SUCCESS);

	const VkRenderingAttachmentInfo color_attachment_info = {
	    .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
	    .pNext              = NULL,
	    .imageView          = image_view,
	    .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .resolveMode        = VK_RESOLVE_MODE_NONE,
	    .resolveImageView   = VK_NULL_HANDLE,
	    .resolveImageLayout = VK_IMAGE_LAYOUT_GENERAL,
	    .loadOp             = paused ? KORE_GPU_LOAD_OP_LOAD : convert_load_op(parameters->color_attachments[0].load_op),
	    .storeOp            = convert_store_op(parameters->color_attachments[0].store_op),
	    .clearValue         = convert_color_clear_value(texture->vulkan.format, parameters->color_attachments[0].clear_value),
	};

	VkRenderingAttachmentInfo depth_attachment_info;

	if (parameters->depth_stencil_attachment.texture != NULL) {
		VkImageViewCreateInfo depth_view_create_info = {
		    .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		    .pNext    = NULL,
		    .image    = parameters->depth_stencil_attachment.texture->vulkan.image,
		    .viewType = VK_IMAGE_VIEW_TYPE_2D,
		    .format   = convert_to_vulkan_format(parameters->depth_stencil_attachment.texture->vulkan.format),
		    .components =
		        {
		            .r = VK_COMPONENT_SWIZZLE_R,
		            .g = VK_COMPONENT_SWIZZLE_G,
		            .b = VK_COMPONENT_SWIZZLE_B,
		            .a = VK_COMPONENT_SWIZZLE_A,
		        },
		    .subresourceRange =
		        {
		            .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
		            .baseMipLevel   = 0,
		            .levelCount     = 1,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		    .flags = 0,
		};

		VkImageView depth_image_view;
		VkResult    result = vkCreateImageView(list->vulkan.device, &depth_view_create_info, NULL, &depth_image_view);
		assert(result == VK_SUCCESS);

		depth_attachment_info = (VkRenderingAttachmentInfo){
		    .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		    .pNext              = NULL,
		    .imageView          = depth_image_view,
		    .imageLayout        = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		    .resolveMode        = VK_RESOLVE_MODE_NONE,
		    .resolveImageView   = VK_NULL_HANDLE,
		    .resolveImageLayout = VK_IMAGE_LAYOUT_GENERAL,
		    .loadOp             = paused ? KORE_GPU_LOAD_OP_LOAD : convert_load_op(parameters->depth_stencil_attachment.depth_load_op),
		    .storeOp            = convert_store_op(parameters->depth_stencil_attachment.depth_store_op),
		    .clearValue         = convert_depth_clear_value(parameters->depth_stencil_attachment.depth_clear_value),
		};
	}

	const VkRect2D render_area = {
	    .offset =
	        {
	            .x = 0,
	            .y = 0,
	        },
	    .extent =
	        {
	            .width  = texture->vulkan.width,
	            .height = texture->vulkan.height,
	        },
	};

	const VkRenderingInfo rendering_info = {
	    .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
	    .pNext                = NULL,
	    .flags                = 0,
	    .renderArea           = render_area,
	    .layerCount           = 1,
	    .viewMask             = 0,
	    .colorAttachmentCount = 1,
	    .pColorAttachments    = &color_attachment_info,
	    .pDepthAttachment     = parameters->depth_stencil_attachment.texture == NULL ? VK_NULL_HANDLE : &depth_attachment_info,
	    .pStencilAttachment   = VK_NULL_HANDLE,
	};

#ifndef KORE_ANDROID // TODO
	vkCmdBeginRendering(list->vulkan.command_buffer, &rendering_info);
#endif

	kore_vulkan_command_list_set_viewport(list, 0, 0, (float)texture->vulkan.width, (float)texture->vulkan.height, 0.1f, 1.0f);

	kore_vulkan_command_list_set_scissor_rect(list, 0, 0, texture->vulkan.width, texture->vulkan.height);

	list->vulkan.render_pass_status = KORE_VULKAN_RENDER_PASS_STATUS_ACTIVE;
}

void kore_vulkan_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	list->vulkan.current_render_pass = *parameters;
	list->vulkan.render_pass_status  = KORE_VULKAN_RENDER_PASS_STATUS_SET;
}

void kore_vulkan_command_list_end_render_pass(kore_gpu_command_list *list) {
	if (list->vulkan.render_pass_status = KORE_VULKAN_RENDER_PASS_STATUS_ACTIVE) {
#ifndef KORE_ANDROID // TODO
		vkCmdEndRendering(list->vulkan.command_buffer);
#endif
	}
	list->vulkan.render_pass_status = KORE_VULKAN_RENDER_PASS_STATUS_NONE;
}

void kore_vulkan_command_list_present(kore_gpu_command_list *list) {
	list->vulkan.presenting = true;
}

void kore_vulkan_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset,
                                               uint64_t size) {
	vkCmdBindIndexBuffer(list->vulkan.command_buffer, buffer->vulkan.buffer, offset,
	                     index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
}

void kore_vulkan_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_vulkan_buffer *buffer, uint64_t offset, uint64_t size,
                                                uint64_t stride) {
	vkCmdBindVertexBuffers(list->vulkan.command_buffer, slot, 1, &buffer->buffer, &offset);
}

static kore_vulkan_render_pipeline *current_render_pipeline = NULL;

void kore_vulkan_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_vulkan_render_pipeline *pipeline) {
	vkCmdBindPipeline(list->vulkan.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
	current_render_pipeline = pipeline;
}

void kore_vulkan_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                                   uint32_t first_instance) {}

void kore_vulkan_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                                           int32_t base_vertex, uint32_t first_instance) {
	resume_render_pass(list);
	vkCmdDrawIndexed(list->vulkan.command_buffer, index_count, instance_count, first_index, base_vertex, first_instance);
}

void kore_vulkan_command_list_set_descriptor_set(kore_gpu_command_list *list, uint32_t set_index, kore_vulkan_descriptor_set *set,
                                                 kore_gpu_buffer **dynamic_buffers, uint32_t *dynamic_offsets, uint32_t *dynamic_sizes) {
	pause_render_pass(list);
	vkCmdBindDescriptorSets(list->vulkan.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, current_render_pipeline->pipeline_layout, set_index, 1,
	                        &set->descriptor_set, 0, NULL);
}

void kore_vulkan_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size) {}

void kore_vulkan_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                    uint64_t destination_offset, uint64_t size) {}

void kore_vulkan_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {
	if (destination->texture->vulkan.image_layout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		VkImageMemoryBarrier barrier = {
		    .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		    .pNext         = NULL,
		    .srcAccessMask = 0,
		    .dstAccessMask = 0,
		    .oldLayout     = destination->texture->vulkan.image_layout,
		    .newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		    .image         = destination->texture->vulkan.image,
		    .subresourceRange =
		        {
		            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
		            .baseMipLevel   = 0,
		            .levelCount     = 1,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		    .srcAccessMask = 0,
		    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		};

		vkCmdPipelineBarrier(list->vulkan.command_buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1,
		                     &barrier);

		destination->texture->vulkan.image_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	}

	VkImageAspectFlags aspect_mask;
	switch (destination->aspect) {
	case KORE_GPU_IMAGE_COPY_ASPECT_ALL:
		aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
		break;
	case KORE_GPU_IMAGE_COPY_ASPECT_DEPTH_ONLY:
		aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
		break;
	case KORE_GPU_IMAGE_COPY_ASPECT_STENCIL_ONLY:
		aspect_mask = VK_IMAGE_ASPECT_STENCIL_BIT;
		break;
	}

	VkBufferImageCopy region = {
	    .bufferOffset      = source->offset,
	    .bufferRowLength   = source->bytes_per_row / kore_gpu_texture_format_byte_size(destination->texture->vulkan.format),
	    .bufferImageHeight = source->rows_per_image,
	    .imageSubresource =
	        {
	            .aspectMask     = aspect_mask,
	            .mipLevel       = destination->mip_level,
	            .baseArrayLayer = destination->origin_z,
	            .layerCount     = depth_or_array_layers,
	        },
	    .imageOffset =
	        {
	            .x = destination->origin_x,
	            .y = destination->origin_y,
	            .z = 0,
	        },
	    .imageExtent =
	        {
	            .width  = width,
	            .height = height,
	            .depth  = 1,
	        },
	};
	vkCmdCopyBufferToImage(list->vulkan.command_buffer, source->buffer->vulkan.buffer, destination->texture->vulkan.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	                       1, &region);
}

void kore_vulkan_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {
	if (source->texture->vulkan.image_layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		VkImageMemoryBarrier barrier = {
		    .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		    .pNext         = NULL,
		    .srcAccessMask = 0,
		    .dstAccessMask = 0,
		    .oldLayout     = source->texture->vulkan.image_layout,
		    .newLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		    .image         = source->texture->vulkan.image,
		    .subresourceRange =
		        {
		            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
		            .baseMipLevel   = 0,
		            .levelCount     = 1,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		    .srcAccessMask = 0,
		    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		};

		vkCmdPipelineBarrier(list->vulkan.command_buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1,
		                     &barrier);

		source->texture->vulkan.image_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	}

	VkImageAspectFlags aspect_mask;
	switch (source->aspect) {
	case KORE_GPU_IMAGE_COPY_ASPECT_ALL:
		aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
		break;
	case KORE_GPU_IMAGE_COPY_ASPECT_DEPTH_ONLY:
		aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
		break;
	case KORE_GPU_IMAGE_COPY_ASPECT_STENCIL_ONLY:
		aspect_mask = VK_IMAGE_ASPECT_STENCIL_BIT;
		break;
	}

	VkBufferImageCopy region = {
	    .bufferOffset      = destination->offset,
	    .bufferRowLength   = destination->bytes_per_row / kore_gpu_texture_format_byte_size(source->texture->vulkan.format),
	    .bufferImageHeight = destination->rows_per_image,
	    .imageSubresource =
	        {
	            .aspectMask     = aspect_mask,
	            .mipLevel       = source->mip_level,
	            .baseArrayLayer = source->origin_z,
	            .layerCount     = depth_or_array_layers,
	        },
	    .imageOffset =
	        {
	            .x = source->origin_x,
	            .y = source->origin_y,
	            .z = 0,
	        },
	    .imageExtent =
	        {
	            .width  = width,
	            .height = height,
	            .depth  = 1,
	        },
	};
	vkCmdCopyImageToBuffer(list->vulkan.command_buffer, source->texture->vulkan.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destination->buffer->vulkan.buffer,
	                       1, &region);
}

void kore_vulkan_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                      const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                      uint32_t depth_or_array_layers) {}

void kore_vulkan_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {}

void kore_vulkan_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_vulkan_compute_pipeline *pipeline) {}

void kore_vulkan_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {}

void kore_vulkan_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, kore_gpu_raytracing_volume *volume) {}

void kore_vulkan_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_vulkan_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                          kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_vulkan_command_list_set_ray_pipeline(kore_gpu_command_list *list, kore_vulkan_ray_pipeline *pipeline) {}

void kore_vulkan_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth) {}

void kore_vulkan_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth) {
	// The coordinate system is upside down compared to other APIs, -height for the viewport (supported via VK_KHR_MAINTENANCE1) is an easy fix.
	VkViewport viewport = {.x = x, .y = y + height, .width = width, .height = -height, .minDepth = min_depth, .maxDepth = max_depth};
	vkCmdSetViewport(list->vulkan.command_buffer, 0, 1, &viewport);
}

void kore_vulkan_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	VkRect2D scissor = {
	    .offset =
	        {
	            .x = 0,
	            .y = 0,
	        },
	    .extent =
	        {
	            .width  = width,
	            .height = height,
	        },
	};
	vkCmdSetScissor(list->vulkan.command_buffer, 0, 1, &scissor);
}

void kore_vulkan_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color) {}

void kore_vulkan_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference) {}

void kore_vulkan_command_list_set_name(kore_gpu_command_list *list, const char *name) {
	VkDebugUtilsObjectNameInfoEXT name_info = {
	    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
	    .objectType   = VK_OBJECT_TYPE_COMMAND_BUFFER,
	    .objectHandle = (uint64_t)list->vulkan.command_buffer,
	    .pObjectName  = name,
	};
	vkSetDebugUtilsObjectName(list->vulkan.device, &name_info);
}

void kore_vulkan_command_list_push_debug_group(kore_gpu_command_list *list, const char *name) {
	const VkDebugUtilsLabelEXT label_info = {
	    .sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
	    .pNext      = NULL,
	    .pLabelName = name,
	    .color      = {0.0f, 0.0f, 1.0f, 1.0f},
	};
	vkCmdBeginDebugUtilsLabel(list->vulkan.command_buffer, &label_info);
}

void kore_vulkan_command_list_pop_debug_group(kore_gpu_command_list *list) {
	vkCmdEndDebugUtilsLabel(list->vulkan.command_buffer);
}

void kore_vulkan_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name) {
	const VkDebugUtilsLabelEXT label_info = {
	    .sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
	    .pNext      = NULL,
	    .pLabelName = name,
	    .color      = {0.0f, 0.0f, 1.0f, 1.0f},
	};
	vkCmdInsertDebugUtilsLabel(list->vulkan.command_buffer, &label_info);
}

void kore_vulkan_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index) {}

void kore_vulkan_command_list_end_occlusion_query(kore_gpu_command_list *list) {}

void kore_vulkan_command_list_resolve_query_set(kore_gpu_command_list *list, kore_gpu_query_set *query_set, uint32_t first_query, uint32_t query_count,
                                                kore_gpu_buffer *destination, uint64_t destination_offset) {}

void kore_vulkan_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset, uint32_t max_draw_count,
                                            kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_vulkan_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                    uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_vulkan_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset) {}

void kore_vulkan_command_list_queue_buffer_access(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {}

void kore_vulkan_command_list_queue_descriptor_set_access(kore_gpu_command_list *list, kore_vulkan_descriptor_set *descriptor_set) {}
