#ifndef KORE_D3D12_RAYTRACING_FUNCTIONS_HEADER
#define KORE_D3D12_RAYTRACING_FUNCTIONS_HEADER

#include <kore3/gpu/raytracing.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_d3d12_raytracing_volume_destroy(kore_gpu_raytracing_volume *volume);

void kore_d3d12_raytracing_hierarchy_destroy(kore_gpu_raytracing_hierarchy *hierarchy);

bool kore_d3d12_raytracing_volume_in_use(kore_gpu_raytracing_volume *volume);

void kore_d3d12_raytracing_volume_wait_until_not_in_use(kore_gpu_raytracing_volume *volume);

bool kore_d3d12_raytracing_hierarchy_in_use(kore_gpu_raytracing_hierarchy *hierarchy);

void kore_d3d12_raytracing_hierarchy_wait_until_not_in_use(kore_gpu_raytracing_hierarchy *hierarchy);

#ifdef __cplusplus
}
#endif

#endif
