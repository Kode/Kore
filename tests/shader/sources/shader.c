#include <kore3/graphics4/graphics.h>
#include <kore3/graphics4/indexbuffer.h>
#include <kore3/graphics4/pipeline.h>
#include <kore3/graphics4/rendertarget.h>
#include <kore3/graphics4/shader.h>
#include <kore3/graphics4/vertexbuffer.h>
#include <kore3/io/filereader.h>
#include <kore3/system.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <assert.h>
#include <stdlib.h>

static kore_g4_shader_t vertex_shader;
static kore_g4_shader_t fragment_shader;
static kore_g4_pipeline_t pipeline;
static kore_g4_vertex_buffer_t vertices;
static kore_g4_index_buffer_t indices;
static kore_g4_render_target_t render_target;

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void update(void *data) {
	kore_g4_begin(0);

	kore_g4_render_target_t *render_targets = {&render_target};
	kore_g4_set_render_targets(&render_targets, 1);

	kore_g4_clear(KORE_G4_CLEAR_COLOR, 0xFF000000, 0.0f, 0);
	kore_g4_set_pipeline(&pipeline);
	kore_g4_set_vertex_buffer(&vertices);
	kore_g4_set_index_buffer(&indices);
	kore_g4_draw_indexed_vertices();

	kore_g4_end(0);
	kore_g4_swap_buffers();

	uint8_t *pixels = (uint8_t *)malloc(2048 * 2048 * 4);
	kore_g4_render_target_get_pixels(&render_target, pixels);
	if (kore_g4_render_targets_inverted_y()) {
		uint8_t *inverted_pixels = (uint8_t *)malloc(2048 * 2048 * 4);
		for (int y = 0; y < 2048; ++y) {
			for (int x = 0; x < 2048; ++x) {
				for (int c = 0; c < 4; ++c) {
					inverted_pixels[y * 2048 * 4 + x * 4 + c] = pixels[(2047 - y) * 2048 * 4 + x * 4 + c];
				}
			}
		}
		pixels = inverted_pixels;
	}
	stbi_write_png("test.png", 2048, 2048, 4, pixels, 2048 * 4);
	exit(0);
}

static void load_shader(const char *filename, kore_g4_shader_t *shader, kore_g4_shader_type_t shader_type) {
	kore_file_reader_t file;
	kore_file_reader_open(&file, filename, KORE_FILE_TYPE_ASSET);
	size_t data_size = kore_file_reader_size(&file);
	uint8_t *data = allocate(data_size);
	kore_file_reader_read(&file, data, data_size);
	kore_file_reader_close(&file);
	kore_g4_shader_init(shader, data, data_size, shader_type);
}

int kickstart(int argc, char **argv) {
	kore_init("Shader", 1024, 768, NULL, NULL);
	kore_set_update_callback(update, NULL);

	heap = (uint8_t *)malloc(HEAP_SIZE);
	assert(heap != NULL);

	load_shader("shader.vert", &vertex_shader, KORE_G4_SHADER_TYPE_VERTEX);
	load_shader("shader.frag", &fragment_shader, KORE_G4_SHADER_TYPE_FRAGMENT);

	kore_g4_vertex_structure_t structure;
	kore_g4_vertex_structure_init(&structure);
	kore_g4_vertex_structure_add(&structure, "pos", KORE_G4_VERTEX_DATA_F32_3X);
	kore_g4_pipeline_init(&pipeline);
	pipeline.vertex_shader = &vertex_shader;
	pipeline.fragment_shader = &fragment_shader;
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	kore_g4_pipeline_compile(&pipeline);

	kore_g4_vertex_buffer_init(&vertices, 3, &structure, KORE_G4_USAGE_STATIC, 0);
	{
		float *v = kore_g4_vertex_buffer_lock_all(&vertices);
		int i = 0;

		v[i++] = -1;
		v[i++] = -1;
		v[i++] = 0.5;

		v[i++] = 1;
		v[i++] = -1;
		v[i++] = 0.5;

		v[i++] = -1;
		v[i++] = 1;
		v[i++] = 0.5;

		kore_g4_vertex_buffer_unlock_all(&vertices);
	}

	kore_g4_index_buffer_init(&indices, 3, KORE_G4_INDEX_BUFFER_FORMAT_16BIT, KORE_G4_USAGE_STATIC);
	{
		uint16_t *i = (uint16_t *)kore_g4_index_buffer_lock_all(&indices);
		i[0] = 0;
		i[1] = 1;
		i[2] = 2;
		kore_g4_index_buffer_unlock_all(&indices);
	}

	kore_g4_render_target_init(&render_target, 2048, 2048, KORE_G4_RENDER_TARGET_FORMAT_32BIT, 16, 0);

	kore_start();

	return 0;
}
