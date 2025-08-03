#include <kore3/metal/descriptorset_functions.h>
#include <kore3/metal/descriptorset_structs.h>

#include <kore3/metal/texture_functions.h>

#include <kore3/util/align.h>

void kore_metal_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;
	id<MTLBuffer>               metal_buffer           = (__bridge id<MTLBuffer>)buffer->metal.buffer;

	[render_command_encoder useResource:metal_buffer usage:MTLResourceUsageRead stages:MTLRenderStageVertex | MTLRenderStageFragment];

	if (buffer->metal.host_visible) {
		kore_metal_command_list_queue_buffer_access(list, &buffer->metal, (uint32_t)offset, (uint32_t)size);
	}
}

void kore_metal_descriptor_set_prepare_texture(kore_gpu_command_list *list, void *texture_view, bool writable) {
	id<MTLTexture> metal_texture = (__bridge id<MTLTexture>)texture_view;

	if (list->metal.render_command_encoder != NULL) {
		id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;
		[render_command_encoder useResource:metal_texture
		                              usage:writable ? MTLResourceUsageWrite : MTLResourceUsageRead
		                             stages:MTLRenderStageVertex | MTLRenderStageFragment];
	}
	else if (list->metal.compute_command_encoder != NULL) {
		id<MTLComputeCommandEncoder> compute_command_encoder = (__bridge id<MTLComputeCommandEncoder>)list->metal.compute_command_encoder;

		if (@available(macOS 13.0, *)) {
			[compute_command_encoder useResource:metal_texture usage:writable ? MTLResourceUsageWrite : MTLResourceUsageRead];
		}
		else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			[compute_command_encoder useResource:metal_texture usage:writable ? MTLResourceUsageWrite : MTLResourceUsageSample];
#pragma clang diagnostic pop
		}
	}
	else {
		assert(false);
	}
}
