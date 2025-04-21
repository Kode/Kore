#ifndef KORE_WEBGPU_PIPELINE_FUNCTIONS_HEADER
#define KORE_WEBGPU_PIPELINE_FUNCTIONS_HEADER

#include "device_structs.h"
#include "pipeline_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

void kore_webgpu_render_pipeline_init(kore_webgpu_device *device, kore_webgpu_render_pipeline *pipe, const kore_webgpu_render_pipeline_parameters *parameters,
                                      const WGPUBindGroupLayout *bind_group_layouts, uint32_t bind_group_layouts_count);

void kore_webgpu_render_pipeline_destroy(kore_webgpu_render_pipeline *pipe);

void kore_webgpu_compute_pipeline_init(kore_webgpu_device *device, kore_webgpu_compute_pipeline *pipe,
                                       const kore_webgpu_compute_pipeline_parameters *parameters);

void kore_webgpu_compute_pipeline_destroy(kore_webgpu_compute_pipeline *pipe);

void kore_webgpu_ray_pipeline_init(kore_gpu_device *device, kore_webgpu_ray_pipeline *pipe, const kore_webgpu_ray_pipeline_parameters *parameters);

void kore_webgpu_ray_pipeline_destroy(kore_webgpu_ray_pipeline *pipe);

#ifdef __cplusplus
}
#endif

#endif
