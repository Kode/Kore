#include <kore3/opengl/buffer_functions.h>

#include <kore3/gpu/buffer.h>

#include <stdlib.h>
#include <string.h>

void kore_opengl_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {
#if !defined(KORE_OPENGL_ES) && !defined(KORE_MACOS)
	glObjectLabel(GL_BUFFER, buffer->opengl.buffer, (GLsizei)strlen(name), name);
#endif
}

void kore_opengl_buffer_destroy(kore_gpu_buffer *buffer) {
	glDeleteBuffers(1, &buffer->opengl.buffer);
#ifdef KORE_WEBGL
	free(buffer->opengl.locked_data);
#endif
}

void *kore_opengl_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	return kore_opengl_buffer_lock_all(buffer);
}

void *kore_opengl_buffer_lock_all(kore_gpu_buffer *buffer) {
#ifdef KORE_WEBGL
	if (buffer->opengl.buffer_type == GL_PIXEL_PACK_BUFFER) {
		glGetBufferSubData(buffer->opengl.buffer_type, 0, buffer->opengl.size, buffer->opengl.locked_data);
	}
	return buffer->opengl.locked_data;
#else
	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	buffer->opengl.locked_data = glMapBufferRange(buffer->opengl.buffer_type, 0, buffer->opengl.size,
	                                              buffer->opengl.buffer_type == GL_PIXEL_PACK_BUFFER ? GL_MAP_READ_BIT : GL_MAP_WRITE_BIT);
	glBindBuffer(buffer->opengl.buffer_type, 0);
	return buffer->opengl.locked_data;
#endif
}

void *kore_opengl_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	return kore_opengl_buffer_lock(buffer, offset, size);
}

void *kore_opengl_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
#ifdef KORE_WEBGL
	uint8_t *byte_data = (uint8_t *)buffer->opengl.locked_data;
	if (buffer->opengl.buffer_type == GL_PIXEL_PACK_BUFFER) {
		glGetBufferSubData(buffer->opengl.buffer_type, offset, size, &byte_data[offset]);
	}
	return &byte_data[offset];
#else
	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	buffer->opengl.locked_data =
	    glMapBufferRange(buffer->opengl.buffer_type, offset, size, buffer->opengl.buffer_type == GL_PIXEL_PACK_BUFFER ? GL_MAP_READ_BIT : GL_MAP_WRITE_BIT);
	glBindBuffer(buffer->opengl.buffer_type, 0);
	return buffer->opengl.locked_data;
#endif
}

void kore_opengl_buffer_unlock(kore_gpu_buffer *buffer) {
#ifdef KORE_WEBGL
	if (buffer->opengl.buffer_type != GL_PIXEL_PACK_BUFFER) {
		glBufferData(buffer->opengl.buffer_type, buffer->opengl.size, buffer->opengl.locked_data, GL_STATIC_DRAW);
	}
#else
	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	glUnmapBuffer(buffer->opengl.buffer_type);
	glBindBuffer(buffer->opengl.buffer_type, 0);
	buffer->opengl.locked_data = NULL;
#endif
}
