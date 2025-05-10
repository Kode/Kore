#include <kore3/direct3d11/commandlist_functions.h>

#include "d3d11unit.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>

#include <kore3/direct3d11/texture_functions.h>

#include <kore3/direct3d11/pipeline_structs.h>

#include <kore3/util/align.h>

#include <assert.h>
#include <stdlib.h>

typedef struct set_index_buffer_data {
	kore_gpu_buffer      *buffer;
	kore_gpu_index_format index_format;
	uint64_t              offset;
} set_index_buffer_data;

typedef struct set_vertex_buffer_data {
	uint32_t           slot;
	kore_d3d11_buffer *buffer;
	uint64_t           offset;
	uint64_t           size;
	uint64_t           stride;
} set_vertex_buffer_data;

typedef struct draw_indexed_data {
	uint32_t index_count;
	uint32_t instance_count;
	uint32_t first_index;
	int32_t  base_vertex;
	uint32_t first_instance;
} draw_indexed_data;

typedef struct set_render_pipeline {
	kore_d3d11_render_pipeline *pipeline;
} set_render_pipeline;

typedef struct copy_texture_to_buffer {
	const kore_gpu_image_copy_texture *source;
	const kore_gpu_image_copy_buffer  *destination;
	uint32_t                           width;
	uint32_t                           height;
	uint32_t                           depth_or_array_layers;
} copy_texture_to_buffer;

typedef struct begin_render_pass {
	kore_gpu_render_pass_parameters parameters;
} begin_render_pass;

typedef struct command {
	command_type type;
	uint32_t     size;
	uint32_t     data;
} command;

void kore_d3d11_command_list_destroy(kore_gpu_command_list *list) {
	free(list->d3d11.commands);
	list->d3d11.commands = NULL;
}

void kore_d3d11_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_BEGIN_RENDER_PASS;

	begin_render_pass *data = (begin_render_pass *)&c->data;
	data->parameters        = *parameters;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_end_render_pass(kore_gpu_command_list *list) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_END_RENDER_PASS;

	c->size = sizeof(command) - sizeof(c->data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_present(kore_gpu_command_list *list) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_PRESENT;

	c->size = sizeof(command) - sizeof(c->data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_SET_INDEX_BUFFER;

	set_index_buffer_data *data = (set_index_buffer_data *)&c->data;
	data->buffer                = buffer;
	data->index_format          = index_format;
	data->offset                = offset;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_d3d11_buffer *buffer, uint64_t offset, uint64_t size,
                                               uint64_t stride) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_SET_VERTEX_BUFFER;

	set_vertex_buffer_data *data = (set_vertex_buffer_data *)&c->data;
	data->slot                   = slot;
	data->buffer                 = buffer;
	data->offset                 = offset;
	data->size                   = size;
	data->stride                 = stride;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_d3d11_render_pipeline *pipeline) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_SET_RENDER_PIPELINE;

	set_render_pipeline *data = (set_render_pipeline *)&c->data;
	data->pipeline            = pipeline;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
}

void kore_d3d11_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t base_vertex,
                                          uint32_t first_instance) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_DRAW_INDEXED;

	draw_indexed_data *data = (draw_indexed_data *)&c->data;
	data->index_count       = index_count;
	data->instance_count    = instance_count;
	data->first_index       = first_index;
	data->base_vertex       = base_vertex;
	data->first_instance    = first_instance;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_set_descriptor_table(kore_gpu_command_list *list, uint32_t table_index, kore_d3d11_descriptor_set *set,
                                                  kore_gpu_buffer **dynamic_buffers, uint32_t *dynamic_offsets, uint32_t *dynamic_sizes) {}

void kore_d3d11_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size) {}

void kore_d3d11_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                   uint64_t destination_offset, uint64_t size) {}

void kore_d3d11_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                    const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers) {}

void kore_d3d11_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                    const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                    uint32_t depth_or_array_layers) {
	command *c = (command *)&list->d3d11.commands[list->d3d11.commands_offset];

	c->type = COMMAND_COPY_TEXTURE_TO_BUFFER;

	copy_texture_to_buffer *data = (copy_texture_to_buffer *)&c->data;
	data->source                 = source;
	data->destination            = destination;
	data->width                  = width;
	data->height                 = height;
	data->depth_or_array_layers  = depth_or_array_layers;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->d3d11.commands_offset += c->size;
}

void kore_d3d11_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {}

void kore_d3d11_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {}

void kore_d3d11_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_d3d11_compute_pipeline *pipeline) {}

void kore_d3d11_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {}

void kore_d3d11_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, kore_gpu_raytracing_volume *volume) {}

void kore_d3d11_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_d3d11_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                         kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_d3d11_command_list_set_ray_pipeline(kore_gpu_command_list *list, kore_d3d11_ray_pipeline *pipeline) {}

void kore_d3d11_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth) {}

void kore_d3d11_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth) {}

void kore_d3d11_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

void kore_d3d11_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color) {}

void kore_d3d11_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference) {}

void kore_d3d11_command_list_set_name(kore_gpu_command_list *list, const char *name) {}

void kore_d3d11_command_list_push_debug_group(kore_gpu_command_list *list, const char *name) {}

void kore_d3d11_command_list_pop_debug_group(kore_gpu_command_list *list) {}

void kore_d3d11_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name) {}

void kore_d3d11_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index) {}

void kore_d3d11_command_list_end_occlusion_query(kore_gpu_command_list *list) {}

void kore_d3d11_command_list_resolve_query_set(kore_gpu_command_list *list, kore_gpu_query_set *query_set, uint32_t first_query, uint32_t query_count,
                                               kore_gpu_buffer *destination, uint64_t destination_offset) {}

void kore_d3d11_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset, uint32_t max_draw_count,
                                           kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_d3d11_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                   uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_d3d11_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset) {}

void kore_d3d11_command_list_queue_buffer_access(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {}

void kore_d3d11_command_list_queue_descriptor_set_access(kore_gpu_command_list *list, kore_d3d11_descriptor_set *descriptor_set) {}
