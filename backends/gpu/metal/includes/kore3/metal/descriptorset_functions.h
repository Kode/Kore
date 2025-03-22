#ifndef KORE_METAL_DESCRIPTORSET_FUNCTIONS_HEADER
#define KORE_METAL_DESCRIPTORSET_FUNCTIONS_HEADER

#include <kore3/metal/buffer_structs.h>
#include <kore3/metal/descriptorset_structs.h>
#include <kore3/metal/device_structs.h>

#include <kore3/gpu/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_metal_descriptor_set_prepare_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size);
void kore_metal_descriptor_set_prepare_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view);

#ifdef __cplusplus
}
#endif

#endif
