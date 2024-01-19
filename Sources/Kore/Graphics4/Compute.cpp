#include "Compute.h"

#include <string.h>

using namespace Kore;

Graphics4::ComputeShader::ComputeShader(void *source, int length) {
	kinc_g4_compute_shader_init(&kincImpl, source, length);
}

Graphics4::ComputeShader::~ComputeShader() {
	kinc_g4_compute_shader_destroy(&kincImpl);
}

Graphics4::ConstantLocation Graphics4::ComputeShader::getConstantLocation(const char *name) {
	Graphics4::ConstantLocation location;
	location.kincConstant = kinc_g4_compute_shader_get_constant_location(&kincImpl, name);
	return location;
}

Graphics4::TextureUnit Graphics4::ComputeShader::getTextureUnit(const char *name) {
	Graphics4::TextureUnit unit;
	unit.kincUnit = kinc_g4_compute_shader_get_texture_unit(&kincImpl, name);
	return unit;
}

#ifdef KORE_OPENGL
Graphics4::ShaderStorageBuffer::ShaderStorageBuffer(int count, Graphics4::VertexData type) {
	kinc_shader_storage_buffer_init(&kincImpl, count, (kinc_g4_vertex_data_t)type);
}

Graphics4::ShaderStorageBuffer::~ShaderStorageBuffer() {
	kinc_shader_storage_buffer_destroy(&kincImpl);
}

int *Graphics4::ShaderStorageBuffer::lock() {
	return kinc_shader_storage_buffer_lock(&kincImpl);
}

void Graphics4::ShaderStorageBuffer::unlock() {
	kinc_shader_storage_buffer_unlock(&kincImpl);
}

int Graphics4::ShaderStorageBuffer::count() {
	return kinc_shader_storage_buffer_count(&kincImpl);
}

void Graphics4::ShaderStorageBuffer::_set() {
	kinc_shader_storage_buffer_internal_set(&kincImpl);
}
#endif
