#ifndef KORE_METAL_COMMANDLIST_FUNCTIONS_HEADER
#define KORE_METAL_COMMANDLIST_FUNCTIONS_HEADER

#include <kore3/gpu/commandlist.h>

#include <kore3/metal/device_structs.h>
#include <kore3/metal/pipeline_structs.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_metal_command_list_destroy(kore_gpu_command_list *list);

void kore_metal_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                   uint64_t destination_offset, uint64_t size);

void kore_metal_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                    const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers);

void kore_metal_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                    const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers);

void kore_metal_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers);

void kore_metal_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size);

void kore_metal_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters);

void kore_metal_command_list_end_render_pass(kore_gpu_command_list *list);

void kore_metal_command_list_present(kore_gpu_command_list *list);

void kore_metal_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth);

void kore_metal_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void kore_metal_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color);

void kore_metal_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference);

void kore_metal_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset,
                                              uint64_t size);

void kore_metal_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_metal_buffer *buffer, uint64_t offset, uint64_t size,
                                               uint64_t stride);

void kore_metal_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_metal_render_pipeline *pipeline);

void kore_metal_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);

void kore_metal_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t base_vertex,
                                          uint32_t first_instance);

void kore_metal_command_list_set_descriptor_set(kore_gpu_command_list *list, struct kore_metal_descriptor_set *set, kore_gpu_buffer **dynamic_buffers, uint32_t *dynamic_offsets, uint32_t *dynamic_sizes, uint32_t dynamic_count);

void kore_metal_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size);

void kore_metal_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_metal_compute_pipeline *pipeline);

void kore_metal_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z);

void kore_metal_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, kore_gpu_raytracing_volume *volume);

void kore_metal_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, kore_gpu_raytracing_hierarchy *hierarchy);

void kore_metal_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                         kore_gpu_raytracing_hierarchy *hierarchy);

void kore_metal_command_list_set_ray_pipeline(kore_gpu_command_list *list, kore_metal_ray_pipeline *pipeline);

void kore_metal_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth);

void kore_metal_command_list_set_name(kore_gpu_command_list *list, const char *name);

void kore_metal_command_list_push_debug_group(kore_gpu_command_list *list, const char *name);

void kore_metal_command_list_pop_debug_group(kore_gpu_command_list *list);

void kore_metal_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name);

void kore_metal_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index);

void kore_metal_command_list_end_occlusion_query(kore_gpu_command_list *list);

void kore_metal_command_list_resolve_query_set(kore_gpu_command_list *list, kore_gpu_query_set *query_set, uint32_t first_query, uint32_t query_count,
                                               kore_gpu_buffer *destination, uint64_t destination_offset);

void kore_metal_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset, uint32_t max_draw_count,
                                           kore_gpu_buffer *count_buffer, uint64_t count_offset);

void kore_metal_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                   uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset);

void kore_metal_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset);

void kore_metal_command_list_queue_buffer_access(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size);

void kore_metal_command_list_queue_descriptor_set_access(kore_gpu_command_list *list, struct kore_metal_descriptor_set *descriptor_set);

#ifdef __cplusplus
}
#endif

#endif
