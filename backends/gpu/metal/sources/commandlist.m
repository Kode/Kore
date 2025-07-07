#include <kore3/metal/commandlist_functions.h>

#include "metalunit.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>

#include <kore3/metal/descriptorset_structs.h>
#include <kore3/metal/pipeline_structs.h>
#include <kore3/metal/texture_functions.h>

#include <kore3/util/align.h>

#include <assert.h>

void kore_metal_command_list_destroy(kore_gpu_command_list *list) {
	CFRelease(list->metal.command_queue);
	list->metal.command_queue = NULL;

	CFRelease(list->metal.command_buffer);
	list->metal.command_buffer = NULL;
}

static void kore_metal_command_list_begin_blit_pass(kore_gpu_command_list *list);
static void kore_metal_command_list_end_blit_pass(kore_gpu_command_list *list);
static void kore_metal_command_list_begin_compute_pass(kore_gpu_command_list *list);
static void kore_metal_command_list_end_compute_pass(kore_gpu_command_list *list);

static MTLLoadAction convert_load_op(kore_gpu_load_op op) {
	switch (op) {
	case KORE_GPU_LOAD_OP_LOAD:
		return MTLLoadActionLoad;
	case KORE_GPU_LOAD_OP_CLEAR:
		return MTLLoadActionClear;
	}
}

static MTLStoreAction convert_store_op(kore_gpu_store_op op) {
	switch (op) {
	case KORE_GPU_STORE_OP_STORE:
		return MTLStoreActionStore;
	case KORE_GPU_STORE_OP_DISCARD:
		return MTLStoreActionDontCare;
	}
}

void kore_metal_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	kore_metal_command_list_end_compute_pass(list);
	kore_metal_command_list_end_blit_pass(list);

	id<MTLTexture> textures[8];
	for (size_t index = 0; index < parameters->color_attachments_count; ++index) {
		textures[index] = (__bridge id<MTLTexture>)parameters->color_attachments[index].texture.texture->metal.texture;
	}

	MTLRenderPassDescriptor *render_pass_descriptor = [MTLRenderPassDescriptor renderPassDescriptor];

	for (size_t index = 0; index < parameters->color_attachments_count; ++index) {
		render_pass_descriptor.colorAttachments[index].texture     = textures[index];
		render_pass_descriptor.colorAttachments[index].loadAction  = convert_load_op(parameters->color_attachments[index].load_op);
		render_pass_descriptor.colorAttachments[index].storeAction = convert_store_op(parameters->color_attachments[index].store_op);
		render_pass_descriptor.colorAttachments[index].clearColor =
		    MTLClearColorMake(parameters->color_attachments[index].clear_value.r, parameters->color_attachments[index].clear_value.g,
		                      parameters->color_attachments[index].clear_value.b, parameters->color_attachments[index].clear_value.a);
		render_pass_descriptor.colorAttachments[index].slice = parameters->color_attachments[index].texture.base_array_layer;
		render_pass_descriptor.colorAttachments[index].level = parameters->color_attachments[index].texture.base_mip_level;
	}

	id<MTLTexture> depth_texture = nil;
	if (parameters->depth_stencil_attachment.texture != NULL) {
		depth_texture = (__bridge id<MTLTexture>)parameters->depth_stencil_attachment.texture->metal.texture;
	}
	render_pass_descriptor.depthAttachment.clearDepth  = parameters->depth_stencil_attachment.depth_clear_value;
	render_pass_descriptor.depthAttachment.loadAction  = convert_load_op(parameters->depth_stencil_attachment.depth_load_op);
	render_pass_descriptor.depthAttachment.storeAction = convert_store_op(parameters->depth_stencil_attachment.depth_store_op);
	render_pass_descriptor.depthAttachment.texture     = depth_texture;

	if (parameters->depth_stencil_attachment.texture != NULL && has_stencil(parameters->depth_stencil_attachment.texture->format)) {
		render_pass_descriptor.stencilAttachment.clearStencil = parameters->depth_stencil_attachment.stencil_clear_value;
		render_pass_descriptor.stencilAttachment.loadAction   = convert_load_op(parameters->depth_stencil_attachment.stencil_load_op);
		render_pass_descriptor.stencilAttachment.storeAction  = convert_store_op(parameters->depth_stencil_attachment.stencil_store_op);
		render_pass_descriptor.stencilAttachment.texture      = depth_texture;
	}

	id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
	list->metal.render_command_encoder  = (__bridge_retained void *)[command_buffer renderCommandEncoderWithDescriptor:render_pass_descriptor];
}

void kore_metal_command_list_end_render_pass(kore_gpu_command_list *list) {
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;
	[render_command_encoder endEncoding];

	CFRelease(list->metal.render_command_encoder);
	list->metal.render_command_encoder = NULL;
}

void kore_metal_command_list_present(kore_gpu_command_list *list) {
	id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
	[command_buffer presentDrawable:drawable];

	drawable = nil;
}

static void kore_metal_command_list_begin_blit_pass(kore_gpu_command_list *list) {
	assert(list->metal.render_command_encoder == NULL);

	kore_metal_command_list_end_compute_pass(list);

	if (list->metal.blit_command_encoder == NULL) {
		id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
		list->metal.blit_command_encoder    = (__bridge_retained void *)[command_buffer blitCommandEncoder];
	}
}

static void kore_metal_command_list_end_blit_pass(kore_gpu_command_list *list) {
	if (list->metal.blit_command_encoder != NULL) {
		id<MTLBlitCommandEncoder> blit_command_encoder = (__bridge id<MTLBlitCommandEncoder>)list->metal.blit_command_encoder;
		[blit_command_encoder endEncoding];

		CFRelease(list->metal.blit_command_encoder);
		list->metal.blit_command_encoder = NULL;
	}
}

static void kore_metal_command_list_begin_compute_pass(kore_gpu_command_list *list) {
	assert(list->metal.render_command_encoder == NULL);

	kore_metal_command_list_end_blit_pass(list);

	if (list->metal.compute_command_encoder == NULL) {
		id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
		list->metal.compute_command_encoder = (__bridge_retained void *)[command_buffer computeCommandEncoder];
	}
}

static void kore_metal_command_list_end_compute_pass(kore_gpu_command_list *list) {
	if (list->metal.compute_command_encoder != NULL) {
		id<MTLComputeCommandEncoder> compute_command_encoder = (__bridge id<MTLComputeCommandEncoder>)list->metal.compute_command_encoder;
		[compute_command_encoder endEncoding];

		CFRelease(list->metal.compute_command_encoder);
		list->metal.compute_command_encoder = NULL;
	}
}

void kore_metal_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset) {
	list->metal.index_buffer        = buffer->metal.buffer;
	list->metal.sixteen_bit_indices = index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? true : false;
	
	if (buffer->metal.host_visible) {
		kore_metal_command_list_queue_buffer_access(list, &buffer->metal, (uint32_t)offset, (uint32_t)(buffer->metal.size - offset));
	}
}

void kore_metal_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_metal_buffer *buffer, uint64_t offset, uint64_t size,
                                               uint64_t stride) {
	id<MTLBuffer>               metal_buffer           = (__bridge id<MTLBuffer>)buffer->buffer;
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;

	[render_command_encoder setVertexBuffer:metal_buffer offset:offset atIndex:slot];
	
	if (buffer->host_visible) {
		kore_metal_command_list_queue_buffer_access(list, buffer, (uint32_t)offset, (uint32_t)size);
	}
}

void kore_metal_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_metal_render_pipeline *pipeline) {
	id<MTLRenderPipelineState>  metal_pipeline         = (__bridge id<MTLRenderPipelineState>)pipeline->pipeline;
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;

	[render_command_encoder setRenderPipelineState:metal_pipeline];

	if (pipeline->depth_stencil_state != NULL) {
		id<MTLDepthStencilState> depth_stencil_state = (__bridge id<MTLDepthStencilState>)pipeline->depth_stencil_state;
		[render_command_encoder setDepthStencilState:depth_stencil_state];
	}
}

void kore_metal_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
}

void kore_metal_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t base_vertex,
                                          uint32_t first_instance) {
	id<MTLBuffer> index_buffer = (__bridge id<MTLBuffer>)list->metal.index_buffer;

	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;

	[render_command_encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
	                                   indexCount:index_count
	                                    indexType:list->metal.sixteen_bit_indices ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32
	                                  indexBuffer:index_buffer
	                            indexBufferOffset:first_index
	                                instanceCount:instance_count
	                                   baseVertex:base_vertex
	                                 baseInstance:first_instance];
}

void kore_metal_command_list_set_descriptor_set(kore_gpu_command_list *list, uint32_t vertex_table_index, uint32_t fragment_table_index,
                                                uint32_t compute_table_index, struct kore_metal_descriptor_set *set, kore_gpu_buffer **dynamic_buffers,
                                                uint32_t *dynamic_offsets, uint32_t *dynamic_sizes, uint32_t dynamic_count) {
	if (list->metal.render_command_encoder == NULL) {
		id<MTLComputeCommandEncoder> compute_command_encoder = (__bridge id<MTLComputeCommandEncoder>)list->metal.compute_command_encoder;
		id<MTLBuffer>                metal_buffer            = (__bridge id<MTLBuffer>)set->argument_buffer.metal.buffer;

		[compute_command_encoder setBuffer:metal_buffer offset:0 atIndex:compute_table_index];

		for (uint32_t dynamic_index = 0; dynamic_index < dynamic_count; ++dynamic_index) {
			id<MTLBuffer> dynamic_buffer = (__bridge id<MTLBuffer>)dynamic_buffers[dynamic_index]->metal.buffer;

			[compute_command_encoder setBuffer:dynamic_buffer offset:dynamic_offsets[dynamic_index] atIndex:dynamic_index + 2];
		}
	}
	else {
		id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;
		id<MTLBuffer>               metal_buffer           = (__bridge id<MTLBuffer>)set->argument_buffer.metal.buffer;

		[render_command_encoder setVertexBuffer:metal_buffer offset:0 atIndex:vertex_table_index];
		[render_command_encoder setFragmentBuffer:metal_buffer offset:0 atIndex:fragment_table_index];

		for (uint32_t dynamic_index = 0; dynamic_index < dynamic_count; ++dynamic_index) {
			id<MTLBuffer> dynamic_buffer = (__bridge id<MTLBuffer>)dynamic_buffers[dynamic_index]->metal.buffer;

			[render_command_encoder setVertexBuffer:dynamic_buffer offset:dynamic_offsets[dynamic_index] atIndex:dynamic_index + vertex_table_index + 1];
			[render_command_encoder setFragmentBuffer:dynamic_buffer offset:dynamic_offsets[dynamic_index] atIndex:dynamic_index + fragment_table_index + 1];
		}
	}
}

void kore_metal_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                   uint64_t destination_offset, uint64_t size) {
	kore_metal_command_list_begin_blit_pass(list);

	id<MTLBlitCommandEncoder> blit_command_encoder = (__bridge id<MTLBlitCommandEncoder>)list->metal.blit_command_encoder;
	id<MTLBuffer>             source_buffer        = (__bridge id<MTLBuffer>)source->metal.buffer;
	id<MTLBuffer>             destination_buffer   = (__bridge id<MTLBuffer>)destination->metal.buffer;

	[blit_command_encoder copyFromBuffer:source_buffer sourceOffset:source_offset toBuffer:destination_buffer destinationOffset:destination_offset size:size];
}

void kore_metal_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                    const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers) {
	kore_metal_command_list_begin_blit_pass(list);

	id<MTLBlitCommandEncoder> blit_command_encoder = (__bridge id<MTLBlitCommandEncoder>)list->metal.blit_command_encoder;
	id<MTLBuffer>             source_buffer        = (__bridge id<MTLBuffer>)source->buffer->metal.buffer;
	id<MTLTexture>            destination_texture  = (__bridge id<MTLTexture>)destination->texture->metal.texture;

	switch (destination->texture->metal.dimension) {
	case KORE_GPU_TEXTURE_DIMENSION_1D:
		assert(height == 1);
		[blit_command_encoder copyFromBuffer:source_buffer
		                        sourceOffset:source->offset
		                   sourceBytesPerRow:source->bytes_per_row
		                 sourceBytesPerImage:source->bytes_per_row
		                          sourceSize:MTLSizeMake(width, 1, 1)
		                           toTexture:destination_texture
		                    destinationSlice:destination->origin_y
		                    destinationLevel:destination->mip_level
		                   destinationOrigin:MTLOriginMake(destination->origin_x, 0, 0)];
		break;
	case KORE_GPU_TEXTURE_DIMENSION_2D:
		[blit_command_encoder copyFromBuffer:source_buffer
		                        sourceOffset:source->offset
		                   sourceBytesPerRow:source->bytes_per_row
		                 sourceBytesPerImage:source->bytes_per_row * source->rows_per_image
		                          sourceSize:MTLSizeMake(width, height, 1)
		                           toTexture:destination_texture
		                    destinationSlice:destination->origin_z
		                    destinationLevel:destination->mip_level
		                   destinationOrigin:MTLOriginMake(destination->origin_x, destination->origin_y, 0)];
		break;
	case KORE_GPU_TEXTURE_DIMENSION_3D:
		[blit_command_encoder copyFromBuffer:source_buffer
		                        sourceOffset:source->offset
		                   sourceBytesPerRow:source->bytes_per_row
		                 sourceBytesPerImage:source->bytes_per_row * source->rows_per_image * depth_or_array_layers
		                          sourceSize:MTLSizeMake(width, height, depth_or_array_layers)
		                           toTexture:destination_texture
		                    destinationSlice:0
		                    destinationLevel:destination->mip_level
		                   destinationOrigin:MTLOriginMake(destination->origin_x, destination->origin_y, destination->origin_z)];
		break;
	}
}

void kore_metal_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                    const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers) {
	kore_metal_command_list_begin_blit_pass(list);

	id<MTLBlitCommandEncoder> blit_command_encoder = (__bridge id<MTLBlitCommandEncoder>)list->metal.blit_command_encoder;
	id<MTLTexture>            source_texture       = (__bridge id<MTLTexture>)source->texture->metal.texture;
	id<MTLBuffer>             destination_buffer   = (__bridge id<MTLBuffer>)destination->buffer->metal.buffer;

	[blit_command_encoder copyFromTexture:source_texture
	                          sourceSlice:0
	                          sourceLevel:source->mip_level
	                         sourceOrigin:MTLOriginMake(source->origin_x, source->origin_y, source->origin_z)
	                           sourceSize:MTLSizeMake(width, height, depth_or_array_layers)
	                             toBuffer:destination_buffer
	                    destinationOffset:destination->offset
	               destinationBytesPerRow:destination->bytes_per_row
	             destinationBytesPerImage:destination->bytes_per_row * destination->rows_per_image];
}

void kore_metal_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {
	kore_metal_command_list_begin_blit_pass(list);

	id<MTLBlitCommandEncoder> blit_command_encoder = (__bridge id<MTLBlitCommandEncoder>)list->metal.blit_command_encoder;
	id<MTLTexture>            source_texture       = (__bridge id<MTLTexture>)source->texture->metal.texture;
	id<MTLTexture>            destination_texture  = (__bridge id<MTLTexture>)destination->texture->metal.texture;

	[blit_command_encoder copyFromTexture:source_texture
	                          sourceSlice:0
	                          sourceLevel:source->mip_level
	                         sourceOrigin:MTLOriginMake(source->origin_x, source->origin_y, source->origin_z)
	                           sourceSize:MTLSizeMake(width, height, depth_or_array_layers)
	                            toTexture:destination_texture
	                     destinationSlice:0
	                     destinationLevel:destination->mip_level
	                    destinationOrigin:MTLOriginMake(destination->origin_x, destination->origin_y, destination->origin_z)];
}

void kore_metal_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {
	kore_metal_command_list_begin_blit_pass(list);

	id<MTLBlitCommandEncoder> blit_command_encoder = (__bridge id<MTLBlitCommandEncoder>)list->metal.blit_command_encoder;
	id<MTLBuffer>             metal_buffer         = (__bridge id<MTLBuffer>)buffer->metal.buffer;

	[blit_command_encoder fillBuffer:metal_buffer range:NSMakeRange(offset, size) value:0];
}

static uint32_t threads_per_threadsgroup_x = 0;
static uint32_t threads_per_threadsgroup_y = 0;
static uint32_t threads_per_threadsgroup_z = 0;

void kore_metal_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_metal_compute_pipeline *pipeline, uint32_t threads_x, uint32_t threads_y,
                                                  uint32_t threads_z) {
	threads_per_threadsgroup_x = threads_x;
	threads_per_threadsgroup_y = threads_y;
	threads_per_threadsgroup_z = threads_z;

	kore_metal_command_list_begin_compute_pass(list);

	id<MTLComputePipelineState>  metal_pipeline          = (__bridge id<MTLComputePipelineState>)pipeline->pipeline;
	id<MTLComputeCommandEncoder> compute_command_encoder = (__bridge id<MTLComputeCommandEncoder>)list->metal.compute_command_encoder;

	[compute_command_encoder setComputePipelineState:metal_pipeline];
}

void kore_metal_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {
	kore_metal_command_list_begin_compute_pass(list);

	id<MTLComputeCommandEncoder> compute_command_encoder = (__bridge id<MTLComputeCommandEncoder>)list->metal.compute_command_encoder;
	[compute_command_encoder dispatchThreadgroups:MTLSizeMake(workgroup_count_x, workgroup_count_y, workgroup_count_z)
	                        threadsPerThreadgroup:MTLSizeMake(threads_per_threadsgroup_x, threads_per_threadsgroup_y, threads_per_threadsgroup_z)];
}

void kore_metal_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t vertex_table_index, uint32_t fragment_table_index,
                                                uint32_t compute_table_index, const void *data, size_t data_size) {
	if (list->metal.render_command_encoder != NULL) {
		id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;
		[render_command_encoder setVertexBytes:data length:data_size atIndex:vertex_table_index];
		[render_command_encoder setFragmentBytes:data length:data_size atIndex:fragment_table_index];
	}
	else if (list->metal.compute_command_encoder != NULL) {
		id<MTLComputeCommandEncoder> compute_command_encoder = (__bridge id<MTLComputeCommandEncoder>)list->metal.compute_command_encoder;
		[compute_command_encoder setBytes:data length:data_size atIndex:compute_table_index];
	}
}

void kore_metal_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, kore_gpu_raytracing_volume *volume) {}

void kore_metal_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_metal_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                         kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_metal_command_list_set_ray_pipeline(kore_gpu_command_list *list, kore_metal_ray_pipeline *pipeline) {}

void kore_metal_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth) {}

void kore_metal_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth) {}

void kore_metal_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

void kore_metal_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color) {}

void kore_metal_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference) {}

void kore_metal_command_list_set_name(kore_gpu_command_list *list, const char *name) {}

void kore_metal_command_list_push_debug_group(kore_gpu_command_list *list, const char *name) {}

void kore_metal_command_list_pop_debug_group(kore_gpu_command_list *list) {}

void kore_metal_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name) {}

void kore_metal_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index) {}

void kore_metal_command_list_end_occlusion_query(kore_gpu_command_list *list) {}

void kore_metal_command_list_resolve_query_set(kore_gpu_command_list *list, kore_gpu_query_set *query_set, uint32_t first_query, uint32_t query_count,
                                               kore_gpu_buffer *destination, uint64_t destination_offset) {}

void kore_metal_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset, uint32_t max_draw_count,
                                           kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_metal_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                   uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_metal_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset) {}

void kore_metal_command_list_queue_buffer_access(kore_gpu_command_list *list, kore_metal_buffer *buffer, uint32_t offset, uint32_t size) {
	kore_metal_buffer_access access;
	access.buffer = buffer;
	access.offset = offset;
	access.size   = size;

	list->metal.queued_buffer_accesses[list->metal.queued_buffer_accesses_count] = access;
	list->metal.queued_buffer_accesses_count += 1;
}


void kore_metal_command_list_queue_descriptor_set_access(kore_gpu_command_list *list, struct kore_metal_descriptor_set *descriptor_set) {}
