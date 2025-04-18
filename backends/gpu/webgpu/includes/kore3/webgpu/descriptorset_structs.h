#ifndef KORE_WEBGPU_DESCRIPTORSET_STRUCTS_HEADER
#define KORE_WEBGPU_DESCRIPTORSET_STRUCTS_HEADER

#include "device_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_webgpu_descriptor_set {
	WGPUBindGroup bind_group;
} kore_webgpu_descriptor_set;

#ifdef __cplusplus
}
#endif

#endif
