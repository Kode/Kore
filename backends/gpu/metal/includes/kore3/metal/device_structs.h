#ifndef KORE_METAL_DEVICE_STRUCTS_HEADER
#define KORE_METAL_DEVICE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_METAL_FRAME_COUNT 2

#define KORE_METAL_EXECUTION_FENCE_COUNT 8

typedef struct kore_metal_execution_fence {
	void    *command_buffers[KORE_METAL_EXECUTION_FENCE_COUNT];
	uint64_t commend_buffer_execution_indices[KORE_METAL_EXECUTION_FENCE_COUNT];

	uint64_t next_execution_index;
	uint64_t completed_index;
} kore_metal_execution_fence;

typedef struct kore_metal_device {
	void *device;
	void *library;

	kore_metal_execution_fence execution_fence;
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
