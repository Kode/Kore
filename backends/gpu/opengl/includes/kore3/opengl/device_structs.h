#ifndef KORE_OPENGL_DEVICE_STRUCTS_HEADER
#define KORE_OPENGL_DEVICE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_OPENGL_FRAME_COUNT 2

typedef struct kore_opengl_device {
	uint32_t default_framebuffer;
	uint32_t custom_framebuffer;
} kore_opengl_device;

typedef struct kore_opengl_query_set {
	int nothing;
} kore_opengl_query_set;

typedef struct kore_opengl_raytracing_volume {
	int nothing;
} kore_opengl_raytracing_volume;

typedef struct kore_opengl_raytracing_hierarchy {
	int nothing;
} kore_opengl_raytracing_hierarchy;

#ifdef __cplusplus
}
#endif

#endif
