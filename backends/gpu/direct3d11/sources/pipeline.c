#include "pipeline_functions.h"
#include "pipeline_structs.h"

#include <kore3/log.h>

void kore_d3d11_render_pipeline_init(kore_d3d11_device *device, kore_d3d11_render_pipeline *pipe, const kore_d3d11_render_pipeline_parameters *parameters) {

}

void kore_d3d11_render_pipeline_destroy(kore_d3d11_render_pipeline *pipe) {}

void kore_d3d11_compute_pipeline_init(kore_d3d11_device *device, kore_d3d11_compute_pipeline *pipe,
                                       const kore_d3d11_compute_pipeline_parameters *parameters) {}

void kore_d3d11_compute_pipeline_destroy(kore_d3d11_compute_pipeline *pipe) {}

void kore_d3d11_ray_pipeline_init(kore_gpu_device *device, kore_d3d11_ray_pipeline *pipe, const kore_d3d11_ray_pipeline_parameters *parameters) {}

void kore_d3d11_ray_pipeline_destroy(kore_d3d11_ray_pipeline *pipe) {}
