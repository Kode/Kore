#ifndef KORE_OPENGL_DESCRIPTORSET_FUNCTIONS_HEADER
#define KORE_OPENGL_DESCRIPTORSET_FUNCTIONS_HEADER

#include "buffer_structs.h"
#include "descriptorset_structs.h"
#include "device_structs.h"

#include <kore3/gpu/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_opengl_descriptor_set_set_buffer_view_cbv(kore_gpu_device *device, kore_opengl_descriptor_set *set, kore_gpu_buffer *buffer, uint32_t index);
void kore_opengl_descriptor_set_set_buffer_view_srv(kore_gpu_device *device, kore_opengl_descriptor_set *set, kore_gpu_buffer *buffer, uint32_t index);
void kore_opengl_descriptor_set_set_bvh_view_srv(kore_gpu_device *device, kore_opengl_descriptor_set *set, kore_gpu_raytracing_hierarchy *bvh, uint32_t index);
void kore_opengl_descriptor_set_set_texture_view_srv(kore_gpu_device *device, uint32_t offset, const kore_gpu_texture_view *texture_view);
void kore_opengl_descriptor_set_set_texture_array_view_srv(kore_gpu_device *device, kore_opengl_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                           uint32_t index);
void kore_opengl_descriptor_set_set_texture_cube_view_srv(kore_gpu_device *device, kore_opengl_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                          uint32_t index);
void kore_opengl_descriptor_set_set_texture_view_uav(kore_gpu_device *device, kore_opengl_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                     uint32_t index);
void kore_opengl_descriptor_set_set_sampler(kore_gpu_device *device, kore_opengl_descriptor_set *set, kore_gpu_sampler *sampler, uint32_t index);

void kore_opengl_descriptor_set_prepare_cbv_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size);
void kore_opengl_descriptor_set_prepare_srv_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view);
void kore_opengl_descriptor_set_prepare_uav_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view);

#ifdef __cplusplus
}
#endif

#endif
