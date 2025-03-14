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

void kore_vulkan_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	const kore_gpu_texture *texture = parameters->color_attachments[0].texture.texture;

	const VkClearValue clear_value = {
	    .color =
	        {
	            .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
	        },
	    .depthStencil =
	        {
	            .depth   = 1.0f,
	            .stencil = 0,
	        },
	};

	const VkRenderingAttachmentInfo color_attachment_info = {
	    .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
	    .pNext              = NULL,
	    .imageView          = texture->vulkan.image_view,
	    .imageLayout        = VK_IMAGE_LAYOUT_GENERAL,
	    .resolveMode        = VK_RESOLVE_MODE_NONE,
	    .resolveImageView   = VK_NULL_HANDLE,
	    .resolveImageLayout = VK_IMAGE_LAYOUT_GENERAL,
	    .loadOp             = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	    .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
	    .clearValue         = clear_value,
	};

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
	    .pDepthAttachment     = VK_NULL_HANDLE,
	    .pStencilAttachment   = VK_NULL_HANDLE,
	};

#ifndef KORE_ANDROID // TODO
	vkCmdBeginRendering(list->vulkan.command_buffer, &rendering_info);
#endif

	VkViewport viewport = {.x = 0, .y = 0, .width = (float)texture->vulkan.width, .height = (float)texture->vulkan.height, .minDepth = 0.1f, .maxDepth = 1.0f};
	vkCmdSetViewport(list->vulkan.command_buffer, 0, 1, &viewport);

	VkRect2D scissor = {
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
	vkCmdSetScissor(list->vulkan.command_buffer, 0, 1, &scissor);
}

void kore_vulkan_command_list_end_render_pass(kore_gpu_command_list *list) {
#ifndef KORE_ANDROID // TODO
	vkCmdEndRendering(list->vulkan.command_buffer);
#endif
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

void kore_vulkan_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_vulkan_render_pipeline *pipeline) {
	vkCmdBindPipeline(list->vulkan.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
}

void kore_vulkan_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                                   uint32_t first_instance) {}

void kore_vulkan_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                                           int32_t base_vertex, uint32_t first_instance) {
	vkCmdDrawIndexed(list->vulkan.command_buffer, index_count, instance_count, first_index, base_vertex, first_instance);
}

void kore_vulkan_command_list_set_descriptor_table(kore_gpu_command_list *list, uint32_t table_index, kore_vulkan_descriptor_set *set,
                                                   kore_gpu_buffer **dynamic_buffers, uint32_t *dynamic_offsets, uint32_t *dynamic_sizes) {}

void kore_vulkan_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size) {}

void kore_vulkan_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                    uint64_t destination_offset, uint64_t size) {}

void kore_vulkan_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {}

void kore_vulkan_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {}

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

void kore_vulkan_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth) {}

void kore_vulkan_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

void kore_vulkan_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color) {}

void kore_vulkan_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference) {}

void kore_vulkan_command_list_set_name(kore_gpu_command_list *list, const char *name) {
	const VkDebugMarkerObjectNameInfoEXT name_info = {
	    .sType       = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
	    .pNext       = NULL,
	    .objectType  = VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT,
	    .object      = (uint64_t)list->vulkan.command_buffer,
	    .pObjectName = name,
	};

	vulkan_DebugMarkerSetObjectNameEXT(list->vulkan.device, &name_info);
}

void kore_vulkan_command_list_push_debug_group(kore_gpu_command_list *list, const char *name) {
	const VkDebugMarkerMarkerInfoEXT marker_info = {
	    .sType       = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
	    .pNext       = NULL,
	    .pMarkerName = name,
	    .color       = {0.0f, 0.0f, 0.0f, 1.0f},
	};

	vulkan_CmdDebugMarkerBeginEXT(list->vulkan.command_buffer, &marker_info);
}

void kore_vulkan_command_list_pop_debug_group(kore_gpu_command_list *list) {
	vulkan_CmdDebugMarkerEndEXT(list->vulkan.command_buffer);
}

void kore_vulkan_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name) {
	const VkDebugMarkerMarkerInfoEXT marker_info = {
	    .sType       = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
	    .pNext       = NULL,
	    .pMarkerName = name,
	    .color       = {0.0f, 0.0f, 0.0f, 1.0f},
	};

	vulkan_CmdDebugMarkerInsertEXT(list->vulkan.command_buffer, &marker_info);
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
