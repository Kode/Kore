#ifndef KORE_D3D12_PIPELINE_FUNCTIONS_HEADER
#define KORE_D3D12_PIPELINE_FUNCTIONS_HEADER

#include "device_structs.h"
#include "pipeline_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

void kore_d3d12_render_pipeline_init(kore_d3d12_device *device, kore_d3d12_render_pipeline *pipe, const kore_d3d12_render_pipeline_parameters *parameters);

void kore_d3d12_render_pipeline_destroy(kore_d3d12_render_pipeline *pipe);

void kore_d3d12_compute_pipeline_init(kore_d3d12_device *device, kore_d3d12_compute_pipeline *pipe, const kore_d3d12_compute_pipeline_parameters *parameters);

void kore_d3d12_compute_pipeline_destroy(kore_d3d12_compute_pipeline *pipe);

struct ID3D12RootSignature;

void kore_d3d12_ray_pipeline_init(kore_gpu_device *device, kore_d3d12_ray_pipeline *pipe, const kore_d3d12_ray_pipeline_parameters *parameters,
                                  struct ID3D12RootSignature *root_signature);

void kore_d3d12_ray_pipeline_destroy(kore_d3d12_ray_pipeline *pipe);

#ifdef __cplusplus
}
#endif

#endif
