#ifndef KORE_OPENGL_BUFFER_STRUCTS_HEADER
#define KORE_OPENGL_BUFFER_STRUCTS_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

typedef struct kore_opengl_buffer {
	uint32_t buffer;
	uint32_t buffer_type;
	uint64_t size;
} kore_opengl_buffer;

#ifdef __cplusplus
}
#endif

#endif
