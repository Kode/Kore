#include <kore3/gpu/commandlist.h>

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/commandlist_functions.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/commandlist_functions.h>
#elif defined(KORE_METAL)
#include <kore3/metal/commandlist_functions.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/commandlist_functions.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/commandlist_functions.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/commandlist_functions.h>
#else
#error("Unknown GPU backend")
#endif

#include <assert.h>

void kore_gpu_command_list_destroy(kore_gpu_command_list *list) {
	KORE_GPU_CALL1(command_list_destroy, list);
}

void kore_gpu_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                 uint64_t destination_offset, uint64_t size) {
	KORE_GPU_CALL6(command_list_copy_buffer_to_buffer, list, source, source_offset, destination, destination_offset, size);
}

void kore_gpu_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                  const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                  uint32_t depth_or_array_layers) {
#ifdef KORE_GPU_VALIDATION
	assert(source->offset % 512 == 0); // D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT
#endif
	KORE_GPU_CALL6(command_list_copy_buffer_to_texture, list, source, destination, width, height, depth_or_array_layers);
}

void kore_gpu_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                  const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                  uint32_t depth_or_array_layers) {
	KORE_GPU_CALL6(command_list_copy_texture_to_buffer, list, source, destination, width, height, depth_or_array_layers);
}

void kore_gpu_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                   const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                   uint32_t depth_or_array_layers) {
#ifdef KORE_GPU_VALIDATION
	assert(source->texture->validation_format == destination->texture->validation_format);
#endif
	KORE_GPU_CALL6(command_list_copy_texture_to_texture, list, source, destination, width, height, depth_or_array_layers);
}

void kore_gpu_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {
	KORE_GPU_CALL4(command_list_clear_buffer, list, buffer, offset, size);
}

void kore_gpu_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	KORE_GPU_CALL2(command_list_begin_render_pass, list, parameters);
}

void kore_gpu_command_list_end_render_pass(kore_gpu_command_list *list) {
	KORE_GPU_CALL1(command_list_end_render_pass, list);
}

void kore_gpu_command_list_present(kore_gpu_command_list *list) {
	KORE_GPU_CALL1(command_list_present, list);
}

void kore_gpu_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset,
                                            uint64_t size) {
	KORE_GPU_CALL5(command_list_set_index_buffer, list, buffer, index_format, offset, size);
}

void kore_gpu_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
	KORE_GPU_CALL5(command_list_draw, list, vertex_count, instance_count, first_vertex, first_instance);
}

void kore_gpu_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t base_vertex,
                                        uint32_t first_instance) {
	KORE_GPU_CALL6(command_list_draw_indexed, list, index_count, instance_count, first_index, base_vertex, first_instance);
}

void kore_gpu_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {
	KORE_GPU_CALL4(command_list_compute, list, workgroup_count_x, workgroup_count_y, workgroup_count_z);
}

void kore_gpu_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, kore_gpu_raytracing_volume *volume) {
	KORE_GPU_CALL2(command_list_prepare_raytracing_volume, list, volume);
}
void kore_gpu_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, kore_gpu_raytracing_hierarchy *hierarchy) {
	KORE_GPU_CALL2(command_list_prepare_raytracing_hierarchy, list, hierarchy);
}

void kore_gpu_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                       kore_gpu_raytracing_hierarchy *hierarchy) {
	KORE_GPU_CALL4(command_list_update_raytracing_hierarchy, list, volume_transforms, volumes_count, hierarchy);
}

void kore_gpu_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth) {
	KORE_GPU_CALL4(command_list_trace_rays, list, width, height, depth);
}

void kore_gpu_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth) {
	KORE_GPU_CALL7(command_list_set_viewport, list, x, y, width, height, min_depth, max_depth);
}

void kore_gpu_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	KORE_GPU_CALL5(command_list_set_scissor_rect, list, x, y, width, height);
}

void kore_gpu_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color) {
	KORE_GPU_CALL2(command_list_set_blend_constant, list, color);
}

void kore_gpu_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference) {
	KORE_GPU_CALL2(command_list_set_stencil_reference, list, reference);
}

void kore_gpu_command_list_set_name(kore_gpu_command_list *list, const char *name) {
	KORE_GPU_CALL2(command_list_set_name, list, name);
}

void kore_gpu_command_list_push_debug_group(kore_gpu_command_list *list, const char *name) {
	KORE_GPU_CALL2(command_list_push_debug_group, list, name);
}

void kore_gpu_command_list_pop_debug_group(kore_gpu_command_list *list) {
	KORE_GPU_CALL1(command_list_pop_debug_group, list);
}

void kore_gpu_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name) {
	KORE_GPU_CALL2(command_list_insert_debug_marker, list, name);
}

void kore_gpu_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index) {
	KORE_GPU_CALL2(command_list_begin_occlusion_query, list, query_index);
}

void kore_gpu_command_list_end_occlusion_query(kore_gpu_command_list *list) {
	KORE_GPU_CALL1(command_list_end_occlusion_query, list);
}

void kore_gpu_command_list_resolve_query_set(kore_gpu_command_list *list, kore_gpu_query_set *query_set, uint32_t first_query, uint32_t query_count,
                                             kore_gpu_buffer *destination, uint64_t destination_offset) {
	KORE_GPU_CALL6(command_list_resolve_query_set, list, query_set, first_query, query_count, destination, destination_offset);
}

void kore_gpu_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset, uint32_t max_draw_count,
                                         kore_gpu_buffer *count_buffer, uint64_t count_offset) {
	KORE_GPU_CALL6(command_list_draw_indirect, list, indirect_buffer, indirect_offset, max_draw_count, count_buffer, count_offset);
}

void kore_gpu_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                 uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset) {
	KORE_GPU_CALL6(command_list_draw_indexed_indirect, list, indirect_buffer, indirect_offset, max_draw_count, count_buffer, count_offset);
}

void kore_gpu_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset) {
	KORE_GPU_CALL3(command_list_compute_indirect, list, indirect_buffer, indirect_offset);
}
