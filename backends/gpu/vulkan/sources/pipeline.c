#include <kore3/vulkan/pipeline_functions.h>
#include <kore3/vulkan/pipeline_structs.h>

#include <kore3/log.h>

static uint32_t vertex_attribute_size(kore_vulkan_vertex_format format) {
	switch (format) {
	case KORE_VULKAN_VERTEX_FORMAT_UINT8X2:
		return 2;
	case KORE_VULKAN_VERTEX_FORMAT_UINT8X4:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_SINT8X2:
		return 2;
	case KORE_VULKAN_VERTEX_FORMAT_SINT8X4:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_UNORM8X2:
		return 2;
	case KORE_VULKAN_VERTEX_FORMAT_UNORM8X4:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_SNORM8X2:
		return 2;
	case KORE_VULKAN_VERTEX_FORMAT_SNORM8X4:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_UINT16X2:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_UINT16X4:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_SINT16X2:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_SINT16X4:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_UNORM16X2:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_UNORM16X4:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_SNORM16X2:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_SNORM16X4:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_FLOAT16X2:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_FLOAT16X4:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_FLOAT32:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_FLOAT32X2:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_FLOAT32X3:
		return 12;
	case KORE_VULKAN_VERTEX_FORMAT_FLOAT32X4:
		return 16;
	case KORE_VULKAN_VERTEX_FORMAT_UINT32:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_UINT32X2:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_UINT32X3:
		return 12;
	case KORE_VULKAN_VERTEX_FORMAT_UINT32X4:
		return 16;
	case KORE_VULKAN_VERTEX_FORMAT_SINT32:
		return 4;
	case KORE_VULKAN_VERTEX_FORMAT_SINT32X2:
		return 8;
	case KORE_VULKAN_VERTEX_FORMAT_SINT32X3:
		return 12;
	case KORE_VULKAN_VERTEX_FORMAT_SINT32X4:
		return 16;
	case KORE_VULKAN_VERTEX_FORMAT_UNORM10_10_10_2:
		return 4;
	}

	return 0;
}

static VkCullModeFlags convert_cull_mode(kore_vulkan_cull_mode mode) {
	switch (mode) {
	case KORE_VULKAN_CULL_MODE_NONE:
		return VK_CULL_MODE_NONE;
	case KORE_VULKAN_CULL_MODE_FRONT:
		return VK_CULL_MODE_FRONT_BIT;
	case KORE_VULKAN_CULL_MODE_BACK:
		return VK_CULL_MODE_BACK_BIT;
	}

	return VK_CULL_MODE_NONE;
}

static VkBlendFactor convert_blend_factor(kore_vulkan_blend_factor factor) {
	switch (factor) {
	case KORE_VULKAN_BLEND_FACTOR_ZERO:
		return VK_BLEND_FACTOR_ZERO;
	case KORE_VULKAN_BLEND_FACTOR_ONE:
		return VK_BLEND_FACTOR_ONE;
	case KORE_VULKAN_BLEND_FACTOR_SRC:
		return VK_BLEND_FACTOR_SRC_COLOR;
	case KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_SRC:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case KORE_VULKAN_BLEND_FACTOR_SRC_ALPHA:
		return VK_BLEND_FACTOR_SRC_ALPHA;
	case KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case KORE_VULKAN_BLEND_FACTOR_DST:
		return VK_BLEND_FACTOR_DST_COLOR;
	case KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_DST:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case KORE_VULKAN_BLEND_FACTOR_DST_ALPHA:
		return VK_BLEND_FACTOR_DST_ALPHA;
	case KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	case KORE_VULKAN_BLEND_FACTOR_SRC_ALPHA_SATURATED:
		return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	case KORE_VULKAN_BLEND_FACTOR_CONSTANT:
		return VK_BLEND_FACTOR_CONSTANT_COLOR;
	case KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_CONSTANT:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
	}

	return VK_BLEND_FACTOR_ONE;
}

static VkBlendOp convert_blend_operation(kore_vulkan_blend_operation operation) {
	switch (operation) {
	case KORE_VULKAN_BLEND_OPERATION_ADD:
		return VK_BLEND_OP_ADD;
	case KORE_VULKAN_BLEND_OPERATION_SUBTRACT:
		return VK_BLEND_OP_SUBTRACT;
	case KORE_VULKAN_BLEND_OPERATION_REVERSE_SUBTRACT:
		return VK_BLEND_OP_REVERSE_SUBTRACT;
	case KORE_VULKAN_BLEND_OPERATION_MIN:
		return VK_BLEND_OP_MIN;
	case KORE_VULKAN_BLEND_OPERATION_MAX:
		return VK_BLEND_OP_MAX;
	}

	return VK_BLEND_OP_ADD;
}

static VkCompareOp convert_compare_mode(kore_gpu_compare_function compare) {
	switch (compare) {
	case KORE_GPU_COMPARE_FUNCTION_UNDEFINED:
		assert(false);
		return VK_COMPARE_OP_NEVER;
	case KORE_GPU_COMPARE_FUNCTION_NEVER:
		return VK_COMPARE_OP_NEVER;
	case KORE_GPU_COMPARE_FUNCTION_LESS:
		return VK_COMPARE_OP_LESS;
	case KORE_GPU_COMPARE_FUNCTION_EQUAL:
		return VK_COMPARE_OP_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_LESS_EQUAL:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER:
		return VK_COMPARE_OP_GREATER;
	case KORE_GPU_COMPARE_FUNCTION_NOT_EQUAL:
		return VK_COMPARE_OP_NOT_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER_EQUAL:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_ALWAYS:
		return VK_COMPARE_OP_ALWAYS;
	}

	assert(false);
	return VK_COMPARE_OP_NEVER;
}

static VkShaderModule create_shader_module(kore_vulkan_device *device, const kore_vulkan_shader *shader) {
	const VkShaderModuleCreateInfo shader_module_create_info = {
	    .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
	    .pNext    = NULL,
	    .codeSize = shader->size,
	    .pCode    = (const uint32_t *)shader->data,
	    .flags    = 0,
	};

	VkShaderModule module;
	VkResult       result = vkCreateShaderModule(device->device, &shader_module_create_info, NULL, &module);
	assert(result == VK_SUCCESS);

	return module;
}

void kore_vulkan_render_pipeline_init(kore_vulkan_device *device, kore_vulkan_render_pipeline *pipeline,
                                      const kore_vulkan_render_pipeline_parameters *parameters, const VkDescriptorSetLayout *descriptor_set_layouts,
                                      uint32_t descriptor_set_layouts_count, uint32_t root_constants_size) {
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
	    .sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	    .pNext          = NULL,
	    .setLayoutCount = descriptor_set_layouts_count,
	    .pSetLayouts    = descriptor_set_layouts,
	};

	VkPushConstantRange push_constant_range = {
	    .offset     = 0,
	    .size       = root_constants_size,
	    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
	};

	if (root_constants_size > 0) {
		pipeline_layout_create_info.pPushConstantRanges    = &push_constant_range;
		pipeline_layout_create_info.pushConstantRangeCount = 1;
	}

	VkResult result = vkCreatePipelineLayout(device->device, &pipeline_layout_create_info, NULL, &pipeline->pipeline_layout);
	assert(result == VK_SUCCESS);

	uint32_t attributes_count = 0;
	uint32_t bindings_count   = 0;
	for (int i = 0; i < parameters->vertex.buffers_count; ++i) {
		attributes_count += (uint32_t)parameters->vertex.buffers[i].attributes_count;
		++bindings_count;
	}

	VkVertexInputBindingDescription   binding_descriptions[64];
	VkVertexInputAttributeDescription attribute_descriptions[256];

	const VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
	    .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	    .pNext                           = NULL,
	    .vertexBindingDescriptionCount   = bindings_count,
	    .pVertexBindingDescriptions      = binding_descriptions,
	    .vertexAttributeDescriptionCount = attributes_count,
	    .pVertexAttributeDescriptions    = attribute_descriptions,
	};

	uint32_t global_attribute_index = 0;
	for (uint32_t binding_index = 0; binding_index < bindings_count; ++binding_index) {
		uint32_t stride = 0;
		for (uint32_t attribute_index = 0; attribute_index < parameters->vertex.buffers[binding_index].attributes_count; ++attribute_index) {
			kore_vulkan_vertex_attribute attribute                  = parameters->vertex.buffers[binding_index].attributes[attribute_index];
			attribute_descriptions[global_attribute_index].binding  = binding_index;
			attribute_descriptions[global_attribute_index].location = attribute.shader_location;
			attribute_descriptions[global_attribute_index].offset   = (uint32_t)attribute.offset;
			stride += vertex_attribute_size(attribute.format);

			switch (attribute.format) {
			case KORE_VULKAN_VERTEX_FORMAT_FLOAT32:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32_SFLOAT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_FLOAT32X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_FLOAT32X3:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_FLOAT32X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT8X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT8X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8_UINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SNORM8X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8_SNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UNORM8X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8_UNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT8X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8B8A8_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT8X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8B8A8_UINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SNORM8X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8B8A8_SNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UNORM8X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R8G8B8A8_UNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT16X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT16X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16_UINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SNORM16X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16_SNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UNORM16X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16_UNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT16X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16B16A16_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT16X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16B16A16_UINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SNORM16X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16B16A16_SNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UNORM16X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R16G16B16A16_UNORM;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT32:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT32:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32_UINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT32X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT32X2:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32_UINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT32X3:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32B32_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT32X3:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32B32_UINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_SINT32X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32B32A32_SINT;
				break;
			case KORE_VULKAN_VERTEX_FORMAT_UINT32X4:
				attribute_descriptions[global_attribute_index].format = VK_FORMAT_R32G32B32A32_UINT;
				break;
			default:
				assert(false);
				break;
			}

			++global_attribute_index;
		}

		binding_descriptions[binding_index].binding   = binding_index;
		binding_descriptions[binding_index].stride    = stride;
		binding_descriptions[binding_index].inputRate = (parameters->vertex.buffers[binding_index].step_mode == KORE_VULKAN_VERTEX_STEP_MODE_INSTANCE)
		                                                    ? VK_VERTEX_INPUT_RATE_INSTANCE
		                                                    : VK_VERTEX_INPUT_RATE_VERTEX;
	}

	const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
	    .sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	const VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
	    .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	    .polygonMode             = VK_POLYGON_MODE_FILL,
	    .cullMode                = convert_cull_mode(parameters->primitive.cull_mode),
	    .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
	    .depthClampEnable        = VK_FALSE,
	    .rasterizerDiscardEnable = VK_FALSE,
	    .depthBiasEnable         = VK_FALSE,
	    .lineWidth               = 1.0f,
	};

	VkPipelineColorBlendAttachmentState color_blend_attachment_states[8] = {0};
	assert(parameters->fragment.targets_count <= sizeof(color_blend_attachment_states) / sizeof(color_blend_attachment_states[0]));
	for (size_t target_index = 0; target_index < parameters->fragment.targets_count; ++target_index) {
		VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
		    .colorWriteMask = parameters->fragment.targets[target_index].write_mask,
		    .blendEnable    = parameters->fragment.targets[target_index].blend.color.src_factor != KORE_VULKAN_BLEND_FACTOR_ONE ||
		                   parameters->fragment.targets[target_index].blend.color.dst_factor != KORE_VULKAN_BLEND_FACTOR_ZERO ||
		                   parameters->fragment.targets[target_index].blend.alpha.src_factor != KORE_VULKAN_BLEND_FACTOR_ONE ||
		                   parameters->fragment.targets[target_index].blend.alpha.dst_factor != KORE_VULKAN_BLEND_FACTOR_ZERO,
		    .srcColorBlendFactor = convert_blend_factor(parameters->fragment.targets[target_index].blend.color.src_factor),
		    .dstColorBlendFactor = convert_blend_factor(parameters->fragment.targets[target_index].blend.color.dst_factor),
		    .colorBlendOp        = convert_blend_operation(parameters->fragment.targets[target_index].blend.color.operation),
		    .srcAlphaBlendFactor = convert_blend_factor(parameters->fragment.targets[target_index].blend.alpha.src_factor),
		    .dstAlphaBlendFactor = convert_blend_factor(parameters->fragment.targets[target_index].blend.alpha.dst_factor),
		    .alphaBlendOp        = convert_blend_operation(parameters->fragment.targets[target_index].blend.alpha.operation),
		};
		color_blend_attachment_states[target_index] = color_blend_attachment_state;
	}

	const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
	    .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	    .attachmentCount = (uint32_t)parameters->fragment.targets_count,
	    .pAttachments    = color_blend_attachment_states,
	};

	const VkPipelineViewportStateCreateInfo viewport_state_create_info = {
	    .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	    .viewportCount = 1,
	    .scissorCount  = 1,
	};

	const VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	const VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
	    .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
	    .pDynamicStates    = dynamic_states,
	    .dynamicStateCount = sizeof(dynamic_states) / sizeof(dynamic_states[0]),
	};

	const VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
	    .sType           = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
	    .depthTestEnable = parameters->depth_stencil.depth_compare != KORE_GPU_COMPARE_FUNCTION_ALWAYS &&
	                       parameters->depth_stencil.depth_compare != KORE_GPU_COMPARE_FUNCTION_UNDEFINED,
	    .depthWriteEnable      = parameters->depth_stencil.depth_write_enabled,
	    .depthCompareOp        = convert_compare_mode(parameters->depth_stencil.depth_compare),
	    .depthBoundsTestEnable = VK_FALSE,
	    .back =
	        {
	            .failOp    = VK_STENCIL_OP_KEEP,
	            .passOp    = VK_STENCIL_OP_KEEP,
	            .compareOp = VK_COMPARE_OP_ALWAYS,
	        },
	    .stencilTestEnable = VK_FALSE,
	    .front =
	        {
	            .failOp    = VK_STENCIL_OP_KEEP,
	            .passOp    = VK_STENCIL_OP_KEEP,
	            .compareOp = VK_COMPARE_OP_ALWAYS,
	        },
	};

	const VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
	    .sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	    .pSampleMask          = NULL,
	    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	};

	const VkPipelineShaderStageCreateInfo shader_stages[2] = {
	    {
	        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
	        .module = create_shader_module(device, &parameters->vertex.shader),
	        .pName  = "main",
	    },
	    {
	        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	        .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
	        .module = create_shader_module(device, &parameters->fragment.shader),
	        .pName  = "main",
	    },
	};

	VkFormat color_attachment_formats[8];
	for (size_t target_index = 0; target_index < parameters->fragment.targets_count; ++target_index) {
		color_attachment_formats[target_index] = convert_to_vulkan_format(parameters->fragment.targets[target_index].format);
	}

	const VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
	    .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
	    .pNext                   = NULL,
	    .colorAttachmentCount    = (uint32_t)parameters->fragment.targets_count,
	    .pColorAttachmentFormats = color_attachment_formats,
	    .depthAttachmentFormat   = convert_to_vulkan_format(parameters->depth_stencil.format),
	};

	if (!device->has_dynamic_rendering) {
		find_pipeline_render_pass(device->device, color_attachment_formats, (uint32_t)parameters->fragment.targets_count,
		                          convert_to_vulkan_format(parameters->depth_stencil.format), &pipeline->render_pass);
	}

	VkGraphicsPipelineCreateInfo pipeline_create_info = {
	    .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
	    .pNext               = NULL,
	    .layout              = pipeline->pipeline_layout,
	    .renderPass          = VK_NULL_HANDLE,
	    .stageCount          = 2,
	    .pVertexInputState   = &vertex_input_state_create_info,
	    .pInputAssemblyState = &input_assembly_state_create_info,
	    .pRasterizationState = &rasterization_state_create_info,
	    .pColorBlendState    = &color_blend_state_create_info,
	    .pMultisampleState   = &multisample_state_create_info,
	    .pViewportState      = &viewport_state_create_info,
	    .pDepthStencilState  = &depth_stencil_state_create_info,
	    .pStages             = shader_stages,
	    .pDynamicState       = &dynamic_state_create_info,
	};

	if (device->has_dynamic_rendering) {
		pipeline_create_info.pNext = &pipeline_rendering_create_info;
	}
	else {
		pipeline_create_info.renderPass = pipeline->render_pass;
	}

	result = vkCreateGraphicsPipelines(device->device, VK_NULL_HANDLE, 1, &pipeline_create_info, NULL, &pipeline->pipeline);
	assert(result == VK_SUCCESS);

	vkDestroyShaderModule(device->device, shader_stages[0].module, NULL);
	vkDestroyShaderModule(device->device, shader_stages[1].module, NULL);
}

void kore_vulkan_render_pipeline_destroy(kore_vulkan_render_pipeline *pipeline) {}

void kore_vulkan_compute_pipeline_init(kore_vulkan_device *device, kore_vulkan_compute_pipeline *pipeline,
                                       const kore_vulkan_compute_pipeline_parameters *parameters, const VkDescriptorSetLayout *descriptor_set_layouts,
                                       uint32_t descriptor_set_layouts_count, uint32_t root_constants_size) {
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
	    .sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	    .pNext          = NULL,
	    .setLayoutCount = descriptor_set_layouts_count,
	    .pSetLayouts    = descriptor_set_layouts,
	};

	VkPushConstantRange push_constant_range = {
	    .offset     = 0,
	    .size       = root_constants_size,
	    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
	};

	if (root_constants_size > 0) {
		pipeline_layout_create_info.pPushConstantRanges    = &push_constant_range;
		pipeline_layout_create_info.pushConstantRangeCount = 1;
	}

	VkResult result = vkCreatePipelineLayout(device->device, &pipeline_layout_create_info, NULL, &pipeline->pipeline_layout);
	assert(result == VK_SUCCESS);

	const VkComputePipelineCreateInfo pipeline_create_info = {
	    .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
	    .pNext  = NULL,
	    .layout = pipeline->pipeline_layout,
	    .stage =
	        {
	            .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	            .stage  = VK_SHADER_STAGE_COMPUTE_BIT,
	            .module = create_shader_module(device, &parameters->shader),
	            .pName  = "main",
	        },
	};

	result = vkCreateComputePipelines(device->device, VK_NULL_HANDLE, 1, &pipeline_create_info, NULL, &pipeline->pipeline);
	assert(result == VK_SUCCESS);

	vkDestroyShaderModule(device->device, pipeline_create_info.stage.module, NULL);
}

void kore_vulkan_compute_pipeline_destroy(kore_vulkan_compute_pipeline *pipeline) {}

void kore_vulkan_ray_pipeline_init(kore_gpu_device *device, kore_vulkan_ray_pipeline *pipeline, const kore_vulkan_ray_pipeline_parameters *parameters) {}

void kore_vulkan_ray_pipeline_destroy(kore_vulkan_ray_pipeline *pipeline) {}
