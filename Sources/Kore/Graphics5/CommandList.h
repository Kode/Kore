#pragma once

#include <kinc/graphics5/commandlist.h>

#include "Graphics.h"

namespace Kore {
	namespace Graphics5 {
		class ComputeShader;
		class ConstantBuffer;
		class IndexBuffer;
		class PipelineState;
		class RenderTarget;
		class Sampler;
		class Texture;
		class VertexBuffer;
#ifdef KORE_OPENGL
		class ShaderStorageBuffer;
#endif

		class CommandList {
		public:
			CommandList();
			~CommandList();
			void begin();
			void end();
			void clear(RenderTarget *renderTarget, uint flags, uint color = 0, float depth = 1.0f, int stencil = 0);
			void renderTargetToFramebufferBarrier(RenderTarget *renderTarget);
			void framebufferToRenderTargetBarrier(RenderTarget *renderTarget);
			void textureToRenderTargetBarrier(RenderTarget *renderTarget);
			void renderTargetToTextureBarrier(RenderTarget *renderTarget);
			void drawIndexedVertices();
			void drawIndexedVertices(int start, int count);
			void drawIndexedVerticesInstanced(int instanceCount);
			void drawIndexedVerticesInstanced(int instanceCount, int start, int count);
			void viewport(int x, int y, int width, int height);
			void scissor(int x, int y, int width, int height);
			void disableScissor();
			void setPipeline(PipelineState *pipeline);
			void setVertexBuffers(VertexBuffer **buffers, int *offsets, int count);
			void setIndexBuffer(IndexBuffer &buffer);
			// void restoreRenderTarget();
			void setRenderTargets(RenderTarget **targets, int count);
			void upload(IndexBuffer *buffer);
			void upload(VertexBuffer *buffer);
			void upload(Texture *texture);
			void setVertexConstantBuffer(ConstantBuffer *buffer, int offset, size_t size);
			void setFragmentConstantBuffer(ConstantBuffer *buffer, int offset, size_t size);
			void execute();
			void waitForExecutionToFinish();

			void setTexture(TextureUnit unit, Texture *texture);
			void setImageTexture(TextureUnit unit, Texture *texture);
			void setRenderTargetFace(RenderTarget *texture, int face = 0);
			void setSampler(TextureUnit unit, Sampler *sampler);
			void setTextureFromRenderTarget(TextureUnit unit, RenderTarget *target);
			void setTextureFromRenderTargetDepth(TextureUnit unit, RenderTarget *target);

			void setComputeShader(ComputeShader *shader);
			void compute(int x, int y, int z);

			// Occlusion Query
			bool initOcclusionQuery(uint *occlusionQuery);
			void deleteOcclusionQuery(uint occlusionQuery);
			void renderOcclusionQuery(uint occlusionQuery, int triangles);
			bool isQueryResultsAvailable(uint occlusionQuery);
			void getQueryResults(uint occlusionQuery, uint *pixelCount);

			kinc_g5_command_list_t kincCommandList;
		};
	}
}
