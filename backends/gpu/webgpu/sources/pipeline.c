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

static void compilation_info_callback(WGPUCompilationInfoRequestStatus status, const WGPUCompilationInfo *info, void *userdata) {
	assert(status == WGPUCompilationInfoRequestStatus_Success);
	assert(info->messageCount == 0);
	kore_log(KORE_LOG_LEVEL_INFO, "Shader compile succeeded");
}

static WGPUBlendFactor convert_blend_factor(kore_webgpu_blend_factor factor) {
	switch (factor) {
	case KORE_WEBGPU_BLEND_FACTOR_ZERO:
		return WGPUBlendFactor_Zero;
	case KORE_WEBGPU_BLEND_FACTOR_ONE:
		return WGPUBlendFactor_One;
	case KORE_WEBGPU_BLEND_FACTOR_SRC:
		return WGPUBlendFactor_Src;
	case KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_SRC:
		return WGPUBlendFactor_OneMinusSrc;
	case KORE_WEBGPU_BLEND_FACTOR_SRC_ALPHA:
		return WGPUBlendFactor_SrcAlpha;
	case KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
		return WGPUBlendFactor_OneMinusSrcAlpha;
	case KORE_WEBGPU_BLEND_FACTOR_DST:
		return WGPUBlendFactor_Dst;
	case KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_DST:
		return WGPUBlendFactor_OneMinusDst;
	case KORE_WEBGPU_BLEND_FACTOR_DST_ALPHA:
		return WGPUBlendFactor_DstAlpha;
	case KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		return WGPUBlendFactor_OneMinusDstAlpha;
	case KORE_WEBGPU_BLEND_FACTOR_SRC_ALPHA_SATURATED:
		return WGPUBlendFactor_SrcAlphaSaturated;
	case KORE_WEBGPU_BLEND_FACTOR_CONSTANT:
		return WGPUBlendFactor_Constant;
	case KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_CONSTANT:
		return WGPUBlendFactor_OneMinusConstant;
	}

	assert(false);
	return WGPUBlendFactor_One;
}

static WGPUBlendOperation convert_blend_operation(kore_webgpu_blend_operation op) {
	switch (op) {
	case KORE_WEBGPU_BLEND_OPERATION_ADD:
		return WGPUBlendOperation_Add;
	case KORE_WEBGPU_BLEND_OPERATION_SUBTRACT:
		return WGPUBlendOperation_Subtract;
	case KORE_WEBGPU_BLEND_OPERATION_REVERSE_SUBTRACT:
		return WGPUBlendOperation_ReverseSubtract;
	case KORE_WEBGPU_BLEND_OPERATION_MIN:
		return WGPUBlendOperation_Min;
	case KORE_WEBGPU_BLEND_OPERATION_MAX:
		return WGPUBlendOperation_Max;
	}

	assert(false);
	return WGPUBlendOperation_Add;
}

const char *kore_webgpu_prepare_shader(kore_gpu_device *device, const char *code, size_t size, bool uses_framebuffer_texture_format) {
	if (!uses_framebuffer_texture_format) {
		return code;
	}

	char *new_code = malloc(size + 1);
	memcpy(new_code, code, size + 1);

	char format[64];
	switch (kore_webgpu_device_framebuffer_format(device)) {
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
		strcpy(format, "rgba8unorm");
		break;
	default:
		strcpy(format, "error_format");
		break;
	}

	for (size_t index = 0; index < size; ++index) {
		if (code[index] == '$') {
			size_t format_index = 0;
			while (format[format_index] != 0) {
				new_code[index] = format[format_index];
				++index;
				++format_index;
			}
		}
	}

	return new_code;
}

void kore_webgpu_render_pipeline_init(kore_webgpu_device *device, kore_webgpu_render_pipeline *pipe, const kore_webgpu_render_pipeline_parameters *parameters,
                                      const WGPUBindGroupLayout *bind_group_layouts, uint32_t bind_group_layouts_count) {
	WGPUShaderModuleWGSLDescriptor vertex_shader_module_wgsl_descriptor = {
	    .code        = parameters->vertex.shader.data,
	    .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
	};

	WGPUShaderModuleDescriptor vertex_shader_module_descriptor = {
	    .nextInChain = (WGPUChainedStruct *)&vertex_shader_module_wgsl_descriptor,
	};

	WGPUShaderModule vertex_shader_module = wgpuDeviceCreateShaderModule(device->device, &vertex_shader_module_descriptor);
	wgpuShaderModuleGetCompilationInfo(vertex_shader_module, compilation_info_callback, NULL);

	WGPUShaderModuleWGSLDescriptor fragment_shader_module_wgsl_descriptor = {
	    .code        = parameters->fragment.shader.data,
	    .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
	};

	WGPUShaderModuleDescriptor fragment_shader_module_descriptor = {
	    .nextInChain = (WGPUChainedStruct *)&fragment_shader_module_wgsl_descriptor,
	};

	WGPUShaderModule fragment_shader_module = wgpuDeviceCreateShaderModule(device->device, &fragment_shader_module_descriptor);
	wgpuShaderModuleGetCompilationInfo(fragment_shader_module, compilation_info_callback, NULL);

	WGPUColorTargetState color_target_states[8];

	WGPUBlendState blend_states[8];

	for (uint32_t target_index = 0; target_index < parameters->fragment.targets_count; ++target_index) {
		color_target_states[target_index] = (WGPUColorTargetState){
		    .format    = kore_webgpu_convert_texture_format_to_webgpu(parameters->fragment.targets[target_index].format),
		    .writeMask = WGPUColorWriteMask_All,
		};

		const kore_webgpu_blend_state *blend = &parameters->fragment.targets[target_index].blend;

		if (blend->color.src_factor == KORE_WEBGPU_BLEND_FACTOR_ONE && blend->color.dst_factor == KORE_WEBGPU_BLEND_FACTOR_ZERO &&
		    blend->color.operation == KORE_WEBGPU_BLEND_OPERATION_ADD && blend->alpha.src_factor == KORE_WEBGPU_BLEND_FACTOR_ONE &&
		    blend->alpha.dst_factor == KORE_WEBGPU_BLEND_FACTOR_ZERO && blend->alpha.operation == KORE_WEBGPU_BLEND_OPERATION_ADD) {
			// disabled
		}
		else {
			blend_states[target_index] = (WGPUBlendState){
			    .color =
			        {
			            .operation = convert_blend_operation(blend->color.operation),
			            .srcFactor = convert_blend_factor(blend->color.src_factor),
			            .dstFactor = convert_blend_factor(blend->color.dst_factor),
			        },
			    .alpha =
			        {
			            .operation = convert_blend_operation(blend->alpha.operation),
			            .srcFactor = convert_blend_factor(blend->alpha.src_factor),
			            .dstFactor = convert_blend_factor(blend->alpha.dst_factor),
			        },
			};

			color_target_states[target_index].blend = &blend_states[target_index];
		}
	}

	WGPUPipelineLayoutDescriptor pipeline_layout_descriptor = {
	    .bindGroupLayoutCount = bind_group_layouts_count,
	    .bindGroupLayouts     = bind_group_layouts,
	};

	WGPUVertexAttribute    attributes[8];
	WGPUVertexBufferLayout vertex_buffer_layouts[8];

	size_t attribute_offset = 0;

	for (size_t buffer_index = 0; buffer_index < parameters->vertex.buffers_count; ++buffer_index) {
		WGPUVertexBufferLayout vertex_buffer_layout = {
		    .arrayStride    = parameters->vertex.buffers[buffer_index].array_stride,
		    .attributeCount = parameters->vertex.buffers[buffer_index].attributes_count,
		    .attributes     = &attributes[attribute_offset],
		    .stepMode       = parameters->vertex.buffers[buffer_index].step_mode == KORE_WEBGPU_VERTEX_STEP_MODE_VERTEX ? WGPUVertexStepMode_Vertex
		                                                                                                                : WGPUVertexStepMode_Instance,
		};

		for (size_t attribute_index = 0; attribute_index < parameters->vertex.buffers[buffer_index].attributes_count; ++attribute_index) {
			WGPUVertexAttribute attribute = {
			    .shaderLocation = parameters->vertex.buffers[buffer_index].attributes[attribute_index].shader_location,
			    .offset         = parameters->vertex.buffers[buffer_index].attributes[attribute_index].offset,
			    .format         = convert_vertex_format(parameters->vertex.buffers[buffer_index].attributes[attribute_index].format),
			};
			attributes[attribute_offset] = attribute;
			++attribute_offset;
		}

		vertex_buffer_layouts[buffer_index] = vertex_buffer_layout;
	}

	WGPUVertexState vertex_state = {
	    .module      = vertex_shader_module,
	    .entryPoint  = "main",
	    .bufferCount = parameters->vertex.buffers_count,
	    .buffers     = &vertex_buffer_layouts[0],
	};

	WGPUFragmentState fragment_state = {
	    .module      = fragment_shader_module,
	    .entryPoint  = "main",
	    .targetCount = parameters->fragment.targets_count,
	    .targets     = color_target_states,
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
		    .format            = kore_webgpu_convert_texture_format_to_webgpu(parameters->depth_stencil.format),
		    .depthWriteEnabled = parameters->depth_stencil.depth_write_enabled,
		    .depthCompare      = convert_compare(parameters->depth_stencil.depth_compare),
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
                                       const kore_webgpu_compute_pipeline_parameters *parameters, const WGPUBindGroupLayout *bind_group_layouts,
                                       uint32_t bind_group_layouts_count) {
	WGPUShaderModuleWGSLDescriptor shader_module_wgsl_descriptor = {
	    .code        = parameters->shader.data,
	    .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
	};

	WGPUShaderModuleDescriptor shader_module_descriptor = {
	    .nextInChain = (WGPUChainedStruct *)&shader_module_wgsl_descriptor,
	};

	WGPUShaderModule shader_module = wgpuDeviceCreateShaderModule(device->device, &shader_module_descriptor);
	wgpuShaderModuleGetCompilationInfo(shader_module, compilation_info_callback, NULL);

	WGPUPipelineLayoutDescriptor pipeline_layout_descriptor = {
	    .bindGroupLayoutCount = bind_group_layouts_count,
	    .bindGroupLayouts     = bind_group_layouts,
	};

	WGPUProgrammableStageDescriptor compute_state = {
	    .module     = shader_module,
	    .entryPoint = "main",
	};

	WGPUComputePipelineDescriptor compute_pipeline_descriptor = {
	    .layout  = wgpuDeviceCreatePipelineLayout(device->device, &pipeline_layout_descriptor),
	    .compute = compute_state,
	};

	pipe->compute_pipeline = wgpuDeviceCreateComputePipeline(device->device, &compute_pipeline_descriptor);
}

void kore_webgpu_compute_pipeline_destroy(kore_webgpu_compute_pipeline *pipe) {}

void kore_webgpu_ray_pipeline_init(kore_gpu_device *device, kore_webgpu_ray_pipeline *pipe, const kore_webgpu_ray_pipeline_parameters *parameters) {}

void kore_webgpu_ray_pipeline_destroy(kore_webgpu_ray_pipeline *pipe) {}
