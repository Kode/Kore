#ifndef KORE_D3D11_DEVICE_STRUCTS_HEADER
#define KORE_D3D11_DEVICE_STRUCTS_HEADER

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/texture.h>
#include <kore3/util/indexallocator.h>
#include <kore3/util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_D3D11_FRAME_COUNT 2

typedef struct kore_d3d11_device {
	int nothing;
} kore_d3d11_device;

typedef struct kore_d3d11_query_set {
	int nothing;
} kore_d3d11_query_set;

typedef struct kore_d3d11_raytracing_volume {
	int nothing;
} kore_d3d11_raytracing_volume;

typedef struct kore_d3d11_raytracing_hierarchy {
	int nothing;
} kore_d3d11_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
