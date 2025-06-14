#include <kore3/opengl/commandlist_functions.h>

#include "openglunit.h"

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>

#include <kore3/opengl/texture_functions.h>

#include <kore3/opengl/pipeline_structs.h>

#include <kore3/util/align.h>

#include <assert.h>
#include <stdlib.h>

typedef enum command_type {
	COMMAND_SET_INDEX_BUFFER,
	COMMAND_SET_VERTEX_BUFFER,
	COMMAND_DRAW_INDEXED,
	COMMAND_SET_RENDER_PIPELINE,
	COMMAND_COPY_BUFFER_TO_TEXTURE,
	COMMAND_COPY_TEXTURE_TO_BUFFER,
	COMMAND_COPY_TEXTURE_TO_TEXTURE,
	COMMAND_SET_UNIFORM_BUFFER,
	COMMAND_SET_TEXTURE,
	COMMAND_SET_SAMPLER,
	COMMAND_PRESENT,
	COMMAND_BEGIN_RENDER_PASS,
	COMMAND_END_RENDER_PASS,
} command_type;

typedef struct set_index_buffer_data {
	kore_gpu_buffer      *buffer;
	kore_gpu_index_format index_format;
	uint64_t              offset;
} set_index_buffer_data;

typedef struct set_vertex_buffer_data {
	uint32_t            slot;
	kore_opengl_buffer *buffer;
	uint64_t            offset;
	uint64_t            size;
	uint64_t            stride;
} set_vertex_buffer_data;

typedef struct draw_indexed_data {
	uint32_t index_count;
	uint32_t instance_count;
	uint32_t first_index;
	int32_t  base_vertex;
	uint32_t first_instance;
} draw_indexed_data;

typedef struct set_render_pipeline {
	kore_opengl_render_pipeline *pipeline;
} set_render_pipeline;

typedef struct copy_buffer_to_texture {
	kore_gpu_image_copy_buffer  source;
	kore_gpu_image_copy_texture destination;
	uint32_t                    width;
	uint32_t                    height;
	uint32_t                    depth_or_array_layers;
} copy_buffer_to_texture;

typedef struct copy_texture_to_buffer {
	kore_gpu_image_copy_texture source;
	kore_gpu_image_copy_buffer  destination;
	uint32_t                    width;
	uint32_t                    height;
	uint32_t                    depth_or_array_layers;
} copy_texture_to_buffer;

typedef struct copy_texture_to_texture {
	kore_gpu_image_copy_texture source;
	kore_gpu_image_copy_texture destination;
	uint32_t                    width;
	uint32_t                    height;
	uint32_t                    depth_or_array_layers;
} copy_texture_to_texture;

typedef struct set_uniform_buffer {
	kore_gpu_buffer *buffer;
	uint32_t         uniform_block_index;
	uint32_t         offset;
	uint32_t         size;
} set_uniform_buffer;

typedef struct set_texture {
	kore_gpu_texture_view view;
} set_texture;

typedef struct set_sampler {
	kore_gpu_sampler sampler;
} set_sampler;

typedef struct begin_render_pass {
	kore_gpu_render_pass_parameters parameters;
} begin_render_pass;

typedef struct command {
	command_type type;
	uint32_t     size;
	uint32_t     data;
} command;

void kore_opengl_command_list_destroy(kore_gpu_command_list *list) {
	free(list->opengl.commands);
	list->opengl.commands = NULL;
}

void kore_opengl_command_list_begin_render_pass(kore_gpu_command_list *list, const kore_gpu_render_pass_parameters *parameters) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_BEGIN_RENDER_PASS;

	begin_render_pass *data = (begin_render_pass *)&c->data;
	data->parameters        = *parameters;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_end_render_pass(kore_gpu_command_list *list) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_END_RENDER_PASS;

	c->size = sizeof(command) - sizeof(c->data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_present(kore_gpu_command_list *list) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_PRESENT;

	c->size = sizeof(command) - sizeof(c->data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_set_index_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, kore_gpu_index_format index_format, uint64_t offset) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_SET_INDEX_BUFFER;

	set_index_buffer_data *data = (set_index_buffer_data *)&c->data;
	data->buffer                = buffer;
	data->index_format          = index_format;
	data->offset                = offset;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_set_vertex_buffer(kore_gpu_command_list *list, uint32_t slot, kore_opengl_buffer *buffer, uint64_t offset, uint64_t size,
                                                uint64_t stride) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_SET_VERTEX_BUFFER;

	set_vertex_buffer_data *data = (set_vertex_buffer_data *)&c->data;
	data->slot                   = slot;
	data->buffer                 = buffer;
	data->offset                 = offset;
	data->size                   = size;
	data->stride                 = stride;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_set_render_pipeline(kore_gpu_command_list *list, kore_opengl_render_pipeline *pipeline) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_SET_RENDER_PIPELINE;

	set_render_pipeline *data = (set_render_pipeline *)&c->data;
	data->pipeline            = pipeline;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_draw(kore_gpu_command_list *list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                                   uint32_t first_instance) {}

void kore_opengl_command_list_draw_indexed(kore_gpu_command_list *list, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                                           int32_t base_vertex, uint32_t first_instance) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_DRAW_INDEXED;

	draw_indexed_data *data = (draw_indexed_data *)&c->data;
	data->index_count       = index_count;
	data->instance_count    = instance_count;
	data->first_index       = first_index;
	data->base_vertex       = base_vertex;
	data->first_instance    = first_instance;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_set_root_constants(kore_gpu_command_list *list, uint32_t table_index, const void *data, size_t data_size) {}

void kore_opengl_command_list_copy_buffer_to_buffer(kore_gpu_command_list *list, kore_gpu_buffer *source, uint64_t source_offset, kore_gpu_buffer *destination,
                                                    uint64_t destination_offset, uint64_t size) {}

void kore_opengl_command_list_copy_buffer_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_buffer *source,
                                                     const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_COPY_BUFFER_TO_TEXTURE;

	copy_buffer_to_texture *data = (copy_buffer_to_texture *)&c->data;
	data->source                 = *source;
	data->destination            = *destination;
	data->width                  = width;
	data->height                 = height;
	data->depth_or_array_layers  = depth_or_array_layers;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_copy_texture_to_buffer(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                     const kore_gpu_image_copy_buffer *destination, uint32_t width, uint32_t height,
                                                     uint32_t depth_or_array_layers) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_COPY_TEXTURE_TO_BUFFER;

	copy_texture_to_buffer *data = (copy_texture_to_buffer *)&c->data;
	data->source                 = *source;
	data->destination            = *destination;
	data->width                  = width;
	data->height                 = height;
	data->depth_or_array_layers  = depth_or_array_layers;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_copy_texture_to_texture(kore_gpu_command_list *list, const kore_gpu_image_copy_texture *source,
                                                      const kore_gpu_image_copy_texture *destination, uint32_t width, uint32_t height,
                                                      uint32_t depth_or_array_layers) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_COPY_TEXTURE_TO_TEXTURE;

	copy_texture_to_texture *data = (copy_texture_to_texture *)&c->data;
	data->source                  = *source;
	data->destination             = *destination;
	data->width                   = width;
	data->height                  = height;
	data->depth_or_array_layers   = depth_or_array_layers;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_clear_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, size_t offset, uint64_t size) {}

void kore_opengl_command_list_set_compute_pipeline(kore_gpu_command_list *list, kore_opengl_compute_pipeline *pipeline) {}

void kore_opengl_command_list_compute(kore_gpu_command_list *list, uint32_t workgroup_count_x, uint32_t workgroup_count_y, uint32_t workgroup_count_z) {}

void kore_opengl_command_list_prepare_raytracing_volume(kore_gpu_command_list *list, kore_gpu_raytracing_volume *volume) {}

void kore_opengl_command_list_prepare_raytracing_hierarchy(kore_gpu_command_list *list, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_opengl_command_list_update_raytracing_hierarchy(kore_gpu_command_list *list, kore_matrix4x4 *volume_transforms, uint32_t volumes_count,
                                                          kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_opengl_command_list_set_ray_pipeline(kore_gpu_command_list *list, kore_opengl_ray_pipeline *pipeline) {}

void kore_opengl_command_list_trace_rays(kore_gpu_command_list *list, uint32_t width, uint32_t height, uint32_t depth) {}

void kore_opengl_command_list_set_viewport(kore_gpu_command_list *list, float x, float y, float width, float height, float min_depth, float max_depth) {}

void kore_opengl_command_list_set_scissor_rect(kore_gpu_command_list *list, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

void kore_opengl_command_list_set_blend_constant(kore_gpu_command_list *list, kore_gpu_color color) {}

void kore_opengl_command_list_set_stencil_reference(kore_gpu_command_list *list, uint32_t reference) {}

void kore_opengl_command_list_set_name(kore_gpu_command_list *list, const char *name) {}

void kore_opengl_command_list_push_debug_group(kore_gpu_command_list *list, const char *name) {}

void kore_opengl_command_list_pop_debug_group(kore_gpu_command_list *list) {}

void kore_opengl_command_list_insert_debug_marker(kore_gpu_command_list *list, const char *name) {}

void kore_opengl_command_list_begin_occlusion_query(kore_gpu_command_list *list, uint32_t query_index) {}

void kore_opengl_command_list_end_occlusion_query(kore_gpu_command_list *list) {}

void kore_opengl_command_list_resolve_query_set(kore_gpu_command_list *list, kore_gpu_query_set *query_set, uint32_t first_query, uint32_t query_count,
                                                kore_gpu_buffer *destination, uint64_t destination_offset) {}

void kore_opengl_command_list_draw_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset, uint32_t max_draw_count,
                                            kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_opengl_command_list_draw_indexed_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset,
                                                    uint32_t max_draw_count, kore_gpu_buffer *count_buffer, uint64_t count_offset) {}

void kore_opengl_command_list_compute_indirect(kore_gpu_command_list *list, kore_gpu_buffer *indirect_buffer, uint64_t indirect_offset) {}

void kore_opengl_command_list_queue_buffer_access(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {}

void kore_opengl_command_list_queue_descriptor_set_access(kore_gpu_command_list *list, kore_opengl_descriptor_set *descriptor_set) {}

void kore_opengl_command_list_set_uniform_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t uniform_block_index, uint32_t offset,
                                                 uint32_t size) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_SET_UNIFORM_BUFFER;

	set_uniform_buffer *data  = (set_uniform_buffer *)&c->data;
	data->buffer              = buffer;
	data->uniform_block_index = uniform_block_index;
	data->offset              = offset;
	data->size                = size;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_set_texture(kore_gpu_command_list *list, kore_gpu_texture_view *view, bool writable) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_SET_TEXTURE;

	set_texture *data = (set_texture *)&c->data;
	data->view        = *view;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}

void kore_opengl_command_list_set_sampler(kore_gpu_command_list *list, kore_gpu_sampler *sampler) {
	command *c = (command *)&list->opengl.commands[list->opengl.commands_offset];

	c->type = COMMAND_SET_SAMPLER;

	set_sampler *data = (set_sampler *)&c->data;
	data->sampler     = *sampler;

	c->size = sizeof(command) - sizeof(c->data) + sizeof(*data);
	list->opengl.commands_offset += c->size;
}
