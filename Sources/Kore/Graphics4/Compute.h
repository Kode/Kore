#pragma once

#include <kinc/graphics4/compute.h>

#ifdef KORE_OPENGL
#include <kinc/backend/graphics4/ShaderStorageBufferImpl.h>
#endif
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Math/Matrix.h>

namespace Kore {
	namespace Graphics4 {
		class Texture;
		class RenderTarget;

		class ComputeShader {
		public:
			ComputeShader(void *source, int length);
			~ComputeShader();
			ConstantLocation getConstantLocation(const char *name);
			TextureUnit getTextureUnit(const char *name);
			kinc_g4_compute_shader kincImpl;
		};

#ifdef KORE_OPENGL
		class ShaderStorageBuffer {
		public:
			ShaderStorageBuffer(int count, Graphics4::VertexData type);
			virtual ~ShaderStorageBuffer();
			int *lock();
			void unlock();
			int count();
			void _set();
			kinc_shader_storage_buffer_t kincImpl;
		};
#endif
	}
}
