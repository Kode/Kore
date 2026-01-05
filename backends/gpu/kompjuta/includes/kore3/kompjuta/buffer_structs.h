#ifndef KORE_KOMPJUTA_BUFFER_STRUCTS_HEADER
#define KORE_KOMPJUTA_BUFFER_STRUCTS_HEADER

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

// add-ons to kore_gpu_buffer_usage, top 16 bits only
typedef enum kore_kompjuta_buffer_usage {
	KORE_KOMPJUTA_BUFFER_USAGE_VERTEX  = 0x00010000,
	KORE_KOMPJUTA_BUFFER_USAGE_UNIFORM = 0x00020000,
	KORE_KOMPJUTA_BUFFER_USAGE_STORAGE = 0x00040000,
} kore_kompjuta_buffer_usage;

typedef struct kore_kompjuta_buffer {
	int nothing;
} kore_kompjuta_buffer;

#ifdef __cplusplus
}
#endif

#endif
