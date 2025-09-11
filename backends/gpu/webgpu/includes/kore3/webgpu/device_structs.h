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

#ifdef __cplusplus
}
#endif

#endif
