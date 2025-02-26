#ifndef KORE_OPENGL_PIPELINE_FUNCTIONS_HEADER
#define KORE_OPENGL_PIPELINE_FUNCTIONS_HEADER

#include "device_structs.h"
#include "pipeline_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

void kore_opengl_render_pipeline_init(kore_opengl_device *device, kore_opengl_render_pipeline *pipe, const kore_opengl_render_pipeline_parameters *parameters);

void kore_opengl_render_pipeline_destroy(kore_opengl_render_pipeline *pipe);

void kore_opengl_compute_pipeline_init(kore_opengl_device *device, kore_opengl_compute_pipeline *pipe,
                                       const kore_opengl_compute_pipeline_parameters *parameters);

void kore_opengl_compute_pipeline_destroy(kore_opengl_compute_pipeline *pipe);

void kore_opengl_ray_pipeline_init(kore_gpu_device *device, kore_opengl_ray_pipeline *pipe, const kore_opengl_ray_pipeline_parameters *parameters);

void kore_opengl_ray_pipeline_destroy(kore_opengl_ray_pipeline *pipe);

#ifdef __cplusplus
}
#endif

#endif
