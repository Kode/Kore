#pragma once

#include <kinc/graphics5/compute.h>

#ifdef KORE_OPENGL
#include <kinc/backend/graphics4/ShaderStorageBufferImpl.h>
#endif
#include <Kore/Graphics5/Graphics.h>
#include <Kore/Math/Matrix.h>

namespace Kore {
	namespace Graphics5 {
		class ComputeShader {
		public:
			ComputeShader(void *source, int length);
			~ComputeShader();
			ConstantLocation getConstantLocation(const char *name);
			TextureUnit getTextureUnit(const char *name);
			kinc_g5_compute_shader kincImpl;
		};
	}
}
