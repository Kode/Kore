#include <kore3/webgpu/commandlist_functions.h>

#include "webgpuunit.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>

#include <kore3/webgpu/texture_functions.h>

#include <kore3/webgpu/pipeline_structs.h>

#include <kore3/util/align.h>

#include <assert.h>

static WGPULoadOp convert_load_op(kore_gpu_load_op op) {
	switch (op) {
	case KORE_GPU_LOAD_OP_LOAD:
		return WGPULoadOp_Load;
	case KORE_GPU_LOAD_OP_CLEAR:
		return WGPULoadOp_Clear;
	}

	assert(false);
	return WGPULoadOp_Load;
}

static WGPUStoreOp convert_store_op(kore_gpu_store_op op) {
	switch (op) {
	case KORE_GPU_STORE_OP_STORE:
		return WGPUStoreOp_Store;
	case KORE_GPU_STORE_OP_DISCARD:
		return WGPUStoreOp_Discard;
	}

	assert(false);
	return WGPUStoreOp_Store;
}

static WGPUTextureViewDimension convert_dimension(kore_gpu_texture_view_dimension dimension) {
	switch (dimension) {
	case KORE_GPU_TEXTURE_VIEW_DIMENSION_1D:
		return WGPUTextureViewDimension_1D;
	case KORE_GPU_TEXTURE_VIEW_DIMENSION_2D:
		return WGPUTextureViewDimension_2D;
	case KORE_GPU_TEXTURE_VIEW_DIMENSION_2DARRAY:
		return WGPUTextureViewDimension_2DArray;
	case KORE_GPU_TEXTURE_VIEW_DIMENSION_CUBE:
		return WGPUTextureViewDimension_Cube;
	case KORE_GPU_TEXTURE_VIEW_DIMENSION_CUBEARRAY:
		return WGPUTextureViewDimension_CubeArray;
	case KORE_GPU_TEXTURE_VIEW_DIMENSION_3D:
		return WGPUTextureViewDimension_3D;
	}

	assert(false);
	return WGPUTextureViewDimension_2D;
}

void kore_webgpu_command_list_destroy(kore_gpu_command_list *list) {}

void kore_webgpu_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	if (list->webgpu.compute_pass_encoder != NULL) {
		wgpuComputePassEncoderEnd(list->webgpu.compute_pass_encoder);
		list->webgpu.compute_pass_encoder = NULL;
	}

	WGPUTextureView               texture_views[8];
	WGPURenderPassColorAttachment color_attachments[8];

	for (uint32_t attachment_index = 0; attachment_index < parameters->color_attachments_count; ++attachment_index) {
		WGPUTextureViewDescriptor texture_view_descriptor = {
		    .format          = kore_webgpu_convert_texture_format_to_webgpu(parameters->color_attachments[attachment_index].texture.texture->webgpu.format),
		    .dimension       = convert_dimension(parameters->color_attachments[attachment_index].texture.dimension),
		    .baseMipLevel    = parameters->color_attachments[attachment_index].texture.base_mip_level,
		    .mipLevelCount   = parameters->color_attachments[attachment_index].texture.mip_level_count,
		    .baseArrayLayer  = parameters->color_attachments[attachment_index].texture.base_array_layer,
		    .arrayLayerCount = parameters->color_attachments[attachment_index].texture.array_layer_count,
		};
		texture_views[attachment_index] =
		    wgpuTextureCreateView(parameters->color_attachments[attachment_index].texture.texture->webgpu.texture, &texture_view_descriptor);

		color_attachments[attachment_index] = (WGPURenderPassColorAttachment){
		    .view       = texture_views[attachment_index],
		    .loadOp     = convert_load_op(parameters->color_attachments[attachment_index].load_op),
		    .storeOp    = convert_store_op(parameters->color_attachments[attachment_index].store_op),
		    .clearValue = {parameters->color_attachments[attachment_index].clear_value.r, parameters->color_attachments[attachment_index].clear_value.g,
		                   parameters->color_attachments[attachment_index].clear_value.b, parameters->color_attachments[attachment_index].clear_value.a},
		    .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
		};
	}

	if (parameters->depth_stencil_attachment.texture != NULL) {
		WGPUTextureViewDescriptor depth_view_descriptor = {
		    .format          = kore_webgpu_convert_texture_format_to_webgpu(parameters->depth_stencil_attachment.texture->webgpu.format),
		    .dimension       = WGPUTextureViewDimension_2D,
		    .arrayLayerCount = 1,
		    .mipLevelCount   = 1,
		};
		WGPUTextureView depth_view = wgpuTextureCreateView(parameters->depth_stencil_attachment.texture->webgpu.texture, &depth_view_descriptor);

		WGPURenderPassDepthStencilAttachment depth_attachment = (WGPURenderPassDepthStencilAttachment){
		    .view            = depth_view,
		    .depthLoadOp     = convert_load_op(parameters->depth_stencil_attachment.depth_load_op),
		    .depthStoreOp    = convert_store_op(parameters->depth_stencil_attachment.depth_store_op),
		    .depthClearValue = parameters->depth_stencil_attachment.depth_clear_value,
		};

		WGPURenderPassDescriptor render_pass_descriptor = {
		    .colorAttachmentCount   = parameters->color_attachments_count,
		    .colorAttachments       = color_attachments,
		    .depthStencilAttachment = &depth_attachment,
		};

		list->webgpu.render_pass_encoder = wgpuCommandEncoderBeginRenderPass(list->webgpu.command_encoder, &render_pass_descriptor);
	}
	else {
		WGPURenderPassDescriptor render_pass_descriptor = {
		    .colorAttachmentCount = parameters->color_attachments_count,
		    .colorAttachments     = color_attachments,
		};

		list->webgpu.render_pass_encoder = wgpuCommandEncoderBeginRenderPass(list->webgpu.command_encoder, &render_pass_descriptor);
	}
}

void kore_webgpu_command_list_end_render_pass(kore_gpu_command_list *list) {
	wgpuRenderPassEncoderEnd(list->webgpu.render_pass_encoder);
	list->webgpu.render_pass_encoder = NULL;
}

void kore_webgpu_command_list_present(kore_gpu_command_list *list) {}

void kore_webgpu_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset) {
	kore_webgpu_buffer_schedule_uploads(&buffer->webgpu);

	wgpuRenderPassEncoderSetIndexBuffer(list->webgpu.render_pass_encoder, buffer->webgpu.buffer,
	                                    index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? WGPUIndexFormat_Uint16 : WGPUIndexFormat_Uint32, offset,
	                                    buffer->webgpu.size - offset);
}

void kore_webgpu_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_webgpu_buffer *buffer, uint64_t offset, uint64_t size,
                                                uint64_t stride) {
	kore_webgpu_buffer_schedule_uploads(buffer);

	wgpuRenderPassEncoderSetVertexBuffer(list->webgpu.render_pass_encoder, slot, buffer->buffer, offset, size); // why is stride not needed?
}

void kore_webgpu_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_webgpu_render_pipeline *pipeline) {
	wgpuRenderPassEncoderSetPipeline(list->webgpu.render_pass_encoder, pipeline->render_pipeline);
}

void kore_webgpu_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                                   uint32_t first_instance) {}

void kore_webgpu_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                                           int32_t base_vertex, uint32_t first_instance) {
	wgpuRenderPassEncoderDrawIndexed(list->webgpu.render_pass_encoder, index_count, instance_count, first_index, base_vertex, first_instance);
}

void kore_webgpu_command_list_set_bind_group(kore_gpu_command_list *list, uint32_t index, kore_webgpu_descriptor_set *set, uint32_t dynamic_count,
                                             uint32_t *dynamic_offsets) {
	if (list->webgpu.render_pass_encoder != NULL) {
		wgpuRenderPassEncoderSetBindGroup(list->webgpu.render_pass_encoder, index, set->bind_group, dynamic_count, dynamic_offsets);
	}
	else {
		assert(list->webgpu.compute_pass_encoder != NULL);
		wgpuComputePassEncoderSetBindGroup(list->webgpu.compute_pass_encoder, index, set->bind_group, dynamic_count, dynamic_offsets);
	}
}

void kore_webgpu_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size) {}

void kore_webgpu_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                    uint64_t destination_offset, uint64_t size) {
	wgpuCommandEncoderCopyBufferToBuffer(list->webgpu.command_encoder, source->webgpu.has_copy_buffer ? source->webgpu.copy_buffer : source->webgpu.buffer,
	                                     source_offset, destination->webgpu.buffer, destination_offset, size);
}

static WGPUTextureAspect convert_texture_aspect(kore_gpu_texture_aspect aspect) {
	switch (aspect) {
	case KORE_GPU_IMAGE_COPY_ASPECT_ALL:
		return WGPUTextureAspect_All;
	case KORE_GPU_IMAGE_COPY_ASPECT_DEPTH_ONLY:
		return WGPUTextureAspect_DepthOnly;
	case KORE_GPU_IMAGE_COPY_ASPECT_STENCIL_ONLY:
		return WGPUTextureAspect_StencilOnly;
	}

	assert(false);
	return WGPUTextureAspect_All;
}

void kore_webgpu_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {
	WGPUImageCopyBuffer copy_buffer = {
	    .layout =
	        {
	            .offset       = source->offset,
	            .bytesPerRow  = source->bytes_per_row,
	            .rowsPerImage = source->rows_per_image,
	        },
	    .buffer = source->buffer->webgpu.has_copy_buffer ? source->buffer->webgpu.copy_buffer : source->buffer->webgpu.buffer,
	};

	WGPUImageCopyTexture copy_texture = {
	    .texture  = destination->texture->webgpu.texture,
	    .mipLevel = destination->mip_level,
	    .origin =
	        {
	            .x = destination->origin_x,
	            .y = destination->origin_y,
	            .z = destination->origin_z,
	        },
	    .aspect = convert_texture_aspect(destination->aspect),
	};

	WGPUExtent3D size = {
	    .width              = width,
	    .height             = height,
	    .depthOrArrayLayers = depth_or_array_layers,
	};

	wgpuCommandEncoderCopyBufferToTexture(list->webgpu.command_encoder, &copy_buffer, &copy_texture, &size);
}

void kore_webgpu_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {
	WGPUImageCopyTexture copy_texture = {
	    .texture  = source->texture->webgpu.texture,
	    .mipLevel = source->mip_level,
	    .origin =
	        {
	            .x = source->origin_x,
	            .y = source->origin_y,
	            .z = source->origin_z,
	        },
	    .aspect = convert_texture_aspect(source->aspect),
	};

	WGPUImageCopyBuffer copy_buffer = {
	    .layout =
	        {
	            .offset       = destination->offset,
	            .bytesPerRow  = destination->bytes_per_row,
	            .rowsPerImage = destination->rows_per_image,
	        },
	    .buffer = destination->buffer->webgpu.buffer,
	};

	WGPUExtent3D size = {
	    .width              = width,
	    .height             = height,
	    .depthOrArrayLayers = depth_or_array_layers,
	};

	wgpuCommandEncoderCopyTextureToBuffer(list->webgpu.command_encoder, &copy_texture, &copy_buffer, &size);
}

void kore_webgpu_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                      const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                      uint32_t depth_or_array_layers) {
	WGPUImageCopyTexture source_texture = {
	    .texture  = source->texture->webgpu.texture,
	    .mipLevel = source->mip_level,
	    .origin =
	        {
	            .x = source->origin_x,
	            .y = source->origin_y,
	            .z = source->origin_z,
	        },
	    .aspect = convert_texture_aspect(source->aspect),
	};

	WGPUImageCopyTexture destination_texture = {
	    .texture  = destination->texture->webgpu.texture,
	    .mipLevel = destination->mip_level,
	    .origin =
	        {
	            .x = destination->origin_x,
	            .y = destination->origin_y,
	            .z = destination->origin_z,
	        },
	    .aspect = convert_texture_aspect(source->aspect),
	};

	WGPUExtent3D size = {
	    .width              = width,
	    .height             = height,
	    .depthOrArrayLayers = depth_or_array_layers,
	};

	wgpuCommandEncoderCopyTextureToTexture(list->webgpu.command_encoder, &source_texture, &destination_texture, &size);
}

void kore_webgpu_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {}

void kore_webgpu_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_webgpu_compute_pipeline *pipeline) {
	WGPUComputePassDescriptor compute_pass_descriptor = {0};
	list->webgpu.compute_pass_encoder                 = wgpuCommandEncoderBeginComputePass(list->webgpu.command_encoder, &compute_pass_descriptor);

	wgpuComputePassEncoderSetPipeline(list->webgpu.compute_pass_encoder, pipeline->compute_pipeline);
}

void kore_webgpu_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {
	wgpuComputePassEncoderDispatchWorkgroups(list->webgpu.compute_pass_encoder, workgroup_count_x, workgroup_count_y, workgroup_count_z);
}

void kore_webgpu_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, kore_gpu_raytracing_volume *volume) {}

void kore_webgpu_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_webgpu_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                          kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_webgpu_command_list_set_ray_pipeline(kore_gpu_command_list *list, kore_webgpu_ray_pipeline *pipeline) {}

void kore_webgpu_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth) {}

void kore_webgpu_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth) {}

void kore_webgpu_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

void kore_webgpu_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color) {}

void kore_webgpu_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference) {}

void kore_webgpu_command_list_set_name(kore_gpu_command_list *list, const char *name) {}

void kore_webgpu_command_list_push_debug_group(kore_gpu_command_list *list, const char *name) {}

void kore_webgpu_command_list_pop_debug_group(kore_gpu_command_list *list) {}

void kore_webgpu_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name) {}

void kore_webgpu_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index) {}

void kore_webgpu_command_list_end_occlusion_query(kore_gpu_command_list *list) {}

void kore_webgpu_command_list_resolve_query_set(kore_gpu_command_list *list, kore_gpu_query_set *query_set, uint32_t first_query, uint32_t query_count,
                                                kore_gpu_buffer *destination, uint64_t destination_offset) {}

void kore_webgpu_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset, uint32_t max_draw_count,
                                            kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_webgpu_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                    uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_webgpu_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset) {}

void kore_webgpu_command_list_queue_buffer_access(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {}

void kore_webgpu_command_list_queue_descriptor_set_access(kore_gpu_command_list *list, kore_webgpu_descriptor_set *descriptor_set) {}
