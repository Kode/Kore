#include "Compute.h"

#include <string.h>

using namespace Kore;

Graphics5::ComputeShader::ComputeShader(void *source, int length) {
	kinc_g5_compute_shader_init(&kincImpl, source, length);
}

Graphics5::ComputeShader::~ComputeShader() {
	kinc_g5_compute_shader_destroy(&kincImpl);
}

Graphics5::ConstantLocation Graphics5::ComputeShader::getConstantLocation(const char *name) {
	Graphics5::ConstantLocation location;
	location.kincConstantLocation = kinc_g5_compute_shader_get_constant_location(&kincImpl, name);
	return location;
}

Graphics5::TextureUnit Graphics5::ComputeShader::getTextureUnit(const char *name) {
	Graphics5::TextureUnit unit;
	unit.kincTextureUnit = kinc_g5_compute_shader_get_texture_unit(&kincImpl, name);
	return unit;
}
