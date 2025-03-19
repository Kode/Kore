#include <kore3/metal/commandlist_functions.h>

#include "metalunit.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>

#include <kore3/metal/pipeline_structs.h>
#include <kore3/metal/texture_functions.h>

#include <kore3/util/align.h>

#include <assert.h>

void kore_metal_command_list_destroy(kore_gpu_command_list *list) {}

void kore_metal_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	id<MTLTexture> texture = (__bridge id<MTLTexture>)parameters->color_attachments[0].texture.texture->metal.texture;

	MTLRenderPassDescriptor *render_pass_descriptor        = [MTLRenderPassDescriptor renderPassDescriptor];
	render_pass_descriptor.colorAttachments[0].texture     = texture;
	render_pass_descriptor.colorAttachments[0].loadAction  = MTLLoadActionClear;
	render_pass_descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
	render_pass_descriptor.colorAttachments[0].clearColor  = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
	render_pass_descriptor.depthAttachment.clearDepth      = 1;
	render_pass_descriptor.depthAttachment.loadAction      = MTLLoadActionClear;
	render_pass_descriptor.depthAttachment.storeAction     = MTLStoreActionStore;
	render_pass_descriptor.depthAttachment.texture         = nil; // depthTexture;
	render_pass_descriptor.stencilAttachment.clearStencil  = 0;
	render_pass_descriptor.stencilAttachment.loadAction    = MTLLoadActionDontCare;
	render_pass_descriptor.stencilAttachment.storeAction   = MTLStoreActionDontCare;
	render_pass_descriptor.stencilAttachment.texture       = nil; // depthTexture;

	id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
	list->metal.render_command_encoder  = (__bridge_retained void *)[command_buffer renderCommandEncoderWithDescriptor:render_pass_descriptor];
}

void kore_metal_command_list_end_render_pass(kore_gpu_command_list *list) {
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;
	[render_command_encoder endEncoding];
	list->metal.render_command_encoder = NULL;
}

void kore_metal_command_list_present(kore_gpu_command_list *list) {
	id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
	[command_buffer presentDrawable:drawable];

	drawable = nil;
}

void kore_metal_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset,
                                              uint64_t size) {
	list->metal.index_buffer        = buffer->metal.buffer;
	list->metal.sixteen_bit_indices = index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? true : false;
}

void kore_metal_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_metal_buffer *buffer, uint64_t offset, uint64_t size,
                                               uint64_t stride) {
	id<MTLBuffer>               metal_buffer           = (__bridge id<MTLBuffer>)buffer->buffer;
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;

	[render_command_encoder setVertexBuffer:metal_buffer offset:offset atIndex:slot];
}

void kore_metal_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_metal_render_pipeline *pipeline) {
	id<MTLRenderPipelineState>  metal_pipeline         = (__bridge id<MTLRenderPipelineState>)pipeline->pipeline;
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;

	[render_command_encoder setRenderPipelineState:metal_pipeline];
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

void kore_metal_command_list_set_descriptor_table(kore_gpu_command_list *list, uint32_t table_index, struct kore_metal_descriptor_set *set,
                                                  kore_gpu_buffer **dynamic_buffers, uint32_t *dynamic_offsets, uint32_t *dynamic_sizes) {}

void kore_metal_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size) {}

void kore_metal_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                   uint64_t destination_offset, uint64_t size) {}

void kore_metal_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                    const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers) {}

void kore_metal_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                    const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers) {}

void kore_metal_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {}

void kore_metal_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {}

void kore_metal_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_metal_compute_pipeline *pipeline) {}

void kore_metal_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {}

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

void kore_metal_command_list_queue_buffer_access(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {}

void kore_metal_command_list_queue_descriptor_set_access(kore_gpu_command_list *list, struct kore_metal_descriptor_set *descriptor_set) {}
