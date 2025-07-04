#ifndef KORE_WEBGPU_DEVICE_STRUCTS_HEADER
#define KORE_WEBGPU_DEVICE_STRUCTS_HEADER

#include "webgpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_WEBGPU_FRAME_COUNT 2

typedef struct kore_webgpu_device {
	WGPUDevice  device;
	WGPUQueue   queue;
	WGPUSurface surface;
} kore_webgpu_device;

typedef struct kore_webgpu_query_set {
	int nothing;
} kore_webgpu_query_set;

typedef struct kore_webgpu_raytracing_volume {
	int nothing;
} kore_webgpu_raytracing_volume;

typedef struct kore_webgpu_raytracing_hierarchy {
	int nothing;
} kore_webgpu_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
