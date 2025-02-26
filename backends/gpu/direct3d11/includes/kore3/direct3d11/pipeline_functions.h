#ifndef KORE_D3D11_PIPELINE_FUNCTIONS_HEADER
#define KORE_D3D11_PIPELINE_FUNCTIONS_HEADER

#include "device_structs.h"
#include "pipeline_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

void kore_d3d11_render_pipeline_init(kore_d3d11_device *device, kore_d3d11_render_pipeline *pipe, const kore_d3d11_render_pipeline_parameters *parameters);

void kore_d3d11_render_pipeline_destroy(kore_d3d11_render_pipeline *pipe);

void kore_d3d11_compute_pipeline_init(kore_d3d11_device *device, kore_d3d11_compute_pipeline *pipe, const kore_d3d11_compute_pipeline_parameters *parameters);

void kore_d3d11_compute_pipeline_destroy(kore_d3d11_compute_pipeline *pipe);

void kore_d3d11_ray_pipeline_init(kore_gpu_device *device, kore_d3d11_ray_pipeline *pipe, const kore_d3d11_ray_pipeline_parameters *parameters);

void kore_d3d11_ray_pipeline_destroy(kore_d3d11_ray_pipeline *pipe);

#ifdef __cplusplus
}
#endif

#endif
