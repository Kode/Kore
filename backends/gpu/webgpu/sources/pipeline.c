#include <kore3/webgpu/pipeline_functions.h>
#include <kore3/webgpu/pipeline_structs.h>

#include <kore3/log.h>

static WGPUVertexFormat convert_vertex_format(kore_webgpu_vertex_format format) {
	switch (format) {
	case KORE_WEBGPU_VERTEX_FORMAT_UINT8X2:
		return WGPUVertexFormat_Uint8x2;
	case KORE_WEBGPU_VERTEX_FORMAT_UINT8X4:
		return WGPUVertexFormat_Uint8x4;
	case KORE_WEBGPU_VERTEX_FORMAT_SINT8X2:
		return WGPUVertexFormat_Sint8x2;
	case KORE_WEBGPU_VERTEX_FORMAT_SINT8X4:
		return WGPUVertexFormat_Sint8x4;
	case KORE_WEBGPU_VERTEX_FORMAT_UNORM8X2:
		return WGPUVertexFormat_Unorm8x2;
	case KORE_WEBGPU_VERTEX_FORMAT_UNORM8X4:
		return WGPUVertexFormat_Unorm8x4;
	case KORE_WEBGPU_VERTEX_FORMAT_SNORM8X2:
		return WGPUVertexFormat_Snorm8x2;
	case KORE_WEBGPU_VERTEX_FORMAT_SNORM8X4:
		return WGPUVertexFormat_Snorm8x4;
	case KORE_WEBGPU_VERTEX_FORMAT_UINT16X2:
		return WGPUVertexFormat_Uint16x2;
	case KORE_WEBGPU_VERTEX_FORMAT_UINT16X4:
		return WGPUVertexFormat_Uint16x4;
	case KORE_WEBGPU_VERTEX_FORMAT_SINT16X2:
		return WGPUVertexFormat_Sint16x2;
	case KORE_WEBGPU_VERTEX_FORMAT_SINT16X4:
		return WGPUVertexFormat_Sint16x4;
	case KORE_WEBGPU_VERTEX_FORMAT_UNORM16X2:
		return WGPUVertexFormat_Unorm16x2;
	case KORE_WEBGPU_VERTEX_FORMAT_UNORM16X4:
		return WGPUVertexFormat_Unorm16x4;
	case KORE_WEBGPU_VERTEX_FORMAT_SNORM16X2:
		return WGPUVertexFormat_Snorm16x2;
	case KORE_WEBGPU_VERTEX_FORMAT_SNORM16X4:
		return WGPUVertexFormat_Snorm16x4;
	case KORE_WEBGPU_VERTEX_FORMAT_FLOAT16X2:
		return WGPUVertexFormat_Float16x2;
	case KORE_WEBGPU_VERTEX_FORMAT_FLOAT16X4:
		return WGPUVertexFormat_Float16x4;
	case KORE_WEBGPU_VERTEX_FORMAT_FLOAT32:
		return WGPUVertexFormat_Float32;
	case KORE_WEBGPU_VERTEX_FORMAT_FLOAT32X2:
		return WGPUVertexFormat_Float32x2;
	case KORE_WEBGPU_VERTEX_FORMAT_FLOAT32X3:
		return WGPUVertexFormat_Float32x3;
	case KORE_WEBGPU_VERTEX_FORMAT_FLOAT32X4:
		return WGPUVertexFormat_Float32x4;
	case KORE_WEBGPU_VERTEX_FORMAT_UINT32:
		return WGPUVertexFormat_Uint32;
	case KORE_WEBGPU_VERTEX_FORMAT_UINT32X2:
		return WGPUVertexFormat_Uint32x2;
	case KORE_WEBGPU_VERTEX_FORMAT_UINT32X3:
		return WGPUVertexFormat_Uint32x3;
	case KORE_WEBGPU_VERTEX_FORMAT_UINT32X4:
		return WGPUVertexFormat_Uint32x4;
	case KORE_WEBGPU_VERTEX_FORMAT_SIN32:
		return WGPUVertexFormat_Sint32;
	case KORE_WEBGPU_VERTEX_FORMAT_SINT32X2:
		return WGPUVertexFormat_Sint32x2;
	case KORE_WEBGPU_VERTEX_FORMAT_SINT32X3:
		return WGPUVertexFormat_Sint32x3;
	case KORE_WEBGPU_VERTEX_FORMAT_SINT32X4:
		return WGPUVertexFormat_Sint32x4;
	case KORE_WEBGPU_VERTEX_FORMAT_UNORM10_10_10_2:
		return WGPUVertexFormat_Unorm10_10_10_2;
	}

	assert(false);
	return WGPUVertexFormat_Float32;
}

void kore_webgpu_render_pipeline_init(kore_webgpu_device *device, kore_webgpu_render_pipeline *pipe, const kore_webgpu_render_pipeline_parameters *parameters, const WGPUBindGroupLayout *bind_group_layouts,
	uint32_t bind_group_layouts_count) {
	WGPUColorTargetState color_target_state = {
	    .format    = kore_webgpu_convert_texture_format(parameters->fragment.targets[0].format),
	    .writeMask = WGPUColorWriteMask_All,
	};

	WGPUBlendState blend_state = {
	    .color =
	        {
	            .operation = WGPUBlendOperation_Add,
	            .srcFactor = WGPUBlendFactor_One,
	            .dstFactor = WGPUBlendFactor_Zero,
	        },
	    .alpha =
	        {
	            .operation = WGPUBlendOperation_Add,
	            .srcFactor = WGPUBlendFactor_One,
	            .dstFactor = WGPUBlendFactor_Zero,
	        },
	};
	color_target_state.blend = &blend_state;

	WGPUPipelineLayoutDescriptor pipeline_layout_descriptor = {
	    .bindGroupLayoutCount = bind_group_layouts_count,
	    .bindGroupLayouts     = bind_group_layouts,
	};

	WGPUVertexAttribute attributes[8];
	for (size_t buffer_index = 0; buffer_index < parameters->vertex.buffers_count; ++buffer_index) {
		for (size_t attribute_index = 0; attribute_index < parameters->vertex.buffers[buffer_index].attributes_count; ++attribute_index) {
			WGPUVertexAttribute attribute = {
			    .shaderLocation = parameters->vertex.buffers[buffer_index].attributes[attribute_index].shader_location,
			    .offset         = parameters->vertex.buffers[buffer_index].attributes[attribute_index].offset,
			    .format         = convert_vertex_format(parameters->vertex.buffers[buffer_index].attributes[attribute_index].format),
			};
			attributes[attribute_index] = attribute;
		}
	}

	WGPUVertexBufferLayout vertex_buffer_layout = {
	    .arrayStride    = parameters->vertex.buffers[0].array_stride,
	    .attributeCount = parameters->vertex.buffers[0].attributes_count,
	    .attributes     = &attributes[0],
	};

	WGPUVertexState vertex_state = {
	    .module      = device->shader_module,
	    .entryPoint  = parameters->vertex.shader.function_name,
	    .bufferCount = 1,
	    .buffers     = &vertex_buffer_layout,
	};

	WGPUFragmentState fragment_state = {
	    .module      = device->shader_module,
	    .entryPoint  = parameters->fragment.shader.function_name,
	    .targetCount = 1,
	    .targets     = &color_target_state,
	};

	WGPUPrimitiveState primitive_state = {
	    .topology = WGPUPrimitiveTopology_TriangleList,
	    //.stripIndexFormat = WGPUIndexFormat_Uint32,
	    .frontFace = WGPUFrontFace_CW,
	    .cullMode  = WGPUCullMode_None,
	};

	WGPUMultisampleState multisample_state = {
	    .count                  = 1,
	    .mask                   = 0xffffffff,
	    .alphaToCoverageEnabled = false,
	};

	if (parameters->depth_stencil.format != KORE_GPU_TEXTURE_FORMAT_UNDEFINED) {
		WGPUDepthStencilState depth_stencil_state = {
			.format = kore_webgpu_convert_texture_format(parameters->depth_stencil.format),
			.depthWriteEnabled = parameters->depth_stencil.depth_write_enabled,
			.depthCompare = convert_compare(parameters->depth_stencil.depth_compare),
		};
	
		WGPURenderPipelineDescriptor render_pipeline_descriptor = {
			.layout       = wgpuDeviceCreatePipelineLayout(device->device, &pipeline_layout_descriptor),
			.fragment     = &fragment_state,
			.vertex       = vertex_state,
			.multisample  = multisample_state,
			.primitive    = primitive_state,
			.depthStencil = &depth_stencil_state,
		};

		pipe->render_pipeline = wgpuDeviceCreateRenderPipeline(device->device, &render_pipeline_descriptor);
	}
	else {
		WGPURenderPipelineDescriptor render_pipeline_descriptor = {
			.layout      = wgpuDeviceCreatePipelineLayout(device->device, &pipeline_layout_descriptor),
			.fragment    = &fragment_state,
			.vertex      = vertex_state,
			.multisample = multisample_state,
			.primitive   = primitive_state,
		};

		pipe->render_pipeline = wgpuDeviceCreateRenderPipeline(device->device, &render_pipeline_descriptor);
	}
}

void kore_webgpu_render_pipeline_destroy(kore_webgpu_render_pipeline *pipe) {}

void kore_webgpu_compute_pipeline_init(kore_webgpu_device *device, kore_webgpu_compute_pipeline *pipe,
                                       const kore_webgpu_compute_pipeline_parameters *parameters) {}

void kore_webgpu_compute_pipeline_destroy(kore_webgpu_compute_pipeline *pipe) {}

void kore_webgpu_ray_pipeline_init(kore_gpu_device *device, kore_webgpu_ray_pipeline *pipe, const kore_webgpu_ray_pipeline_parameters *parameters) {}

void kore_webgpu_ray_pipeline_destroy(kore_webgpu_ray_pipeline *pipe) {}
