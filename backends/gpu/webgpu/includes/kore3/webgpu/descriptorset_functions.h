#ifndef KORE_WEBGPU_DESCRIPTORSET_FUNCTIONS_HEADER
#define KORE_WEBGPU_DESCRIPTORSET_FUNCTIONS_HEADER

#include "buffer_structs.h"
#include "descriptorset_structs.h"
#include "device_structs.h"

#include <kore3/gpu/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_webgpu_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif
