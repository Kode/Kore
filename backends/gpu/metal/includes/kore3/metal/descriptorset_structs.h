#ifndef KORE_METAL_DESCRIPTORSET_STRUCTS_HEADER
#define KORE_METAL_DESCRIPTORSET_STRUCTS_HEADER

#include <kore3/metal/device_structs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_metal_descriptor_set {
	kore_gpu_buffer argument_buffer;
} kore_metal_descriptor_set;

#ifdef __cplusplus
}
#endif

#endif
