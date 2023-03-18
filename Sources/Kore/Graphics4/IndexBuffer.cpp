#include "Graphics.h"

using namespace Kore;
using namespace Kore::Graphics4;

IndexBuffer::IndexBuffer(int count, IndexBufferFormat format, Usage usage) {
	kinc_g4_index_buffer_init(&kincBuffer, count, format == Uint32 ? KINC_G4_INDEX_BUFFER_FORMAT_32BIT : KINC_G4_INDEX_BUFFER_FORMAT_16BIT,
	                          (kinc_g4_usage_t)usage);
}

IndexBuffer::~IndexBuffer() {
	kinc_g4_index_buffer_destroy(&kincBuffer);
}

void *IndexBuffer::lock() {
	return kinc_g4_index_buffer_lock_all(&kincBuffer);
}

void *IndexBuffer::lock(int start, int count) {
	return kinc_g4_index_buffer_lock(&kincBuffer, start, count);
}

void IndexBuffer::unlock() {
	kinc_g4_index_buffer_unlock_all(&kincBuffer);
}

void IndexBuffer::unlock(int count) {
	kinc_g4_index_buffer_unlock(&kincBuffer, count);
}

int IndexBuffer::count() {
	return kinc_g4_index_buffer_count(&kincBuffer);
}
