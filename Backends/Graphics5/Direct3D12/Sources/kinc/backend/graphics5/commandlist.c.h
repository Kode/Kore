#include <kinc/graphics5/commandlist.h>
#include <kinc/graphics5/compute.h>
#include <kinc/graphics5/constantbuffer.h>
#include <kinc/graphics5/indexbuffer.h>
#include <kinc/graphics5/pipeline.h>
#include <kinc/graphics5/vertexbuffer.h>
#include <kinc/window.h>

void createHeaps(kinc_g5_command_list_t *list);

static int formatSize(DXGI_FORMAT format) {
	switch (format) {
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return 16;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return 8;
	case DXGI_FORMAT_R16_FLOAT:
		return 2;
	case DXGI_FORMAT_R8_UNORM:
		return 1;
	default:
		return 4;
	}
}

void kinc_g5_command_list_init(struct kinc_g5_command_list *list) {
#ifndef NDEBUG
	list->impl.open = false;
#endif

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_GRAPHICS_PPV_ARGS(&list->impl._commandAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, list->impl._commandAllocator, NULL, IID_GRAPHICS_PPV_ARGS(&list->impl._commandList));

	list->impl.fence_value = 0;
	list->impl.fence_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_GRAPHICS_PPV_ARGS(&list->impl.fence));

	list->impl._indexCount = 0;

	list->impl.current_full_scissor.left = -1;

	for (int i = 0; i < KINC_INTERNAL_G5_TEXTURE_COUNT; ++i) {
		list->impl.currentRenderTargets[i] = NULL;
		list->impl.currentTextures[i] = NULL;
		list->impl.current_samplers[i] = NULL;
	}
	list->impl.heapIndex = 0;
	createHeaps(list);
}

void kinc_g5_command_list_destroy(struct kinc_g5_command_list *list) {}

void kinc_g5_internal_reset_textures(struct kinc_g5_command_list *list);

void kinc_g5_command_list_begin(struct kinc_g5_command_list *list) {
	assert(!list->impl.open);

	compute_pipeline_set = false;

	if (list->impl.fence_value > 0) {
		waitForFence(list->impl.fence, list->impl.fence_value, list->impl.fence_event);
		list->impl._commandAllocator->Reset();
		list->impl._commandList->Reset(list->impl._commandAllocator, NULL);
	}

	kinc_g5_internal_reset_textures(list);

#ifndef NDEBUG
	list->impl.open = true;
#endif
}

void kinc_g5_command_list_end(struct kinc_g5_command_list *list) {
	assert(list->impl.open);

	list->impl._commandList->Close();

#ifndef NDEBUG
	list->impl.open = false;
#endif
}

void kinc_g5_command_list_clear(struct kinc_g5_command_list *list, kinc_g5_render_target_t *renderTarget, unsigned flags, unsigned color, float depth,
                                int stencil) {
	assert(list->impl.open);

	if (flags & KINC_G5_CLEAR_COLOR) {
		float clearColor[] = {((color & 0x00ff0000) >> 16) / 255.0f, ((color & 0x0000ff00) >> 8) / 255.0f, (color & 0x000000ff) / 255.0f,
		                      ((color & 0xff000000) >> 24) / 255.0f};
		list->impl._commandList->ClearRenderTargetView(renderTarget->impl.renderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), clearColor, 0,
		                                               NULL);
	}
	if ((flags & KINC_G5_CLEAR_DEPTH) || (flags & KINC_G5_CLEAR_STENCIL)) {
		D3D12_CLEAR_FLAGS d3dflags = (flags & KINC_G5_CLEAR_DEPTH) && (flags & KINC_G5_CLEAR_STENCIL) ? D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
		                             : (flags & KINC_G5_CLEAR_DEPTH)                                  ? D3D12_CLEAR_FLAG_DEPTH
		                                                                                              : D3D12_CLEAR_FLAG_STENCIL;

		if (renderTarget->impl.depthStencilDescriptorHeap != NULL) {
			list->impl._commandList->ClearDepthStencilView(renderTarget->impl.depthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), d3dflags, depth,
			                                               stencil, 0, NULL);
		}
	}
}

void kinc_g5_command_list_render_target_to_framebuffer_barrier(struct kinc_g5_command_list *list, kinc_g5_render_target_t *renderTarget) {
	assert(list->impl.open);

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Transition.pResource = renderTarget->impl.renderTarget;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	list->impl._commandList->ResourceBarrier(1, &barrier);
}

void kinc_g5_command_list_framebuffer_to_render_target_barrier(struct kinc_g5_command_list *list, kinc_g5_render_target_t *renderTarget) {
	assert(list->impl.open);

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Transition.pResource = renderTarget->impl.renderTarget;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	list->impl._commandList->ResourceBarrier(1, &barrier);
}

void kinc_g5_command_list_texture_to_render_target_barrier(struct kinc_g5_command_list *list, kinc_g5_render_target_t *renderTarget) {
	assert(list->impl.open);

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Transition.pResource = renderTarget->impl.renderTarget;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	list->impl._commandList->ResourceBarrier(1, &barrier);
}

void kinc_g5_command_list_render_target_to_texture_barrier(struct kinc_g5_command_list *list, kinc_g5_render_target_t *renderTarget) {
	assert(list->impl.open);

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Transition.pResource = renderTarget->impl.renderTarget;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	list->impl._commandList->ResourceBarrier(1, &barrier);
}

void kinc_g5_command_list_set_vertex_constant_buffer(struct kinc_g5_command_list *list, kinc_g5_constant_buffer_t *buffer, int offset, size_t size) {
	assert(list->impl.open);

#ifdef KINC_DXC
	if (list->impl._currentPipeline->impl.vertexConstantsSize > 0) {
		if (list->impl._currentPipeline->impl.textures > 0) {
			list->impl._commandList->SetGraphicsRootConstantBufferView(2, buffer->impl.constant_buffer->GetGPUVirtualAddress() + offset);
		}
		else {
			list->impl._commandList->SetGraphicsRootConstantBufferView(0, buffer->impl.constant_buffer->GetGPUVirtualAddress() + offset);
		}
	}
#else
	list->impl._commandList->SetGraphicsRootConstantBufferView(2, buffer->impl.constant_buffer->GetGPUVirtualAddress() + offset);
#endif
}

void kinc_g5_command_list_set_fragment_constant_buffer(struct kinc_g5_command_list *list, kinc_g5_constant_buffer_t *buffer, int offset, size_t size) {
	assert(list->impl.open);
#ifdef KINC_DXC
	if (list->impl._currentPipeline->impl.fragmentConstantsSize > 0) {
		list->impl._commandList->SetGraphicsRootConstantBufferView(3, buffer->impl.constant_buffer->GetGPUVirtualAddress() + offset);
	}
#else
	list->impl._commandList->SetGraphicsRootConstantBufferView(3, buffer->impl.constant_buffer->GetGPUVirtualAddress() + offset);
#endif
}

void kinc_g5_command_list_set_compute_constant_buffer(struct kinc_g5_command_list *list, kinc_g5_constant_buffer_t *buffer, int offset, size_t size) {
	assert(list->impl.open);

#ifdef KINC_DXC
	if (list->impl._currentPipeline->impl.fragmentConstantsSize > 0) {
		list->impl._commandList->SetGraphicsRootConstantBufferView(3, buffer->impl.constant_buffer->GetGPUVirtualAddress() + offset);
	}
#else
	list->impl._commandList->SetComputeRootConstantBufferView(3, buffer->impl.constant_buffer->GetGPUVirtualAddress() + offset);
#endif
}

void kinc_g5_command_list_draw_indexed_vertices(struct kinc_g5_command_list *list) {
	kinc_g5_command_list_draw_indexed_vertices_from_to(list, 0, list->impl._indexCount);
}

void kinc_g5_command_list_draw_indexed_vertices_from_to(struct kinc_g5_command_list *list, int start, int count) {
	assert(list->impl.open);

	list->impl._commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*u8* data;
	D3D12_RANGE range;
	range.Begin = currentConstantBuffer * sizeof(vertexConstants);
	range.End = range.Begin + sizeof(vertexConstants);
	vertexConstantBuffer->Map(0, &range, (void**)&data);
	memcpy(data + currentConstantBuffer * sizeof(vertexConstants), vertexConstants, sizeof(vertexConstants));
	vertexConstantBuffer->Unmap(0, &range);

	range.Begin = currentConstantBuffer * sizeof(fragmentConstants);
	range.End = range.Begin + sizeof(fragmentConstants);
	fragmentConstantBuffer->Map(0, &range, (void**)&data);
	memcpy(data + currentConstantBuffer * sizeof(fragmentConstants), fragmentConstants, sizeof(fragmentConstants));
	fragmentConstantBuffer->Unmap(0, &range);

	_commandList->SetGraphicsRootConstantBufferView(1, vertexConstantBuffer->GetGPUVirtualAddress() + currentConstantBuffer * sizeof(vertexConstants));
	_commandList->SetGraphicsRootConstantBufferView(2, fragmentConstantBuffer->GetGPUVirtualAddress() + currentConstantBuffer * sizeof(fragmentConstants));

	++currentConstantBuffer;
	if (currentConstantBuffer >= constantBufferMultiply) {
	    currentConstantBuffer = 0;
	}*/

	list->impl._commandList->DrawIndexedInstanced(count, 1, start, 0, 0);
}

void kinc_g5_command_list_draw_indexed_vertices_from_to_from(struct kinc_g5_command_list *list, int start, int count, int vertex_offset) {
	assert(list->impl.open);

	list->impl._commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	list->impl._commandList->DrawIndexedInstanced(count, 1, start, vertex_offset, 0);
}

void kinc_g5_command_list_draw_indexed_vertices_instanced(kinc_g5_command_list_t *list, int instanceCount) {
	kinc_g5_command_list_draw_indexed_vertices_instanced_from_to(list, instanceCount, 0, list->impl._indexCount);
}
void kinc_g5_command_list_draw_indexed_vertices_instanced_from_to(kinc_g5_command_list_t *list, int instanceCount, int start, int count) {
	assert(list->impl.open);

	list->impl._commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	list->impl._commandList->DrawIndexedInstanced(count, instanceCount, start, 0, 0);
}

void kinc_g5_command_list_execute(kinc_g5_command_list_t *list) {
	assert(!list->impl.open);

	ID3D12CommandList *commandLists[] = {(ID3D12CommandList *)list->impl._commandList};
	commandQueue->ExecuteCommandLists(1, commandLists);

	commandQueue->Signal(list->impl.fence, ++list->impl.fence_value);
}

void kinc_g5_command_list_wait_for_execution_to_finish(kinc_g5_command_list_t *list) {
	waitForFence(list->impl.fence, list->impl.fence_value, list->impl.fence_event);
}

bool kinc_g5_non_pow2_textures_supported(void) {
	return true;
}

void kinc_g5_command_list_viewport(struct kinc_g5_command_list *list, int x, int y, int width, int height) {
	assert(list->impl.open);

	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = (float)x;
	viewport.TopLeftY = (float)y;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	list->impl._commandList->RSSetViewports(1, &viewport);
}

void kinc_g5_command_list_scissor(struct kinc_g5_command_list *list, int x, int y, int width, int height) {
	assert(list->impl.open);

	D3D12_RECT scissor;
	scissor.left = x;
	scissor.top = y;
	scissor.right = x + width;
	scissor.bottom = y + height;
	list->impl._commandList->RSSetScissorRects(1, &scissor);
}

void kinc_g5_command_list_disable_scissor(struct kinc_g5_command_list *list) {
	assert(list->impl.open);

	if (list->impl.current_full_scissor.left >= 0) {
		list->impl._commandList->RSSetScissorRects(1, (D3D12_RECT *)&list->impl.current_full_scissor);
	}
	else {
		D3D12_RECT scissor;
		scissor.left = 0;
		scissor.top = 0;
		scissor.right = kinc_window_width(0);
		scissor.bottom = kinc_window_height(0);
		list->impl._commandList->RSSetScissorRects(1, &scissor);
	}
}

void kinc_g5_command_list_set_pipeline(struct kinc_g5_command_list *list, kinc_g5_pipeline_t *pipeline) {
	assert(list->impl.open);

	list->impl._currentPipeline = pipeline;
	list->impl._commandList->SetPipelineState(pipeline->impl.pso);
	compute_pipeline_set = false;

	kinc_g5_internal_setConstants(list, list->impl._currentPipeline);
}

void kinc_g5_command_list_set_blend_constant(kinc_g5_command_list_t *list, float r, float g, float b, float a) {
	const FLOAT BlendFactor[4] = {r, g, b, a};
	list->impl._commandList->OMSetBlendFactor(BlendFactor);
}

void kinc_g5_command_list_set_vertex_buffers(struct kinc_g5_command_list *list, kinc_g5_vertex_buffer_t **buffers, int *offsets, int count) {
	assert(list->impl.open);

	D3D12_VERTEX_BUFFER_VIEW *views = (D3D12_VERTEX_BUFFER_VIEW *)alloca(sizeof(D3D12_VERTEX_BUFFER_VIEW) * count);
	ZeroMemory(views, sizeof(D3D12_VERTEX_BUFFER_VIEW) * count);
	for (int i = 0; i < count; ++i) {
		views[i].BufferLocation = buffers[i]->impl.uploadBuffer->GetGPUVirtualAddress() + offsets[i] * kinc_g5_vertex_buffer_stride(buffers[i]);
		views[i].SizeInBytes = (kinc_g5_vertex_buffer_count(buffers[i]) - offsets[i]) * kinc_g5_vertex_buffer_stride(buffers[i]);
		views[i].StrideInBytes = kinc_g5_vertex_buffer_stride(buffers[i]); // * kinc_g5_vertex_buffer_count(buffers[i]);
	}
	list->impl._commandList->IASetVertexBuffers(0, count, views);
}

void kinc_g5_command_list_set_index_buffer(struct kinc_g5_command_list *list, kinc_g5_index_buffer_t *buffer) {
	assert(list->impl.open);

	list->impl._indexCount = kinc_g5_index_buffer_count(buffer);
	list->impl._commandList->IASetIndexBuffer((D3D12_INDEX_BUFFER_VIEW *)&buffer->impl.index_buffer_view);
}

void kinc_g5_command_list_set_render_targets(struct kinc_g5_command_list *list, kinc_g5_render_target_t **targets, int count) {
	assert(list->impl.open);

	kinc_g5_render_target_t *render_target = targets[0];

	D3D12_CPU_DESCRIPTOR_HANDLE target_descriptors[16];
	for (int i = 0; i < count; ++i) {
		target_descriptors[i] = targets[i]->impl.renderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}

	assert(render_target != NULL);

	if (render_target->impl.depthStencilDescriptorHeap != NULL) {
		D3D12_CPU_DESCRIPTOR_HANDLE heapStart = render_target->impl.depthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		list->impl._commandList->OMSetRenderTargets(count, &target_descriptors[0], false, &heapStart);
	}
	else {
		list->impl._commandList->OMSetRenderTargets(count, &target_descriptors[0], false, NULL);
	}

	list->impl._commandList->RSSetViewports(1, (D3D12_VIEWPORT *)&render_target->impl.viewport);
	list->impl._commandList->RSSetScissorRects(1, (D3D12_RECT *)&render_target->impl.scissor);

	list->impl.current_full_scissor = render_target->impl.scissor;
}

void kinc_g5_command_list_upload_vertex_buffer(kinc_g5_command_list_t *list, struct kinc_g5_vertex_buffer *buffer) {}

void kinc_g5_command_list_upload_index_buffer(kinc_g5_command_list_t *list, kinc_g5_index_buffer_t *buffer) {
	assert(list->impl.open);
	kinc_g5_internal_index_buffer_upload(buffer, list->impl._commandList);
}

void kinc_g5_command_list_upload_texture(kinc_g5_command_list_t *list, kinc_g5_texture_t *texture) {
	assert(list->impl.open);

	{
		D3D12_RESOURCE_BARRIER transition = {};
		transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		transition.Transition.pResource = texture->impl.image;
		transition.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		transition.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		transition.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		list->impl._commandList->ResourceBarrier(1, &transition);
	}

	D3D12_RESOURCE_DESC Desc = texture->impl.image->GetDesc();
	ID3D12Device *device = NULL;
	texture->impl.image->GetDevice(IID_GRAPHICS_PPV_ARGS(&device));
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	device->GetCopyableFootprints(&Desc, 0, 1, 0, &footprint, NULL, NULL, NULL);
	device->Release();

	D3D12_TEXTURE_COPY_LOCATION source = {0};
	source.pResource = texture->impl.uploadImage;
	source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	source.PlacedFootprint = footprint;

	D3D12_TEXTURE_COPY_LOCATION destination = {0};
	destination.pResource = texture->impl.image;
	destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destination.SubresourceIndex = 0;

	list->impl._commandList->CopyTextureRegion(&destination, 0, 0, 0, &source, NULL);

	{
		D3D12_RESOURCE_BARRIER transition = {};
		transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		transition.Transition.pResource = texture->impl.image;
		transition.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		transition.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		transition.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		list->impl._commandList->ResourceBarrier(1, &transition);
	}
}

#if defined(KINC_WINDOWS) || defined(KINC_WINDOWSAPP)
static int d3d12_textureAlignment() {
	return D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
}
#else
extern "C" int d3d12_textureAlignment();
#endif

void kinc_g5_command_list_get_render_target_pixels(kinc_g5_command_list_t *list, kinc_g5_render_target_t *render_target, uint8_t *data) {
	assert(list->impl.open);

	DXGI_FORMAT dxgiFormat = render_target->impl.renderTarget->GetDesc().Format;
	int formatByteSize = formatSize(dxgiFormat);
	int rowPitch = render_target->texWidth * formatByteSize;
	int align = rowPitch % d3d12_textureAlignment();
	if (align != 0)
		rowPitch = rowPitch + (d3d12_textureAlignment() - align);

	// Create readback buffer
	if (render_target->impl.renderTargetReadback == NULL) {
		D3D12_HEAP_PROPERTIES heapProperties;
		heapProperties.Type = D3D12_HEAP_TYPE_READBACK;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = rowPitch * render_target->texHeight;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL,
		                                IID_GRAPHICS_PPV_ARGS(&render_target->impl.renderTargetReadback));
	}

	// Copy render target to readback buffer
	D3D12_RESOURCE_STATES sourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	{
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource = render_target->impl.renderTarget;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = sourceState;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		list->impl._commandList->ResourceBarrier(1, &barrier);
	}

	D3D12_TEXTURE_COPY_LOCATION source;
	source.pResource = render_target->impl.renderTarget;
	source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	source.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION dest;
	dest.pResource = render_target->impl.renderTargetReadback;
	dest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dest.PlacedFootprint.Offset = 0;
	dest.PlacedFootprint.Footprint.Format = dxgiFormat;
	dest.PlacedFootprint.Footprint.Width = render_target->texWidth;
	dest.PlacedFootprint.Footprint.Height = render_target->texHeight;
	dest.PlacedFootprint.Footprint.Depth = 1;
	dest.PlacedFootprint.Footprint.RowPitch = rowPitch;

	list->impl._commandList->CopyTextureRegion(&dest, 0, 0, 0, &source, NULL);

	{
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource = render_target->impl.renderTarget;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.StateAfter = sourceState;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		list->impl._commandList->ResourceBarrier(1, &barrier);
	}

	kinc_g5_command_list_end(list);
	kinc_g5_command_list_execute(list);
	kinc_g5_command_list_wait_for_execution_to_finish(list);
	kinc_g5_command_list_begin(list);

	// Read buffer
	void *p;
	render_target->impl.renderTargetReadback->Map(0, NULL, &p);
	memcpy(data, p, render_target->texWidth * render_target->texHeight * formatByteSize);
	render_target->impl.renderTargetReadback->Unmap(0, NULL);
}

void kinc_g5_internal_set_compute_constants(kinc_g5_command_list_t *commandList);

void kinc_g5_command_list_set_compute_shader(kinc_g5_command_list_t *list, kinc_g5_compute_shader *shader) {
	list->impl._commandList->SetPipelineState(shader->impl.pso);
	compute_pipeline_set = true;

	for (int i = 0; i < KINC_INTERNAL_G5_TEXTURE_COUNT; ++i) {
		list->impl.currentRenderTargets[i] = NULL;
		list->impl.currentTextures[i] = NULL;
	}
	kinc_g5_internal_set_compute_constants(list);
}

void kinc_g5_command_list_compute(kinc_g5_command_list_t *list, int x, int y, int z) {
	assert(list->impl.open);
	list->impl._commandList->Dispatch(x, y, z);
}

void kinc_g5_command_list_set_render_target_face(kinc_g5_command_list_t *list, kinc_g5_render_target_t *texture, int face) {}

void kinc_g5_command_list_set_texture(kinc_g5_command_list_t *list, kinc_g5_texture_unit_t unit, kinc_g5_texture_t *texture) {
	if (unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT] >= 0) {
		kinc_g5_internal_texture_set(list, texture, unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT]);
	}
	else if (unit.stages[KINC_G5_SHADER_TYPE_VERTEX] >= 0) {
		kinc_g5_internal_texture_set(list, texture, unit.stages[KINC_G5_SHADER_TYPE_VERTEX]);
	}
	else if (unit.stages[KINC_G5_SHADER_TYPE_COMPUTE] >= 0) {
		kinc_g5_internal_texture_set(list, texture, unit.stages[KINC_G5_SHADER_TYPE_COMPUTE]);
	}
	kinc_g5_internal_set_textures(list);
}

void kinc_g5_internal_sampler_set(kinc_g5_command_list_t *list, kinc_g5_sampler_t *sampler, int unit);

void kinc_g5_command_list_set_sampler(kinc_g5_command_list_t *list, kinc_g5_texture_unit_t unit, kinc_g5_sampler_t *sampler) {
	if (unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT] >= 0) {
		kinc_g5_internal_sampler_set(list, sampler, unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT]);
	}
	else if (unit.stages[KINC_G5_SHADER_TYPE_VERTEX] >= 0) {
		kinc_g5_internal_sampler_set(list, sampler, unit.stages[KINC_G5_SHADER_TYPE_VERTEX]);
	}
	kinc_g5_internal_set_textures(list);
}

bool kinc_g5_command_list_init_occlusion_query(kinc_g5_command_list_t *list, unsigned *occlusionQuery) {
	return false;
}

void kinc_g5_command_list_set_image_texture(kinc_g5_command_list_t *list, kinc_g5_texture_unit_t unit, kinc_g5_texture_t *texture) {
	if (unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT] >= 0) {
		kinc_g5_internal_texture_set(list, texture, unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT]);
	}
	else if (unit.stages[KINC_G5_SHADER_TYPE_VERTEX] >= 0) {
		kinc_g5_internal_texture_set(list, texture, unit.stages[KINC_G5_SHADER_TYPE_VERTEX]);
	}
	else if (unit.stages[KINC_G5_SHADER_TYPE_COMPUTE] >= 0) {
		kinc_g5_internal_texture_set(list, texture, unit.stages[KINC_G5_SHADER_TYPE_COMPUTE]);
	}
	kinc_g5_internal_set_textures(list);
}

void kinc_g5_command_list_delete_occlusion_query(kinc_g5_command_list_t *list, unsigned occlusionQuery) {}

void kinc_g5_command_list_render_occlusion_query(kinc_g5_command_list_t *list, unsigned occlusionQuery, int triangles) {}

bool kinc_g5_command_list_are_query_results_available(kinc_g5_command_list_t *list, unsigned occlusionQuery) {
	return false;
}

void kinc_g5_command_list_get_query_result(kinc_g5_command_list_t *list, unsigned occlusionQuery, unsigned *pixelCount) {}

void kinc_g5_command_list_set_texture_from_render_target(kinc_g5_command_list_t *list, kinc_g5_texture_unit_t unit, kinc_g5_render_target_t *target) {
	if (unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT] >= 0) {
		target->impl.stage = unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT];
		list->impl.currentRenderTargets[target->impl.stage] = target;
	}
	else if (unit.stages[KINC_G5_SHADER_TYPE_VERTEX] >= 0) {
		target->impl.stage = unit.stages[KINC_G5_SHADER_TYPE_VERTEX];
		list->impl.currentRenderTargets[target->impl.stage] = target;
	}
	list->impl.currentTextures[target->impl.stage] = NULL;

	kinc_g5_internal_set_textures(list);
}

void kinc_g5_command_list_set_texture_from_render_target_depth(kinc_g5_command_list_t *list, kinc_g5_texture_unit_t unit, kinc_g5_render_target_t *target) {
	if (unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT] >= 0) {
		target->impl.stage_depth = unit.stages[KINC_G5_SHADER_TYPE_FRAGMENT];
		list->impl.currentRenderTargets[target->impl.stage_depth] = target;
	}
	else if (unit.stages[KINC_G5_SHADER_TYPE_VERTEX] >= 0) {
		target->impl.stage_depth = unit.stages[KINC_G5_SHADER_TYPE_VERTEX];
		list->impl.currentRenderTargets[target->impl.stage_depth] = target;
	}
	list->impl.currentTextures[target->impl.stage_depth] = NULL;

	kinc_g5_internal_set_textures(list);
}
