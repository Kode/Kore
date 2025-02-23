#ifndef KORE_METAL_PIPELINE_FUNCTIONS_HEADER
#define KORE_METAL_PIPELINE_FUNCTIONS_HEADER

#include <kore3/metal/device_structs.h>
#include <kore3/metal/pipeline_structs.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_metal_render_pipeline_init(kore_metal_device *device, kore_metal_render_pipeline *pipe, const kore_metal_render_pipeline_parameters *parameters);

void kore_metal_render_pipeline_destroy(kore_metal_render_pipeline *pipe);

void kore_metal_compute_pipeline_init(kore_metal_device *device, kore_metal_compute_pipeline *pipe, const kore_metal_compute_pipeline_parameters *parameters);

void kore_metal_compute_pipeline_destroy(kore_metal_compute_pipeline *pipe);

void kore_metal_ray_pipeline_init(kore_gpu_device *device, kore_metal_ray_pipeline *pipe, const kore_metal_ray_pipeline_parameters *parameters);

void kore_metal_ray_pipeline_destroy(kore_metal_ray_pipeline *pipe);

#ifdef __cplusplus
}
#endif

#endif
