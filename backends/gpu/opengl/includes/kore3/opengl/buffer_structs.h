#ifndef KORE_OPENGL_BUFFER_STRUCTS_HEADER
#define KORE_OPENGL_BUFFER_STRUCTS_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

// add-ons to kore_gpu_buffer_usage, top 16 bits only
typedef enum kore_opengl_buffer_usage {
	KORE_OPENGL_BUFFER_USAGE_VERTEX = 0x00010000,
} kore_opengl_buffer_usage;

typedef struct kore_opengl_buffer {
	uint32_t buffer;
	uint32_t buffer_type;
	uint64_t size;
#ifdef KORE_WEBGL
	void *data;
#endif
} kore_opengl_buffer;

#ifdef __cplusplus
}
#endif

#endif
