#include <kore3/direct3d11/pipeline_functions.h>
#include <kore3/direct3d11/pipeline_structs.h>

#include <kore3/log.h>

void kore_d3d11_render_pipeline_init(kore_d3d11_device *device, kore_d3d11_render_pipeline *pipe, const kore_d3d11_render_pipeline_parameters *parameters) {
	kore_microsoft_affirm(
	    device->device->lpVtbl->CreateVertexShader(device->device, parameters->vertex.shader.data, parameters->vertex.shader.size, NULL, &pipe->vertex_shader));

	D3D11_INPUT_ELEMENT_DESC input_element_desc = {
	    .SemanticName         = "TEXCOORD",
	    .SemanticIndex        = 0,
	    .Format               = DXGI_FORMAT_R32G32B32_FLOAT,
	    .InputSlot            = 0,
	    .AlignedByteOffset    = 0,
	    .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
	    .InstanceDataStepRate = 0,
	};
	kore_microsoft_affirm(device->device->lpVtbl->CreateInputLayout(device->device, &input_element_desc, 1, parameters->vertex.shader.data,
	                                                                parameters->vertex.shader.size, &pipe->input_layout));

	kore_microsoft_affirm(device->device->lpVtbl->CreatePixelShader(device->device, parameters->fragment.shader.data, parameters->fragment.shader.size, NULL,
	                                                                &pipe->fragment_shader));

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {
	    .DepthEnable    = FALSE,
	    .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
	};

	kore_microsoft_affirm(device->device->lpVtbl->CreateDepthStencilState(device->device, &depth_stencil_desc, &pipe->depth_stencil_state));

	D3D11_RASTERIZER_DESC rasterizer_desc = {
	    .CullMode              = D3D11_CULL_NONE,
	    .FillMode              = D3D11_FILL_SOLID,
	    .FrontCounterClockwise = TRUE,
	    .DepthBias             = 0,
	    .SlopeScaledDepthBias  = 0.0f,
	    .DepthBiasClamp        = 0.0f,
	    .DepthClipEnable       = TRUE,
	    .ScissorEnable         = FALSE,
	    .MultisampleEnable     = FALSE,
	    .AntialiasedLineEnable = FALSE,
	};

	kore_microsoft_affirm(device->device->lpVtbl->CreateRasterizerState(device->device, &rasterizer_desc, &pipe->rasterizer_state));
}

void kore_d3d11_render_pipeline_destroy(kore_d3d11_render_pipeline *pipe) {}

void kore_d3d11_compute_pipeline_init(kore_d3d11_device *device, kore_d3d11_compute_pipeline *pipe, const kore_d3d11_compute_pipeline_parameters *parameters) {}

void kore_d3d11_compute_pipeline_destroy(kore_d3d11_compute_pipeline *pipe) {}

void kore_d3d11_ray_pipeline_init(kore_gpu_device *device, kore_d3d11_ray_pipeline *pipe, const kore_d3d11_ray_pipeline_parameters *parameters) {}

void kore_d3d11_ray_pipeline_destroy(kore_d3d11_ray_pipeline *pipe) {}
