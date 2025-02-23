#ifndef KORE_METAL_DEVICE_STRUCTS_HEADER
#define KORE_METAL_DEVICE_STRUCTS_HEADER

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/texture.h>
#include <kore3/util/indexallocator.h>
#include <kore3/util/offalloc/offalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KOPE_METAL_FRAME_COUNT 2

typedef struct kore_metal_device {
	void *device;
	void *library;
} kore_metal_device;

typedef struct kore_metal_query_set {
	int nothing;
} kore_metal_query_set;

typedef struct kore_metal_raytracing_volume {
	int nothing;
} kore_metal_raytracing_volume;

typedef struct kore_metal_raytracing_hierarchy {
	int nothing;
} kore_metal_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
