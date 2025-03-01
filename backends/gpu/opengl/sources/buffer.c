#include <kore3/opengl/buffer_functions.h>

#include <kore3/gpu/buffer.h>

#include <string.h>

void kore_opengl_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {
#if !defined(KORE_OPENGL_ES) && !defined(KORE_MACOS)
	glObjectLabel(GL_BUFFER, buffer->opengl.buffer, (GLsizei)strlen(name), name);
#endif
}

void kore_opengl_buffer_destroy(kore_gpu_buffer *buffer) {
	glDeleteBuffers(1, &buffer->opengl.buffer);
}

void *kore_opengl_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	return kore_opengl_buffer_lock_all(buffer);
}

void *kore_opengl_buffer_lock_all(kore_gpu_buffer *buffer) {
	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	void *data = glMapBufferRange(buffer->opengl.buffer_type, 0, buffer->opengl.size, GL_MAP_WRITE_BIT);
	glBindBuffer(buffer->opengl.buffer_type, 0);
	return data;
}

void *kore_opengl_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	return kore_opengl_buffer_lock(buffer, offset, size);
}

void *kore_opengl_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	void *data = glMapBufferRange(buffer->opengl.buffer_type, offset, size, GL_MAP_WRITE_BIT);
	glBindBuffer(buffer->opengl.buffer_type, 0);
	return data;
}

void kore_opengl_buffer_unlock(kore_gpu_buffer *buffer) {
	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	glUnmapBuffer(buffer->opengl.buffer_type);
	glBindBuffer(buffer->opengl.buffer_type, 0);
}
