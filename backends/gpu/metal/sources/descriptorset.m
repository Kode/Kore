#include <kore3/metal/descriptorset_functions.h>
#include <kore3/metal/descriptorset_structs.h>

#include <kore3/metal/texture_functions.h>

#include <kore3/util/align.h>

void kore_metal_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {
	id<MTLRenderCommandEncoder> render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)list->metal.render_command_encoder;
	id<MTLBuffer>               metal_buffer           = (__bridge id<MTLBuffer>)buffer->metal.buffer;

	[render_command_encoder useResource:metal_buffer usage:MTLResourceUsageRead stages:MTLRenderStageVertex | MTLRenderStageFragment];
}

void kore_metal_descriptor_set_prepare_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view) {}
