#ifndef KORE_KOMPJUTA_PIPELINE_FUNCTIONS_HEADER
#define KORE_KOMPJUTA_PIPELINE_FUNCTIONS_HEADER

#include "device_structs.h"
#include "pipeline_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

void kore_kompjuta_render_pipeline_init(kore_kompjuta_device *device, kore_kompjuta_render_pipeline *pipe,
                                        const kore_kompjuta_render_pipeline_parameters *parameters);

void kore_kompjuta_render_pipeline_destroy(kore_kompjuta_render_pipeline *pipe);

void kore_kompjuta_compute_pipeline_init(kore_kompjuta_device *device, kore_kompjuta_compute_pipeline *pipe,
                                         const kore_kompjuta_compute_pipeline_parameters *parameters);

void kore_kompjuta_compute_pipeline_destroy(kore_kompjuta_compute_pipeline *pipe);

void kore_kompjuta_ray_pipeline_init(kore_gpu_device *device, kore_kompjuta_ray_pipeline *pipe, const kore_kompjuta_ray_pipeline_parameters *parameters);

void kore_kompjuta_ray_pipeline_destroy(kore_kompjuta_ray_pipeline *pipe);

#ifdef __cplusplus
}
#endif

#endif
