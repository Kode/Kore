#include <kore3/kompjuta/pipeline_functions.h>
#include <kore3/kompjuta/pipeline_structs.h>

#include <kore3/log.h>

void kore_kompjuta_render_pipeline_init(kore_kompjuta_device *device, kore_kompjuta_render_pipeline *pipe,
                                        const kore_kompjuta_render_pipeline_parameters *parameters) {}

void kore_kompjuta_render_pipeline_destroy(kore_kompjuta_render_pipeline *pipe) {}

void kore_kompjuta_compute_pipeline_init(kore_kompjuta_device *device, kore_kompjuta_compute_pipeline *pipe,
                                         const kore_kompjuta_compute_pipeline_parameters *parameters) {}

void kore_kompjuta_compute_pipeline_destroy(kore_kompjuta_compute_pipeline *pipe) {}

void kore_kompjuta_ray_pipeline_init(kore_gpu_device *device, kore_kompjuta_ray_pipeline *pipe, const kore_kompjuta_ray_pipeline_parameters *parameters) {}

void kore_kompjuta_ray_pipeline_destroy(kore_kompjuta_ray_pipeline *pipe) {}
