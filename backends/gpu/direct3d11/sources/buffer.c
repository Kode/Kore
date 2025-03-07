#include <kore3/direct3d11/buffer_functions.h>

#include <kore3/gpu/buffer.h>

void kore_d3d11_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {}

void kore_d3d11_buffer_destroy(kore_gpu_buffer *buffer) {
	buffer->d3d11.buffer->lpVtbl->Release(buffer->d3d11.buffer);
}

void *kore_d3d11_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	return kore_d3d11_buffer_lock_all(buffer);
}

void *kore_d3d11_buffer_lock_all(kore_gpu_buffer *buffer) {
	D3D11_MAPPED_SUBRESOURCE mapped_subresource = {0};
	buffer->d3d11.context->lpVtbl->Map(buffer->d3d11.context, (ID3D11Resource *)buffer->d3d11.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	return mapped_subresource.pData;
}

void *kore_d3d11_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	return kore_d3d11_buffer_lock(buffer, offset, size);
}

void *kore_d3d11_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	D3D11_MAPPED_SUBRESOURCE mapped_subresource = {0};
	buffer->d3d11.context->lpVtbl->Map(buffer->d3d11.context, (ID3D11Resource *)buffer->d3d11.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	uint8_t *data = (uint8_t *)mapped_subresource.pData;
	assert(data != NULL);
	return &data[offset];
}

void kore_d3d11_buffer_unlock(kore_gpu_buffer *buffer) {
	buffer->d3d11.context->lpVtbl->Unmap(buffer->d3d11.context, (ID3D11Resource *)buffer->d3d11.buffer, 0);
}
