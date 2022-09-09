#pragma once

#include <Kore/Graphics1/Image.h>

#include "Shader.h"
#include "Texture.h"
#include "VertexStructure.h"

#include <Kore/Math/Matrix.h>
#include <Kore/Math/Vector.h>

#include <kinc/backend/graphics5/graphics.h>

#include <kinc/graphics5/indexbuffer.h>
#include <kinc/graphics5/vertexbuffer.h>

namespace Kore {
	namespace Graphics5 {
		typedef Graphics1::Image Image;

		class PipelineState;

		class VertexBuffer {
		public:
			VertexBuffer(int count, const VertexStructure &structure, bool gpuMemory, int instanceDataStepRate = 0);
			virtual ~VertexBuffer();
			float *lock();
			float *lock(int start, int count);
			void unlock();
			void unlock(int count);
			int count();
			int stride();
			// int _set(int offset = 0); // Do not call this directly, use Graphics::setVertexBuffers

			kinc_g5_vertex_buffer_t kincBuffer;
		};

		class IndexBuffer {
		public:
			IndexBuffer(int count, bool gpuMemory);
			virtual ~IndexBuffer();
			int *lock();
			void unlock();
			int count();
			// void _set();

			kinc_g5_index_buffer_t kincBuffer;
		};

		enum TextureAddressing { Repeat, Mirror, Clamp, Border };

		enum TextureFilter { PointFilter, LinearFilter, AnisotropicFilter };

		enum MipmapFilter {
			NoMipFilter,
			PointMipFilter,
			LinearMipFilter // linear texture filter + linear mip filter -> trilinear filter
		};

		enum RenderState {
			BlendingState,
			DepthTest,
			DepthTestCompare,
			/*Lighting,*/ DepthWrite,
			Normalize,
			BackfaceCulling,
			/*FogState, FogStartState, FogEndState, FogTypeState, FogColorState,*/ ScissorTestState,
			AlphaTestState,
			AlphaReferenceState
		};

		enum BlendingFactor {
			BlendOne,
			BlendZero,
			SourceAlpha,
			DestinationAlpha,
			InverseSourceAlpha,
			InverseDestinationAlpha,
			SourceColor,
			DestinationColor,
			InverseSourceColor,
			InverseDestinationColor
		};

		enum BlendingOperation { BlendOpAdd, BlendOpSubtract, BlendOpReverseSubtract, BlendOpMin, BlendOpMax };

		enum ZCompareMode {
			ZCompareAlways,
			ZCompareNever,
			ZCompareEqual,
			ZCompareNotEqual,
			ZCompareLess,
			ZCompareLessEqual,
			ZCompareGreater,
			ZCompareGreaterEqual
		};

		enum CullMode { Clockwise, CounterClockwise, NoCulling };

		enum TexDir { U, V, W };

		enum FogType { LinearFog };

		enum RenderTargetFormat { Target32Bit, Target64BitFloat, Target32BitRedFloat, Target128BitFloat, Target16BitDepth, Target8BitRed };

		enum StencilAction { Keep, Zero, Replace, Increment, IncrementWrap, Decrement, DecrementWrap, Invert };

		enum TextureOperation { ModulateOperation, SelectFirstOperation, SelectSecondOperation };

		enum TextureArgument { CurrentColorArgument, TextureColorArgument };

		class RenderTarget {
		public:
			RenderTarget(int width, int height, RenderTargetFormat format = Target32Bit, int depthBufferBits = 0, int stencilBufferBits = 0,
			             int samplesPerPixel = 1);
			RenderTarget(int cubeMapSize, RenderTargetFormat format = Target32Bit, int depthBufferBits = 0, int stencilBufferBits = 0, int samplesPerPixel = 1);
			~RenderTarget();
			int width;
			int height;
			int texWidth;
			int texHeight;
			int contextId;
			bool isCubeMap;
			bool isDepthAttachment;
			void useColorAsTexture(TextureUnit unit);
			void useDepthAsTexture(TextureUnit unit);
			void setDepthStencilFrom(RenderTarget *source);

			kinc_g5_render_target_t kincRenderTarget;
		};

		int antialiasingSamples();
		void setAntialiasingSamples(int samples);

		bool renderTargetsInvertedY();

		void begin(RenderTarget *renderTarget, int window = 0);
		void end(int window = 0);
		bool swapBuffers();

		// void _resize(int window, int width, int height);

		int maxBoundTextures();

		bool nonPow2TexturesSupported();

		const uint ClearColorFlag = 1;
		const uint ClearDepthFlag = 2;
		const uint ClearStencilFlag = 4;

		extern bool fullscreen;

		void flush();
	}
}
