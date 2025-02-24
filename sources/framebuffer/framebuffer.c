#include <kore3/framebuffer/framebuffer.h>

#if 0

#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/texture.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/io/filereader.h>
#include <kinc/log.h>

#if 0

#ifdef KORE_KONG
#include <kong.h>
#endif

#ifndef KORE_KONG
static kore_g4_shader_t vertexShader;
static kore_g4_shader_t fragmentShader;
static kore_g4_pipeline_t pipeline;
static kore_g4_texture_unit_t tex;
kore_g1_texture_filter_t kore_internal_g1_texture_filter_min = KORE_G1_TEXTURE_FILTER_LINEAR;
kore_g1_texture_filter_t kore_internal_g1_texture_filter_mag = KORE_G1_TEXTURE_FILTER_LINEAR;
kore_g1_mipmap_filter_t kore_internal_g1_mipmap_filter = KORE_G1_MIPMAP_FILTER_NONE;
#endif
static kore_g4_vertex_buffer_t vb;
static kore_g4_index_buffer_t ib;
static kore_g4_texture_t texture;

uint32_t *kore_internal_g1_image;
int kore_internal_g1_w, kore_internal_g1_h, kore_internal_g1_tex_width;

void kore_g1_begin(void) {
	kore_g4_begin(0);
	kore_internal_g1_image = (uint32_t *)kore_g4_texture_lock(&texture);
}

static inline kore_g4_texture_filter_t map_texture_filter(kore_g1_texture_filter_t filter) {
	switch (filter) {
	case KORE_G1_TEXTURE_FILTER_POINT:
		return KORE_G4_TEXTURE_FILTER_POINT;
	case KORE_G1_TEXTURE_FILTER_LINEAR:
		return KORE_G4_TEXTURE_FILTER_LINEAR;
	case KORE_G1_TEXTURE_FILTER_ANISOTROPIC:
		return KORE_G4_TEXTURE_FILTER_ANISOTROPIC;
	}

	kore_log(KORE_LOG_LEVEL_WARNING, "unhandled kore_g1_texture_filter_t (%i)", filter);
	return KORE_G4_TEXTURE_FILTER_LINEAR;
}

static inline kore_g4_mipmap_filter_t map_mipmap_filter(kore_g1_mipmap_filter_t filter) {
	switch (filter) {
	case KORE_G1_MIPMAP_FILTER_NONE:
		return KORE_G4_MIPMAP_FILTER_NONE;
	case KORE_G1_MIPMAP_FILTER_POINT:
		return KORE_G4_MIPMAP_FILTER_POINT;
	case KORE_G1_MIPMAP_FILTER_LINEAR:
		return KORE_G4_MIPMAP_FILTER_LINEAR;
	}

	kore_log(KORE_LOG_LEVEL_WARNING, "unhandled kore_g1_mipmap_filter_t (%i)", filter);
	return KORE_G4_MIPMAP_FILTER_NONE;
}

void kore_g1_end(void) {
	kore_internal_g1_image = NULL;
	kore_g4_texture_unlock(&texture);

	kore_g4_clear(KORE_G4_CLEAR_COLOR, 0xff000000, 0.0f, 0);

#ifdef KORE_KONG
	kore_g4_set_pipeline(&kore_g1_pipeline);
#else
	kore_g4_set_pipeline(&pipeline);
#endif

#ifndef KORE_KONG
	kore_g4_set_texture(tex, &texture);
	kore_g4_set_texture_minification_filter(tex, map_texture_filter(kore_internal_g1_texture_filter_min));
	kore_g4_set_texture_magnification_filter(tex, map_texture_filter(kore_internal_g1_texture_filter_mag));
	kore_g4_set_texture_mipmap_filter(tex, map_mipmap_filter(kore_internal_g1_mipmap_filter));
#endif
	kore_g4_set_vertex_buffer(&vb);
	kore_g4_set_index_buffer(&ib);
	kore_g4_draw_indexed_vertices();

	kore_g4_end(0);
	kore_g4_swap_buffers();
}

void kore_g1_init(int width, int height) {
	kore_internal_g1_w = width;
	kore_internal_g1_h = height;

#ifndef KORE_KONG
	{
		kore_file_reader_t file;
		kore_file_reader_open(&file, "g1.vert", KORE_FILE_TYPE_ASSET);
		void *data = malloc(kore_file_reader_size(&file));
		kore_file_reader_read(&file, data, kore_file_reader_size(&file));
		kore_file_reader_close(&file);
		kore_g4_shader_init(&vertexShader, data, kore_file_reader_size(&file), KORE_G4_SHADER_TYPE_VERTEX);
		free(data);
	}

	{
		kore_file_reader_t file;
		kore_file_reader_open(&file, "g1.frag", KORE_FILE_TYPE_ASSET);
		void *data = malloc(kore_file_reader_size(&file));
		kore_file_reader_read(&file, data, kore_file_reader_size(&file));
		kore_file_reader_close(&file);
		kore_g4_shader_init(&fragmentShader, data, kore_file_reader_size(&file), KORE_G4_SHADER_TYPE_FRAGMENT);
		free(data);
	}

	kore_g4_vertex_structure_t structure;
	kore_g4_vertex_structure_init(&structure);
	kore_g4_vertex_structure_add(&structure, "pos", KORE_G4_VERTEX_DATA_F32_3X);
	kore_g4_vertex_structure_add(&structure, "tex", KORE_G4_VERTEX_DATA_F32_2X);
	kore_g4_pipeline_init(&pipeline);
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	pipeline.vertex_shader = &vertexShader;
	pipeline.fragment_shader = &fragmentShader;
	kore_g4_pipeline_compile(&pipeline);

	tex = kore_g4_pipeline_get_texture_unit(&pipeline, "texy");
#endif

	kore_g4_texture_init(&texture, width, height, KORE_IMAGE_FORMAT_RGBA32);
	kore_internal_g1_tex_width = texture.tex_width;

	kore_internal_g1_image = (uint32_t *)kore_g4_texture_lock(&texture);
	int stride = kore_g4_texture_stride(&texture) / 4;
	for (int y = 0; y < texture.tex_height; ++y) {
		for (int x = 0; x < texture.tex_width; ++x) {
			kore_internal_g1_image[y * stride + x] = 0;
		}
	}
	kore_g4_texture_unlock(&texture);

	// Correct for the difference between the texture's desired size and the actual power of 2 size
	float xAspect = (float)width / texture.tex_width;
	float yAspect = (float)height / texture.tex_height;

#ifdef KORE_KONG
	kore_g4_vertex_buffer_init(&vb, 4, &kore_g1_vertex_in_structure, KORE_G4_USAGE_STATIC, 0);
#else
	kore_g4_vertex_buffer_init(&vb, 4, &structure, KORE_G4_USAGE_STATIC, 0);
#endif
	float *v = kore_g4_vertex_buffer_lock_all(&vb);
	{
		int i = 0;
		v[i++] = -1;
		v[i++] = 1;
		v[i++] = 0.5;
		v[i++] = 0;
		v[i++] = 0;
		v[i++] = 1;
		v[i++] = 1;
		v[i++] = 0.5;
		v[i++] = xAspect;
		v[i++] = 0;
		v[i++] = 1;
		v[i++] = -1;
		v[i++] = 0.5;
		v[i++] = xAspect;
		v[i++] = yAspect;
		v[i++] = -1;
		v[i++] = -1;
		v[i++] = 0.5;
		v[i++] = 0;
		v[i++] = yAspect;
	}
	kore_g4_vertex_buffer_unlock_all(&vb);

	kore_g4_index_buffer_init(&ib, 6, KORE_G4_INDEX_BUFFER_FORMAT_32BIT, KORE_G4_USAGE_STATIC);
	uint32_t *ii = (uint32_t *)kore_g4_index_buffer_lock_all(&ib);
	{
		int i = 0;
		ii[i++] = 0;
		ii[i++] = 1;
		ii[i++] = 3;
		ii[i++] = 1;
		ii[i++] = 2;
		ii[i++] = 3;
	}
	kore_g4_index_buffer_unlock_all(&ib);
}

#if defined(KORE_DYNAMIC_COMPILE) || defined(KORE_DYNAMIC)

void kore_g1_set_pixel(int x, int y, float red, float green, float blue) {
	if (x < 0 || x >= kore_internal_g1_w || y < 0 || y >= kore_internal_g1_h)
		return;
	int r = (int)(red * 255);
	int g = (int)(green * 255);
	int b = (int)(blue * 255);
	kore_internal_g1_image[y * kore_internal_g1_tex_width + x] = 0xff << 24 | b << 16 | g << 8 | r;
}

int kore_g1_width() {
	return kore_internal_g1_w;
}

int kore_g1_height() {
	return kore_internal_g1_h;
}

void kore_g1_set_texture_magnification_filter(kore_g1_texture_filter_t filter) {
	kore_internal_g1_texture_filter_mag = filter;
}

void kore_g1_set_texture_minification_filter(kore_g1_texture_filter_t filter) {
	kore_internal_g1_texture_filter_min = filter;
}

void kore_g1_set_texture_mipmap_filter(kore_g1_mipmap_filter_t filter) {
	kore_internal_g1_mipmap_filter = filter;
}

#endif

#endif

#endif
