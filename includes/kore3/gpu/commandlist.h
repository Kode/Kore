#ifndef KORE_GPU_COMMANDLIST_HEADER
#define KORE_GPU_COMMANDLIST_HEADER

#include <kore3/global.h>

#include <kore3/math/matrix.h>

#include "api.h"
#include "buffer.h"
#include "texture.h"

#ifdef KORE_DIRECT3D12
#include <kore3/direct3d12/commandlist_structs.h>
#endif

#ifdef KORE_METAL
#include <kore3/metal/commandlist_structs.h>
#endif

#ifdef KORE_VULKAN
#include <kore3/vulkan/commandlist_structs.h>
#endif

#ifdef KORE_WEBGPU
#include <kore3/webgpu/commandlist_structs.h>
#endif

#include <kore3/math/vector.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_query_set;

typedef struct kore_gpu_command_list {
	KORE_GPU_IMPL(command_list);
} kore_gpu_command_list;

KORE_FUNC void kore_gpu_command_list_destroy(kore_gpu_command_list *list);

KORE_FUNC void kore_gpu_command_list_set_name(kore_gpu_command_list *list, const char *name);

KORE_FUNC void kore_gpu_command_list_push_debug_group(kore_gpu_command_list *list, const char *name);

KORE_FUNC void kore_gpu_command_list_pop_debug_group(kore_gpu_command_list *list);

KORE_FUNC void kore_gpu_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name);

typedef enum kore_gpu_load_op { KORE_GPU_LOAD_OP_LOAD, KORE_GPU_LOAD_OP_CLEAR } kore_gpu_load_op;

typedef enum kore_gpu_store_op { KORE_GPU_STORE_OP_STORE, KORE_GPU_STORE_OP_DISCARD } kore_gpu_store_op;

typedef struct kore_gpu_color {
	float r;
	float g;
	float b;
	float a;
} kore_gpu_color;

typedef struct kore_gpu_render_pass_color_attachment {
	kore_gpu_texture_view texture;
	uint32_t depth_slice;
	kore_gpu_texture_view resolve_target;
	kore_gpu_color clear_value;
	kore_gpu_load_op load_op;
	kore_gpu_store_op store_op;
} kore_gpu_render_pass_color_attachment;

typedef struct kore_gpu_render_pass_depth_stencil_attachment {
	kore_gpu_texture *texture;
	float depth_clear_value;
	kore_gpu_load_op depth_load_op;
	kore_gpu_store_op depth_store_op;
	bool depth_read_only;
	uint32_t stencil_clear_value;
	kore_gpu_load_op stencil_load_op;
	kore_gpu_store_op stencil_store_op;
	bool stencil_read_only;
} kore_gpu_render_pass_depth_stencil_attachment;

typedef struct kore_gpu_render_pass_timestamp_writes {
	struct kore_gpu_query_set *query_set;
	uint32_t beginning_of_pass_write_index;
	uint32_t end_of_pass_write_index;
} kore_gpu_render_pass_timestamp_writes;

typedef struct kore_gpu_render_pass_parameters {
	kore_gpu_render_pass_color_attachment color_attachments[8];
	size_t color_attachments_count;
	kore_gpu_render_pass_depth_stencil_attachment depth_stencil_attachment;
	struct kore_gpu_query_set *occlusion_query_set;
	kore_gpu_render_pass_timestamp_writes timestamp_writes;
} kore_gpu_render_pass_parameters;

KORE_FUNC void kore_gpu_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters);

typedef struct kore_gpu_image_copy_texture {
	kore_gpu_texture_aspect aspect;
	uint32_t mip_level;
	uint32_t origin_x;
	uint32_t origin_y;
	uint32_t origin_z;
	kore_gpu_texture *texture;
} kore_gpu_image_copy_texture;

typedef struct kore_gpu_image_copy_buffer {
	kore_gpu_buffer *buffer;
	uint64_t offset;
	uint32_t bytes_per_row;
	uint32_t rows_per_image;
} kore_gpu_image_copy_buffer;

KORE_FUNC void kore_gpu_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset,
                                                           kore_gpu_buffer *destination, uint64_t destination_offset, uint64_t size);

KORE_FUNC void kore_gpu_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                            const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                            uint32_t depth_or_array_layers);

KORE_FUNC void kore_gpu_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                            const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                            uint32_t depth_or_array_layers);

KORE_FUNC void kore_gpu_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                             const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                             uint32_t depth_or_array_layers);

KORE_FUNC void kore_gpu_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size);

KORE_FUNC void kore_gpu_command_list_resolve_query_set(kore_gpu_command_list *list, struct kore_gpu_query_set *query_set, uint32_t first_query,
                                                       uint32_t query_count, kore_gpu_buffer *destination, uint64_t destination_offset);

typedef enum kore_gpu_index_format { KORE_GPU_INDEX_FORMAT_UINT16, KORE_GPU_INDEX_FORMAT_UINT32 } kore_gpu_index_format;

KORE_FUNC void kore_gpu_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset,
                                                      uint64_t size);

KORE_FUNC void kore_gpu_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                                          uint32_t first_instance);

KORE_FUNC void kore_gpu_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                                                  int32_t base_vertex, uint32_t first_instance);

typedef struct kore_gpu_draw_arguments {
#ifdef KORE_DIRECT3D12
	uint32_t automatic_draw_index;
#endif
	uint32_t vertex_count;
	uint32_t instance_count;
	uint32_t first_vertex;
	uint32_t first_instance;
} kore_gpu_draw_arguments;

typedef struct kore_gpu_draw_indexed_arguments {
#ifdef KORE_DIRECT3D12
	uint32_t automatic_draw_index;
#endif
	uint32_t index_count;
	uint32_t instance_count;
	uint32_t first_index;
	int32_t base_vertex;
	uint32_t first_instance;
} kore_gpu_draw_indexed_arguments;

typedef struct kore_gpu_compute_arguments {
	uint32_t workgroup_count_x;
	uint32_t workgroup_count_y;
	uint32_t workgroup_count_z;
} kore_gpu_compute_arguments;

KORE_FUNC void kore_gpu_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                   uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset);

KORE_FUNC void kore_gpu_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                           uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset);

KORE_FUNC void kore_gpu_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth);

KORE_FUNC void kore_gpu_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

KORE_FUNC void kore_gpu_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color);

KORE_FUNC void kore_gpu_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference);

KORE_FUNC void kore_gpu_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index);

KORE_FUNC void kore_gpu_command_list_end_occlusion_query(kore_gpu_command_list *list);

KORE_FUNC void kore_gpu_command_list_end_render_pass(kore_gpu_command_list *list);

KORE_FUNC void kore_gpu_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z);

KORE_FUNC void kore_gpu_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset);

struct kore_gpu_raytracing_volume;

KORE_FUNC void kore_gpu_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, struct kore_gpu_raytracing_volume *volume);

struct kore_gpu_raytracing_hierarchy;

KORE_FUNC void kore_gpu_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, struct kore_gpu_raytracing_hierarchy *hierarchy);

KORE_FUNC void kore_gpu_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                                 struct kore_gpu_raytracing_hierarchy *hierarchy);

KORE_FUNC void kore_gpu_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth);

KORE_FUNC void kore_gpu_command_list_present(kore_gpu_command_list *list);

#ifdef __cplusplus
}
#endif

#endif
