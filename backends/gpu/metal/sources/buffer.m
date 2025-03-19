#include <kore3/metal/buffer_functions.h>

#include <kore3/gpu/buffer.h>

void kore_metal_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {}

void kore_metal_buffer_destroy(kore_gpu_buffer *buffer) {}

void *kore_metal_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	id<MTLBuffer> metal_buffer = (__bridge id<MTLBuffer>)buffer->metal.buffer;
	buffer->metal.locked_data  = (void *)[metal_buffer contents];
	return buffer->metal.locked_data;
}

void *kore_metal_buffer_lock_all(kore_gpu_buffer *buffer) {
	id<MTLBuffer> metal_buffer = (__bridge id<MTLBuffer>)buffer->metal.buffer;
	buffer->metal.locked_data  = (void *)[metal_buffer contents];
	return buffer->metal.locked_data;
}

void *kore_metal_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	id<MTLBuffer> metal_buffer = (__bridge id<MTLBuffer>)buffer->metal.buffer;
	uint8_t      *data         = (uint8_t *)[metal_buffer contents];
	buffer->metal.locked_data  = &data[offset];
	return buffer->metal.locked_data;
}

void *kore_metal_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	id<MTLBuffer> metal_buffer = (__bridge id<MTLBuffer>)buffer->metal.buffer;
	uint8_t      *data         = (uint8_t *)[metal_buffer contents];
	buffer->metal.locked_data  = &data[offset];
	return buffer->metal.locked_data;
}

void kore_metal_buffer_unlock(kore_gpu_buffer *buffer) {}
