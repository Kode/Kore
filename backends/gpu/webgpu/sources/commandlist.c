#include <kore3/webgpu/commandlist_functions.h>

#include "webgpuunit.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>

#include <kore3/webgpu/texture_functions.h>

#include <kore3/webgpu/pipeline_structs.h>

#include <kore3/util/align.h>

#include <assert.h>

void kore_webgpu_command_list_destroy(kore_gpu_command_list *list) {}

void kore_webgpu_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	WGPUTextureViewDescriptor texture_view_descriptor = {
	    .format          = WGPUTextureFormat_BGRA8Unorm,
	    .dimension       = WGPUTextureViewDimension_2D,
	    .arrayLayerCount = 1,
	    .mipLevelCount   = 1,
	};
	WGPUTextureView texture_view = wgpuTextureCreateView(parameters->color_attachments[0].texture.texture->webgpu.texture, &texture_view_descriptor);

	WGPURenderPassColorAttachment color_attachment = {
	    .view       = texture_view,
	    .loadOp     = WGPULoadOp_Clear,
	    .storeOp    = WGPUStoreOp_Store,
	    .clearValue = {0, 0, 0, 1},
	    .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
	};

	WGPURenderPassDescriptor render_pass_descriptor = {
	    .colorAttachmentCount = 1,
	    .colorAttachments     = &color_attachment,
	};

	list->webgpu.render_pass_encoder = wgpuCommandEncoderBeginRenderPass(list->webgpu.command_encoder, &render_pass_descriptor);
}

void kore_webgpu_command_list_end_render_pass(kore_gpu_command_list *list) {
	wgpuRenderPassEncoderEnd(list->webgpu.render_pass_encoder);
}

void kore_webgpu_command_list_present(kore_gpu_command_list *list) {}

void kore_webgpu_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset,
                                               uint64_t size) {
	if (buffer->webgpu.copy_scheduled) {
		assert(scheduled_buffer_uploads_count < 256);
		scheduled_buffer_uploads[scheduled_buffer_uploads_count++] = &buffer->webgpu;
		buffer->webgpu.copy_scheduled                              = false;
	}
	wgpuRenderPassEncoderSetIndexBuffer(list->webgpu.render_pass_encoder, buffer->webgpu.buffer,
	                                    index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? WGPUIndexFormat_Uint16 : WGPUIndexFormat_Uint32, offset, size);
}

void kore_webgpu_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_webgpu_buffer *buffer, uint64_t offset, uint64_t size,
                                                uint64_t stride) {
	if (buffer->copy_scheduled) {
		assert(scheduled_buffer_uploads_count < 256);
		scheduled_buffer_uploads[scheduled_buffer_uploads_count++] = buffer;
		buffer->copy_scheduled                                     = false;
	}
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

void kore_webgpu_command_list_set_descriptor_table(kore_gpu_command_list *list, uint32_t table_index, kore_webgpu_descriptor_set *set,
                                                   kore_gpu_buffer **dynamic_buffers, uint32_t *dynamic_offsets, uint32_t *dynamic_sizes) {}

void kore_webgpu_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size) {}

void kore_webgpu_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                    uint64_t destination_offset, uint64_t size) {}

void kore_webgpu_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {}

void kore_webgpu_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {}

void kore_webgpu_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                      const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                      uint32_t depth_or_array_layers) {}

void kore_webgpu_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {}

void kore_webgpu_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_webgpu_compute_pipeline *pipeline) {}

void kore_webgpu_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {}

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
