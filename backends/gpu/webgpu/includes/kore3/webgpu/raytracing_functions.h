#ifndef KORE_WEBGPU_RAYTRACING_FUNCTIONS_HEADER
#define KORE_WEBGPU_RAYTRACING_FUNCTIONS_HEADER

#include <kore3/gpu/raytracing.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_webgpu_raytracing_volume_destroy(kore_gpu_raytracing_volume *volume);

void kore_webgpu_raytracing_hierarchy_destroy(kore_gpu_raytracing_hierarchy *hierarchy);

#ifdef __cplusplus
}
#endif

#endif
