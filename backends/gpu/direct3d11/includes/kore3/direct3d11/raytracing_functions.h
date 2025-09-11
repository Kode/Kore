#ifndef KORE_D3D11_RAYTRACING_FUNCTIONS_HEADER
#define KORE_D3D11_RAYTRACING_FUNCTIONS_HEADER

#include <kore3/gpu/raytracing.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_d3d11_raytracing_volume_destroy(kore_gpu_raytracing_volume *volume);

void kore_d3d11_raytracing_hierarchy_destroy(kore_gpu_raytracing_hierarchy *hierarchy);

#ifdef __cplusplus
}
#endif

#endif
