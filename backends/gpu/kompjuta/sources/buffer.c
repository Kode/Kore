#include <kore3/kompjuta/buffer_functions.h>

#include <kore3/gpu/buffer.h>

#include <string.h>

void kore_kompjuta_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {}

void kore_kompjuta_buffer_destroy(kore_gpu_buffer *buffer) {}

void *kore_kompjuta_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	return buffer->kompjuta.data;
}

void *kore_kompjuta_buffer_lock_all(kore_gpu_buffer *buffer) {
	return buffer->kompjuta.data;
}

void *kore_kompjuta_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	uint8_t *data = (uint8_t *)buffer->kompjuta.data;
	return &data[offset];
}

void *kore_kompjuta_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	uint8_t *data = (uint8_t *)buffer->kompjuta.data;
	return &data[offset];
}

void kore_kompjuta_buffer_unlock(kore_gpu_buffer *buffer) {}
