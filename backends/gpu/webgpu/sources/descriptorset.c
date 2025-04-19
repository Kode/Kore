#include <kore3/webgpu/descriptorset_functions.h>
#include <kore3/webgpu/descriptorset_structs.h>

#include <kore3/webgpu/texture_functions.h>

#include <kore3/util/align.h>

void kore_webgpu_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer) {
	if (buffer->webgpu.copy_scheduled) {
		assert(scheduled_buffer_uploads_count < 256);
		scheduled_buffer_uploads[scheduled_buffer_uploads_count++] = &buffer->webgpu;
		buffer->webgpu.copy_scheduled                              = false;
	}
}
