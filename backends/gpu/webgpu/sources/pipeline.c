#include "pipeline_functions.h"
#include "pipeline_structs.h"

#include <kinc/log.h>

void kore_webgpu_render_pipeline_init(kore_webgpu_device *device, kore_webgpu_render_pipeline *pipe, const kore_webgpu_render_pipeline_parameters *parameters) {

}

void kore_webgpu_render_pipeline_destroy(kore_webgpu_render_pipeline *pipe) {}

void kore_webgpu_compute_pipeline_init(kore_webgpu_device *device, kore_webgpu_compute_pipeline *pipe,
                                       const kore_webgpu_compute_pipeline_parameters *parameters) {}

void kore_webgpu_compute_pipeline_destroy(kore_webgpu_compute_pipeline *pipe) {}

void kore_webgpu_ray_pipeline_init(kore_gpu_device *device, kore_webgpu_ray_pipeline *pipe, const kore_webgpu_ray_pipeline_parameters *parameters) {}

void kore_webgpu_ray_pipeline_destroy(kore_webgpu_ray_pipeline *pipe) {}
